// TextAnalyser.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <map>
#include <string>
#include <locale>
#include <list> 
#include <codecvt>
#include <ctime>

using namespace std;


int main()
{
	FILE *text, *specs, *out;																	// потоки для работы с файлами
	char str[1024];																				// Строка с текущей строкой из исходного файла
	char spec[750][30];																			// Массив со списком служебных слов
	int i = 0;																					
	char delim[21] = " .,;:!?\"'()/\\\n";														// Список разделителей
	char *context = NULL;
	bool isSpec = false;																		// Флаг для определения типа текущего символа
	map <string, int> dict;																		// Словарь уникальных слов
	list <pair<int, string>> dict_reverse;														// Отсортированный словарь
	int t;

	string Utf8_to_cp1251(const char *str);


	t = clock();
	
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);																

	setlocale(0, "");

	errno_t err = fopen_s(&text, "text.txt", "r");												// Открытие файла с текстом на чтение
	errno_t err2 = fopen_s(&specs, "specs.txt", "r");											// Открытие файла со списком служебных слов на чтение

	do
	{
		fgets(spec[i], 30, specs);																// Заполняем массив служебных слов из файла
		i++;
	} while (!feof(specs));

	if (specs) err2 = fclose(specs);															// Закрываем поток файла со служебными словами

	do
	{
		fgets(str, 1024, text);																	// По очереди берем следующую строчку из файла
		char *pch = strtok_s(str, delim, &context);												// Делим ее на слова

		while (pch != NULL)
		{
			string w = Utf8_to_cp1251(pch);														// Преобразуем текущее слово в ANSI
			const char *word = w.c_str();														// Записываем его в переменную char для работы функций strlen и strnicmp
			//cout << w << " ";
			for (int i = 0; i < 700; i++)
			{
				int len = (strlen(word) < 4) ? strlen(word) : strlen(word) - 2;					// Отбрасываем от слова две последние буквы, если слово длииное (> 4 букв) 
				if (_strnicmp(word, spec[i], len) == 0)
				{
					isSpec = true;																// Если есть совпадение с хотя бы одним служебным словом - пропускаем
					break;
				}
			}
			if (!isSpec)
			{
				auto result = dict.find(string(word));											// Ищем в словаре текущее слово
				if (result != dict.end()) dict.at(string(word))++;								// Если есть совпадение - увеличиваем счетчик слова на единицу
				else dict.insert(pair<string, int>(string(word), 1));							// В противном случае добавляем слвоо в словарь
			}
			else
			{
				isSpec = false;																	// Если слово - служебное, то сбрасываем флаг
			}
			pch = strtok_s(NULL, delim, &context);												
		}
	} while (!feof(text));

	for (auto it = dict.begin(); it != dict.end(); ++it)										
	{
		dict_reverse.push_back(pair<int, string>((*it).second, (*it).first));					// Переворачиваем словарь, раньше ключом являлось слово, теперь - его количество повторений
	}

	dict_reverse.sort();																		// Сортируем словарь по ключу по возрастанию
	dict_reverse.reverse();																		// И реверсируем

	auto it = dict_reverse.begin();

	for (int i = 0; i < 5; i++)
	{
		printf((*it).second.c_str());															// Выводим первые 5 слов на экран
		printf("\n");
		it++;
	}
	
	if (text) err = fclose(text);																// Закрываем поток

	cout << endl; 
	cout << "Execution time: " << (clock() - t) << " ms" << endl;
	cin.get();

    return 0;
}


string Utf8_to_cp1251(const char *str)															// Преобразовываем в ANSI
{
	string res;
	int result_u, result_c;
	result_u = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
	if (!result_u)
		return 0;
	wchar_t *ures = new wchar_t[result_u];
	if (!MultiByteToWideChar(CP_UTF8, 0, str, -1, ures, result_u))
	{
		delete[] ures;
		return 0;
	}
	result_c = WideCharToMultiByte(1251, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c)
	{
		delete[] ures;
		return 0;
	}
	char *cres = new char[result_c];
	if (!WideCharToMultiByte(1251, 0, ures, -1, cres, result_c, 0, 0))
	{
		delete[] ures;
		delete[] cres;
		return 0;
	}
	delete[] ures;
	res = string(cres);
	delete[] cres;
	return res;
}

