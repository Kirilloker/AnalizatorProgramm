#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

//---------------------------------------------------------------------------------
const string keywords[] = { "for", "do", "int"};

enum state_type { START, IDEN, NUM, ASSIGN, SPR, ARIFM, COM, CONDITIONS, ERROR };
enum lexem_type { KEYWORD, IDENTIFICATOR, NUMBER, OPERATOR, SEPARATOR};

struct lexem
{
	lexem_type name;
	string value;
};

//---------------------------------------------------------------------------------

int lexAnaliz(const char* filename, vector<lexem>* table_lexem);
bool is_keyword(char* lexem);
string get_string_type(lexem_type name);



int main()
{
	vector<lexem> table_lexem;
	const char* filename = "test.txt";

	if (lexAnaliz(filename, &table_lexem) == 0)
		std::cout << "Success!" << std::endl;
	else
		std::cout << "Something wrong..." << std::endl;
	
	std::cout << "Number\t\tLexem\t\tTypeLexem" << std::endl;
;
	for (size_t i = 0; i < table_lexem.size(); i++)
	{
		std::cout << (i + 1) << ":" "\t\t" << table_lexem[i].value << "\t\t" << get_string_type(table_lexem[i].name) << std::endl;;
	}

	return 1;
}




int lexAnaliz(const char* filename, vector<lexem>* table_lexem)
{
	// Для вывода сообщения, о расположение ошибки
	int number_line = 1;
	int number_symbol = 0;

	// Переменные, для того, чтобы корректно обрабатывать лексемы 
	// начинающие с римских цифр, являющиемися идентификаторами
	bool flag_id_with_number = false;
	string buff_id_with_number = "";

	// исходный файл
	FILE* file;

	// Текущий обрабатываемый символ
	char cur_sym;

	// текущая лексема
	lexem cur_lexem;

	fopen_s(&file, filename, "r");

	if (file == NULL)
	{
		std::cout << "Error open file!" << std::endl;
		return -1;
	}

	state_type state = START;

	cur_sym = fgetc(file);
	number_symbol++;

	while(!feof(file))
	{
		switch (state)
		{
		// Начальное состоние. Начинаем обрабатывать новую лексему
		case START:
			// Пропускаем символы пробел, табуляцию и переход 
			// на следующую строчку
			while ((cur_sym == ' ') || (cur_sym == '\t') || (cur_sym == '\n'))
			{
				if (cur_sym == '\n')
				{
					number_line++;
					number_symbol = 0;
				}

				cur_sym = fgetc(file);
				number_symbol++;
			}

			// Если это X V I то переходим в состояние NUM
			if (cur_sym == 'X' || cur_sym == 'V' || cur_sym == 'I')
			{
				state = NUM;
			}

			// Если первый символ буква или _, то переходим в состояния IDEN
			else if (
				((
					(cur_sym >= 'A') && (cur_sym <= 'Z')
				) 
				||
				(
					(cur_sym >= 'a') && (cur_sym <= 'z')
				) 
				|| 
				(cur_sym == '_'))
			   )
			{
				state = IDEN;
			}

			else if (cur_sym == '+' || cur_sym == '-' || cur_sym == '*' ||
					 cur_sym == '/' || cur_sym == '%')
			{
				state = ARIFM;
			}

			// Если символ : то переходим в состояние ASSIGN
			else if (cur_sym == ':')
			{
				state = ASSIGN;
			}

			// Если это ();<>= то переходим в состояние SPR
			else if (cur_sym == '(' || cur_sym == ')' || cur_sym == ';' || 
				     cur_sym == '{' || cur_sym == '}')
			{
				state = SPR;
			}
			else if (cur_sym == '<' || cur_sym == '>' || cur_sym == '=' ||
				cur_sym == '!' )
			{
				state = CONDITIONS;
			}
			// Если не одна проверка не прошла, то переходим в состояние ERROR
			else 
			{
				state = ERROR;
			}
			break;

		// Состояние индентификатор - возможно, это имя переменой или ключевое слово.
		case IDEN:
			
			int size_iden;
			// Сам не знаю почему, но по другому не работает
			size_iden = 0;

			char lexem_iden[32];

			lexem_iden[size_iden] = cur_sym;
			size_iden++;

			cur_sym = fgetc(file);
			number_symbol++;

			// цикл, который будет заполнять lexem_id до тех пор, пока 
			// встречаются символы алфавита, цифры или _
			while (
				((cur_sym >= 'A') && (cur_sym <= 'Z')) || 
				((cur_sym >= 'a') && (cur_sym <= 'z')) || 
				(cur_sym == '_'))
			{
				if (size_iden > 31) break;
				lexem_iden[size_iden] = cur_sym;
				size_iden++;
				cur_sym = fgetc(file);
				number_symbol++;
			}

			// не знаю как по красивее это сделать
			if (size_iden > 31) break;

			lexem_iden[size_iden] = '\0';

			// Если это Ключевое слово, то помечаем имя токена Keyword, иначе Identificator 
			if (is_keyword(lexem_iden))
				cur_lexem.name = KEYWORD;
			else
				cur_lexem.name = IDENTIFICATOR;

			// Идентификатор начинающийся с римским символов
			if (flag_id_with_number == true)
			{
				cur_lexem.value = buff_id_with_number;
				cur_lexem.value += lexem_iden;

				flag_id_with_number = false;
				buff_id_with_number = "";
			}
			else
			{
				cur_lexem.value = lexem_iden;
			}


			(*table_lexem).push_back(cur_lexem);

			state = START;
			break;

		// Состояние числа - возможно что следующая лексема это римское число
		case NUM:
			int size_num;
			size_num = 0;

			char lexem_num[32];

			lexem_num[size_num] = cur_sym;
			size_num++;

			cur_sym = fgetc(file);
			number_symbol++;

			// цикл, который будет заполнять lexem_id до тех пор, пока 
			// встречаются символы X V I
			while ( cur_sym == 'X' || cur_sym == 'V' || cur_sym == 'I')
			{
				if (size_num > 31) break;

				lexem_num[size_num] = cur_sym;
				size_num++;
				cur_sym = fgetc(file);
				number_symbol++;
			}

			// не знаю как по красивее это сделать
			if (size_num > 31) break;


			lexem_num[size_num] = '\0';

			cur_lexem.name = NUMBER;
			cur_lexem.value = lexem_num;

			// Если следующий символ после римских цифр стоит буква алфавита - то это identificator
			if (
				((
					(cur_sym >= 'A') && (cur_sym <= 'Z')
					)
					||
					(
						(cur_sym >= 'a') && (cur_sym <= 'z')
						)
					||
					(cur_sym == '_'))
				)
			{
				state = IDEN;
				flag_id_with_number = true;
				buff_id_with_number = lexem_num;
				break;
			}

			(*table_lexem).push_back(cur_lexem);

			state = START;
			break;

		// состояние приравнивание - возможно это знак := 
		case ASSIGN:
			cur_sym = fgetc(file);
			number_symbol++;

			if (cur_sym == '=')
			{
				cur_lexem.name = OPERATOR;
				cur_lexem.value = ":=";

				(*table_lexem).push_back(cur_lexem);

				cur_sym = fgetc(file);
				number_symbol++;
				
				state = START;
			}
			else 
				state = ERROR;
			
			break;
		// состояние разделителей - это один из символов (){};
		case SPR:
			cur_lexem.name = SEPARATOR;
				
			cur_lexem.value = cur_sym;

			(*table_lexem).push_back(cur_lexem);

			cur_sym = fgetc(file);
			number_symbol++;
			state = START;
		
			break;

		case CONDITIONS:
			if (cur_sym == '=' || cur_sym == '!')
			{
				cur_lexem.name = OPERATOR;
				cur_lexem.value = cur_sym; 

				cur_sym = fgetc(file);
				number_symbol++;

				if (cur_sym != '=')
				{
					state = ERROR;
					break;
				}

				(*table_lexem).push_back(cur_lexem);
			}
			else if ((cur_sym == '<') || (cur_sym == '>'))
			{
				cur_lexem.name = OPERATOR;

				cur_lexem.value = cur_sym;

				cur_sym = fgetc(file);
				number_symbol++;

				(*table_lexem).push_back(cur_lexem);

				state = START;
			}

			break;

		case ARIFM:
			cur_lexem.name = OPERATOR;

			cur_lexem.value = cur_sym;

			cur_sym = fgetc(file);
			number_symbol++;

			if (cur_sym == '*' || cur_sym == '/')
			{
				state = COM;
				break;
			}

			(*table_lexem).push_back(cur_lexem);

			state = START;

			break;
		case COM:

			// Большой комментарий
			if (cur_sym == '*')
			{
				cur_sym = fgetc(file);

				while (true)
				{
					while (cur_sym != '*')
					{
						if (cur_sym == '\n')
						{
							number_line++;
							number_symbol = 0;
						}

						cur_sym = fgetc(file);
						number_symbol++;
					}

					cur_sym = fgetc(file);
					number_symbol++;

					if (cur_sym == '\n')
					{
						number_line++;
						number_symbol = 0;
					}

					if (cur_sym == '/') break;

					cur_sym = fgetc(file);
					number_symbol++;
				}
			}
			// Маленький комментарий
			else
			{
				while (cur_sym != '\n' && !feof(file))
				{
					cur_sym = fgetc(file);
					number_symbol++;
				}

				number_line++;
				number_symbol = 0;
				
			}

			state = START;
			cur_sym = fgetc(file);
			number_symbol++;

			break;
		// состояние ошибки - не понятный символ, или какая-то ошибка
		case ERROR:
			std::cout << "Error in the line:"<< number_line << "   number symbol:" << number_symbol << std::endl;
			
			state = START;

			while ((cur_sym != ' ') && (cur_sym != '\t')&&(cur_sym != '\n'))
			{
				if (cur_sym == '\n')
				{
					number_line++;
					number_symbol = 0;
				}

				cur_sym = fgetc(file);
				number_symbol++;
			}

			break;
		default:
			break;
		}
	}

	if (file != NULL)
	{
		fclose(file);
	}

	return 0;
}




bool is_keyword(char* lexem)
{
	for (size_t i = 0; i < keywords->size(); i++)
	{
		if (keywords[i] == lexem) return true;
	}

	return false;
}

string get_string_type(lexem_type name)
{
	switch (name)
	{
	case KEYWORD:
		return "KEYWORD";
	case IDENTIFICATOR:
		return "IDENTIFICATOR";
	case NUMBER:
		return "NUMBER";
	case OPERATOR:
		return "OPERATOR";
	case SEPARATOR:
		return "SEPARATOR";
	default:
		return "default";
	}
}