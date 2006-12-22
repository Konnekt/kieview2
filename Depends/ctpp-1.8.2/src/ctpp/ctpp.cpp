/*
 * Copyright (c) 2005, 2006 CTPP Dev Team
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the CTPP Team nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      ctpp.cpp
 *
 * $CTPP$
 */

// C++ includes

// Local Includes
#include <ctpp/ctpp.hpp>

namespace template_parser_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Структура function_param_data
//

//
// Конструктор
//
function_param_data::function_param_data(const std::string &sParam, const bool bIsVariable): param(sParam), is_variable(bIsVariable) { ;; }

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс loader_base
//

//
// Загрузка файла
//
void loader_base::load_file(const std::string &sFileName)
{
	char szBuffer[16385];
	sData.erase();

	FILE * fData = fopen(sFileName.c_str(), "r");
	if (fData == NULL) { throw std::logic_error("Cannot open file '" + sFileName + "' for reading."); }

	// Читаем из файла
	while(!feof(fData))
	{
		size_t iSize = fread(szBuffer, 1, 16384, fData);
		szBuffer[iSize] = '\0';
		sData += szBuffer;
	}
	fclose(fData);
}

//
// Получение данных
//
std::string & loader_base::get_data() { return sData; }

//
// Деструктор
//
loader_base::~loader_base() throw() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс t_template.
//

//
// Парзинг при инициализации
//
template_ret_type t_template::parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd) { throw std::logic_error("method parse must be defined"); }

//
// Выполнение пользовательской функции
//
std::string t_template::execute_udf_fn(stack_ref &sStackRef, param_data * pParamData, param_data * pRootParamData, bool bGlobalVars)
{
	// Получаем хэш
	t_param_hash * pvHash = pParamData -> hash();
	t_param_hash::iterator itpvHash;

	using std::string;
	using std::vector;

	// Функция принимает всего один параметр
	if (sStackRef.function -> accept_params() == udf_fn::ONE_PARAM)
	{
		// Попытка передать функции > 1 параметра
		int iParamCount = sStackRef.function_parameters.size();
		if (iParamCount > 1) { throw std::logic_error("Attempt to call function with more than 1 parameter."); }

		if (iParamCount == 1)
		{
			// Если параметр
			if (sStackRef.function_parameters[0].is_variable)
			{
				itpvHash = pvHash -> find(sStackRef.function_parameters[0].param);
				if (itpvHash != pvHash -> end()) { sStackRef.function -> param(*(itpvHash -> second -> val())); }
				else if (bGlobalVars)
				{
					itpvHash = pRootParamData -> hash() -> find(sStackRef.function_parameters[0].param);
					if (itpvHash != pRootParamData -> hash() -> end()) { sStackRef.function -> param(*(itpvHash -> second -> val())); }
					// Пустота
					else { sStackRef.function -> param(""); }
				}
				// Пустота
				else { sStackRef.function -> param(""); }
			}
			// Если константа
			else { sStackRef.function -> param(sStackRef.function_parameters[0].param); }
		}
		// Пустота
		else { sStackRef.function -> param(""); }
	}
	// Функция принимает два параметра
	else if (sStackRef.function -> accept_params() == udf_fn::TWO_PARAMS)
	{
		// Попытка передать функции > 2 параметров
		int iParamCount = sStackRef.function_parameters.size();
		if (iParamCount > 2) { throw std::logic_error("Attempt to call function with more than 2 parameters."); }

		string sFirstParam("");
		string sSecondParam("");
		// Задан 1 или 2 параметра
		if (iParamCount >= 1)
		{
			// Если параметр
			if (sStackRef.function_parameters[0].is_variable)
			{
				itpvHash = pvHash -> find(sStackRef.function_parameters[0].param);
				if (itpvHash != pvHash -> end()) { sFirstParam.assign(*(itpvHash -> second -> val())); }
				else if (bGlobalVars)
				{
					itpvHash = pRootParamData -> hash() -> find(sStackRef.function_parameters[0].param);
					if (itpvHash != pRootParamData -> hash() -> end()) { sFirstParam.assign(*(itpvHash -> second -> val())); }
				}
			}
			// Если константа
			else { sFirstParam.assign(sStackRef.function_parameters[0].param); }
		}
		// Задано 2 параметра
		if (iParamCount == 2)
		{
			// Если параметр
			if (sStackRef.function_parameters[1].is_variable)
			{
				itpvHash = pvHash -> find(sStackRef.function_parameters[1].param);
				if (itpvHash != pvHash -> end()) { sSecondParam.assign(*(itpvHash -> second -> val())); }
				else if (bGlobalVars)
				{
					itpvHash = pRootParamData -> hash() -> find(sStackRef.function_parameters[1].param);
					if (itpvHash != pRootParamData -> hash() -> end()) { sSecondParam.assign(*(itpvHash -> second -> val())); }
				}
			}
			// Если константа
			else { sSecondParam.assign(sStackRef.function_parameters[1].param); }
		}
		sStackRef.function -> param(sFirstParam, sSecondParam);
	}
	else if (sStackRef.function -> accept_params() == udf_fn::THREE_PARAMS)
	{
		// Попытка передать функции > 3 параметров
		int iParamCount = sStackRef.function_parameters.size();
		if (iParamCount > 3) { throw std::logic_error("Attempt to call function with more than 3 parameters."); }

		string sFirstParam("");
		string sSecondParam("");
		string sThirdParam("");
		// Задан 1 или 2 параметра
		if (iParamCount >= 1)
		{
			// Если параметр
			if (sStackRef.function_parameters[0].is_variable)
			{
				itpvHash = pvHash -> find(sStackRef.function_parameters[0].param);
				if (itpvHash != pvHash -> end()) { sFirstParam.assign(*(itpvHash -> second -> val())); }
				else if (bGlobalVars)
				{
					itpvHash = pRootParamData -> hash() -> find(sStackRef.function_parameters[0].param);
					if (itpvHash != pRootParamData -> hash() -> end()) { sFirstParam.assign(*(itpvHash -> second -> val())); }
				}
			}
			// Если константа
			else { sFirstParam.assign(sStackRef.function_parameters[0].param); }

		}
		// Задано 2 параметра
		if (iParamCount >= 2)
		{
			// Если параметр
			if (sStackRef.function_parameters[1].is_variable)
			{
				itpvHash = pvHash -> find(sStackRef.function_parameters[1].param);
				if (itpvHash != pvHash -> end()) { sSecondParam.assign(*(itpvHash -> second -> val())); }
				else if (bGlobalVars)
				{
					itpvHash = pRootParamData -> hash() -> find(sStackRef.function_parameters[1].param);
					if (itpvHash != pRootParamData -> hash() -> end()) { sSecondParam.assign(*(itpvHash -> second -> val())); }
				}
			}
			// Если константа
			else { sSecondParam.assign(sStackRef.function_parameters[1].param); }

		}
		// Задано 3 параметра
		if (iParamCount == 3)
		{
			// Если параметр
			if (sStackRef.function_parameters[2].is_variable)
			{
				itpvHash = pvHash -> find(sStackRef.function_parameters[2].param);
				if (itpvHash != pvHash -> end()) { sThirdParam.assign(*(itpvHash -> second -> val())); }
				else if (bGlobalVars)
				{
					itpvHash = pRootParamData -> hash() -> find(sStackRef.function_parameters[2].param);
					if (itpvHash != pRootParamData -> hash() -> end()) { sThirdParam.assign(*(itpvHash -> second -> val())); }
				}
			}
			// Если константа
			else { sThirdParam.assign(sStackRef.function_parameters[2].param); }
		}
		sStackRef.function -> param(sFirstParam, sSecondParam, sThirdParam);
	}
	// Произвольное количество параметров
	else if (sStackRef.function -> accept_params() == udf_fn::ANY_PARAMS)
	{
		std::vector<function_param_data>::iterator itvParamNames = sStackRef.function_parameters.begin();
		vector<string> vParams;
		while (itvParamNames != sStackRef.function_parameters.end())
		{
			string sParamData("");
			// Если параметр
			if (itvParamNames -> is_variable)
			{
				// Локальная переменная
				itpvHash = pvHash -> find(itvParamNames -> param);

				if (itpvHash != pvHash -> end()) { sParamData.assign(*(itpvHash -> second -> val())); }
				else if (bGlobalVars)
				{
					itpvHash = pRootParamData -> hash() -> find(itvParamNames -> param);
					if (itpvHash != pRootParamData -> hash() -> end()) { sParamData.assign(*(itpvHash -> second -> val())); }
				}
			}
			// Если константа
			else { sParamData.assign(itvParamNames -> param); }

			vParams.push_back(sParamData);
			++itvParamNames;
		}
		sStackRef.function -> param(vParams);
	}
	// Не может быть такого
	else { throw std::logic_error("Function can accept ONLY 1, 2, 3 or > 3 paramaters."); }

	// Выполняем функцию
	sStackRef.function -> handler();
	// Результат ненулевой ?

return sStackRef.function -> result();
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс template_text
//

//
// Конструктор
//
template_text::template_text(udf_fn_factory * pIFactory, const int iILine, const int iIPos, const int iITabLength, const bool bIDebug, const bool bIStrict, const bool bILoopContextVars, const bool bIGlobalVars):
	pFactory(pIFactory),
	iLine(iILine),
	iPos(iIPos),
	iTabLength(iITabLength),
	bDebug(bIDebug),
	bStrict(bIStrict),
	bLoopContextVars(bILoopContextVars),
	bGlobalVars(bIGlobalVars),
	bSelfCreatedFactory(false),
	bSelfCreatedLoader(false),
	pLoaderBase(NULL)
{
	// Если не задана Factory для стандартных функций, то создаем ее
	// и вставляем в нее все стандартные функции
	if (pFactory == NULL)
	{
		pFactory = new udf_fn_factory();
		std_fn_list oList;
		oList.set_all(pFactory);
		bSelfCreatedFactory = true;
	}
}

//
// Получение Factory для установки дополнительных функций.
//
udf_fn_factory * template_text::get_fn_factory() { return pFactory; }

//
// Установка include_dir
//
void template_text::set_include_dir(const v_include_dir & vIIncludeDir) { vIncludeDir = vIIncludeDir; }

//
// Установка загрузчика файлов
//
void template_text::set_file_opener(loader_base * pILoaderBase) { pLoaderBase = pILoaderBase; }

//
// Получение типа
//
e_token_type template_text::get_type() { return TMPL_TEXT; }

//
// Вставка данных
//
bool template_text::param(param_data * pParamData) { return param(pParamData, pParamData); }

//
// Вставка данных
//
bool template_text::param(param_data * pIParamData, param_data * pIRootParamData)
{
	using std::string;
	using std::vector;

	if (pIParamData && pIRootParamData)
	{
		pParamData = pIParamData;
		pRootParamData = pIRootParamData;
		// Получаем хэш
		t_param_hash * pvHash = pParamData -> hash();
		t_param_hash::iterator itpvHash;

		// Итерируем по стеку шаблонов
		v_stack::iterator itvStack = vStack.begin();
		while (itvStack != vStack.end())
		{
			// Если на стеке объект
			if (itvStack -> template_elem)
			{
				// Если на стеке объект типа TMPL_var, то проверяем на наличие переменной в локальной области видимости,
				// а если включены глобальные переменные - то в глобальной области.
				if (itvStack -> template_elem -> get_type() == TMPL_VAR)
				{
					// Сначала - локальные переменные
					itpvHash = pvHash -> find(itvStack -> elem_name);
					if (itpvHash != pvHash -> end()) { itvStack -> template_elem -> param(itpvHash -> second, pRootParamData); }
					// Затем - глобальные переменные
					else if (bGlobalVars)
					{
						itpvHash = pRootParamData -> hash() -> find(itvStack -> elem_name);
						// Имя найдено
						if (itpvHash != pRootParamData -> hash() -> end()) { itvStack -> template_elem -> param(itpvHash -> second, pRootParamData); }
					}
				}
				// Если на стеке объект типа TMPL_loop, то проверяем на наличие массива в локальной области видимости
				else if (itvStack -> template_elem -> get_type() == TMPL_LOOP)
				{
					itpvHash = pvHash -> find(itvStack -> elem_name);
					if (itpvHash != pvHash -> end()) { itvStack -> template_elem -> param(itpvHash -> second, pRootParamData); }
					else                             { itvStack -> template_elem -> param(NULL, NULL); }
				}
				// Для ветвлений мы должны выполнить функцию и проверить результат ее выполнения на истинность
				else if (itvStack -> template_elem -> get_type() == TMPL_IF || itvStack -> template_elem -> get_type() == TMPL_UNLESS)
				{
					bool bVarFound = false;
					// Если задана функция
					if (itvStack -> function)
					{
						if (execute_udf_fn(*itvStack, pParamData, pRootParamData, bGlobalVars).length()) { bVarFound = true; }
					}
					// Пользовательской функции нет, параметр есть.
					else
					{
						bool bSymbolFound = false;
						itpvHash = pvHash -> find(itvStack -> elem_name);
						// Ищем символ в локальной области имен
						if (itpvHash != pvHash -> end()) { bSymbolFound = true; }
						// Ничего не найдено; пытаемся искать в глобальной области имен
						else if ( bGlobalVars)
						{
							itpvHash = pRootParamData -> hash() -> find(itvStack -> elem_name);
							if (itpvHash != pRootParamData -> hash() -> end()) { bSymbolFound = true; }
						}

						// Символ найден; проверяем на истинность
						if (bSymbolFound)
						{
							// Для значений проверяем также наличие информации для вывода
							if (itpvHash -> second -> get_value_type() == param_data::VAR)
							{
								// Строка непустая
								if (itpvHash -> second -> val() -> length() != 0) { bVarFound = true; }
								// Строка пустая
								// else { bVarFound = false; }
							}
							else if (itpvHash -> second -> get_value_type() == param_data::ARRAY)
							{
								// Массив непустой
								if (itpvHash -> second -> array() -> size() != 0) { bVarFound = true; }
								// Массив пустой
								// else { bVarFound = false; }
							}
							else if (itpvHash -> second -> get_value_type() == param_data::HASH)
							{
								// Хеш непустой
								if (itpvHash -> second -> hash() -> size() != 0) { bVarFound = true; }
								// Хеш пустой
								// else { bVarFound = false; }
							}
							// Для всех других типов проверяем только наличие переменной на стеке
							else { bVarFound = true; }
						}
					}
					// TMPL_if
					if (itvStack -> template_elem -> get_type() == TMPL_IF)
					{
						static_cast<template_if *>(itvStack -> template_elem) -> additional_param(bVarFound);
					}
					// TMPL_unless
					else if (itvStack -> template_elem -> get_type() == TMPL_UNLESS)
					{
						static_cast<template_if *>(itvStack -> template_elem) -> additional_param(!bVarFound);
					}
					itvStack -> template_elem -> param(pParamData, pRootParamData);
				}
				// Для пользовательских функций просто передаем нужные параметры
				else if (itvStack -> template_elem -> get_type() == TMPL_UDF)     { itvStack -> template_elem -> param(pParamData, pRootParamData); }
				// Вложения, etc
				else if (itvStack -> template_elem -> get_type() == TMPL_INCLUDE) { itvStack -> template_elem -> param(pParamData, pRootParamData); }
				// Declare
				else if (itvStack -> template_elem -> get_type() == TMPL_DECLARE) { itvStack -> template_elem -> param(pParamData, pRootParamData); }
				// Break
				else if (itvStack -> template_elem -> get_type() == TMPL_BREAK)
				{
					if (itvStack -> template_elem -> param(pParamData, pRootParamData) == false) { return false; }
				}
				// Падаем, поскольку не может быть такого
				else { throw std::logic_error("Error in template_text::param NO SUCH TYPE: " + token2str(itvStack -> template_elem -> get_type())); }
			}
			++itvStack;
		}
	}
return true;
}

//
// Парзинг при инициализации
//
void template_text::parse(const std::string &sTemplate) { parse_block(sTemplate.begin(), sTemplate.end()); }

//
// Парзинг при инициализации
//
void template_text::parse(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd) { parse_block(itmData, itmDataEnd); }

//
// Вставка данных
//
template_ret_type template_text::parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd)
{
	using std::string;
	// Возвращаемое значение из шаблона
	template_ret_type sReturnType;
	// Максимальная длина токена
	unsigned int iPosition = 0;
	// Позиция отката, если предполагаемая последовательность не является управляющей
	string::const_iterator itmRollBackPos = itmData;
	// Итерируем либо до завершения шаблона,
	// либо до нахождения завершающей последовательности.
	// Если тип завершающей последовательности не совпадает с ожидаемым типом, падаем
	e_token_type eTokenType = TMPL_TEXT;
	string sTextSection;

	bool bInToken     = false;
	bool inCloseToken = false;
	bool bParseFlag   = true;
	bool bDoRollback  = false;
	bool bEndToken    = false;

	while (itmData != itmDataEnd && bParseFlag)
	{
		// Ищем начало последовательности токенов
		if (*itmData == '<')
		{
			iPosition = 1;
			// Если находимся внутри любого токена и найден символ '<',
			// то это означает, что предыдущая часть текста - обычный текст;
			// вставляем его в строку.
			if (bInToken || inCloseToken) { sTextSection.append(itmRollBackPos, (itmData - 1)); }
			// Мы не в токене; устанавливаем флаг начала токена
			else { bInToken = true; }
			// Позиция отката
			itmRollBackPos = itmData;
		}
		// Если находимся внутри открывающего токена, то ищем полные названия токенов
		// и названия переменных (для LOOP, VAR и IF).
		if (bInToken)
		{
			switch (iPosition)
			{
				// '<'
				case 1:
					++iPosition;
					break;
				// 'T' или 't'
				case 2:
					if (*itmData == 'T' || *itmData == 't') { iPosition++; }
					// Если это признак закрывающего тега, то выставляем флаг
					// о том, что это - закрывающий тег
					else if (*itmData == '/') { inCloseToken = true; bInToken = false; }
					// Иначе - мы не внутри токена, надо откатываться
					else { bInToken = false; bDoRollback = true; }
					break;
				// 'M' или 'm'
				case 3:
					if (*itmData == 'M' || *itmData == 'm') { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else { bInToken = false; bDoRollback = true; }
					break;
				// 'P' или 'p'
				case 4:
					if (*itmData == 'P' || *itmData == 'p') { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else { bInToken = false; bDoRollback = true; }
					break;
				// 'L' или 'L'
				case 5:
					if (*itmData == 'L' || *itmData == 'l') { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else { bInToken = false; bDoRollback = true; }
					break;
				// '_'
				case 6:
					if (*itmData == '_') { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else { bInToken = false; bDoRollback = true; }
					break;
				// Позиция == 7
				// Для VAR     - первая буква - 'V' или 'v'
				// Для LOOP    - первая буква - 'L' или 'l'
				// Для IF      - первая буква - 'I' или 'i'
				// Для ELSE    - первая буква - 'E' или 'e'
				// Для UNLESS  - первая буква - 'U' или 'u'
				// Для UDF     - первая буква - 'U' или 'u' (считаем, что это UNLESS и проверяем для следующей буквы)
				// Для DECLARE - первая буква - 'D' или 'd'
				// Для BREAK   - первая буква - 'B' или 'b'
				// Для COMMENT - первая буква - 'C' или 'c'
				case 7:
					if      (*itmData == 'V' || *itmData == 'v') { eTokenType = TMPL_VAR;     iPosition++; }
					else if (*itmData == 'L' || *itmData == 'l') { eTokenType = TMPL_LOOP;    iPosition++; }
					else if (*itmData == 'I' || *itmData == 'i') { eTokenType = TMPL_IF;      iPosition++; }
					else if (*itmData == 'E' || *itmData == 'e') { eTokenType = TMPL_ELSE;    iPosition++; }
					else if (*itmData == 'U' || *itmData == 'u') { eTokenType = TMPL_UNLESS;  iPosition++; }
					else if (*itmData == 'D' || *itmData == 'd') { eTokenType = TMPL_DECLARE; iPosition++; }
					else if (*itmData == 'B' || *itmData == 'b') { eTokenType = TMPL_BREAK;   iPosition++; }
					else if (*itmData == 'C' || *itmData == 'c') { eTokenType = TMPL_COMMENT; iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
					break;
				// Для VAR     - вторая буква - 'A' или 'a'
				// Для LOOP    - вторая буква - 'O' или 'o'
				// Для IF      - вторая буква - 'F' или 'F'
				// Для ELSE    - вторая буква - 'L' или 'l'
				// Для UNLESS  - вторая буква - 'N' или 'n'
				// Для UDF     - вторая буква - 'D' или 'd'
				// Для DECLARE - вторая буква - 'E' или 'e'
				// Для BREAK   - вторая буква - 'R' или 'r'
				// Для COMMENT - вторая буква - 'O' или 'o'
				case 8:
					if      ((*itmData == 'A' || *itmData == 'a') && eTokenType == TMPL_VAR)       { iPosition++; }
					else if ((*itmData == 'O' || *itmData == 'o') && (eTokenType == TMPL_LOOP ||
					                                                  eTokenType == TMPL_COMMENT)) { iPosition++; }
					else if ((*itmData == 'F' || *itmData == 'f') && eTokenType == TMPL_IF)        { iPosition++; }
					else if ((*itmData == 'L' || *itmData == 'l') && eTokenType == TMPL_ELSE)      { iPosition++; }
					else if ((*itmData == 'N' || *itmData == 'n') && eTokenType == TMPL_UNLESS)    { iPosition++; }
					else if ((*itmData == 'D' || *itmData == 'd') && eTokenType == TMPL_UNLESS)    { eTokenType = TMPL_UDF;     iPosition++; }
					else if ((*itmData == 'N' || *itmData == 'n') && eTokenType == TMPL_IF)        { eTokenType = TMPL_INCLUDE; iPosition++; }
					else if ((*itmData == 'E' || *itmData == 'e') && eTokenType == TMPL_DECLARE)   { iPosition++; }
					else if ((*itmData == 'R' || *itmData == 'r') && eTokenType == TMPL_BREAK)     { iPosition++; }
					else if ((*itmData == 'O' || *itmData == 'o') && eTokenType == TMPL_COMMENT)   { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
					break;
				// Для VAR     - третья буква - 'R' или 'r'
				// Для LOOP    - третья буква - 'O' или 'o'
				// Для IF      - третая буква - пробел
				// Для ELSE    - третая буква - 'S' или 's'
				// Для UNLESS  - третья буква - 'L' или 'l'
				// Для UDF     - третья буква - 'F' или 'f'
				// Для DECLARE - третья буква - 'C' или 'c'
				// Для BREAK   - третья буква - 'E' или 'e'
				// Для COMMENT - третья буква - 'M' или 'm'
				case 9:
					if      ((*itmData == 'R' || *itmData == 'r') && eTokenType == TMPL_VAR)       { iPosition++; }
					else if ((*itmData == 'O' || *itmData == 'o') && eTokenType == TMPL_LOOP)      { iPosition++; }
					else if ( *itmData == ' '                     && eTokenType == TMPL_IF)        { iPosition++; }
					else if ((*itmData == 'S' || *itmData == 's') && eTokenType == TMPL_ELSE)      { iPosition++; }
					else if ((*itmData == 'L' || *itmData == 'l') && eTokenType == TMPL_UNLESS)    { iPosition++; }
					else if ((*itmData == 'F' || *itmData == 'f') && eTokenType == TMPL_UDF)       { iPosition++; }
					else if ((*itmData == 'C' || *itmData == 'c') && (eTokenType == TMPL_INCLUDE
					                                               || eTokenType == TMPL_DECLARE)) { iPosition++; }
					else if ((*itmData == 'E' || *itmData == 'e') && eTokenType == TMPL_BREAK)     { iPosition++; }
					else if ((*itmData == 'M' || *itmData == 'm') && eTokenType == TMPL_COMMENT)   { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
			}
			// TMPL_VAR
			if (eTokenType == TMPL_VAR && iPosition > 9)
			{
				// В этой позиции должен стоять пробел
				if (iPosition == 11)
				{
					if (*itmData == ' ')  { iPosition++; }
					// Если пробела нет, значит - ошибка
					else                  { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition > 11)
				{
					// Вставляем текстовый блок ///////////////////////////////
					insert_text_block(sTextSection);
					// Парзинг строки параметров
					parse_param_string(iPosition, TMPL_VAR, itmData, itmDataEnd, itmRollBackPos);
					// Токен закончился
					bEndToken = true; bInToken = false; iPosition = 0;
				}
				else { iPosition++; }
			}
			// TMPL_IF
			else if (eTokenType == TMPL_IF && iPosition > 9)
			{
				if (iPosition > 10)
				{
					// Вставляем текстовый блок ///////////////////////////////
					insert_text_block(sTextSection);
					// Парзинг строки параметров
					parse_param_string(iPosition, TMPL_IF, itmData, itmDataEnd, itmRollBackPos);
					// Токен закончился;
					bEndToken = true; bInToken = false; iPosition = 0;
				}
				else { iPosition++; }
			}
			// TMPL_ELSE
			else if (eTokenType == TMPL_ELSE && iPosition > 9)
			{
				// Для ELSE - вторая буква - 'S' или 's'
				// 'S' или 's'
				if (iPosition == 10)
				{
					if (*itmData == 'S' || *itmData == 's') { iPosition++; }
					else                                    { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'E' или 'e'
				else if (iPosition == 11)
				{
					if (*itmData == 'E' || *itmData == 'e') { iPosition++; }
					else                                    { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// '>'
				else if (iPosition == 12)
				{
					// Выходим из цикла
					if (*itmData == '>') { bInToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
					else                 { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else { iPosition++; }
			}
			// TMPL_UNLESS
			else if (eTokenType == TMPL_UNLESS && iPosition > 9)
			{
				// 'L' или 'l'
				if (iPosition == 10)
				{
					if (*itmData == 'L' || *itmData == 'l')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'E' или 'e'
				else if (iPosition == 11)
				{
					if (*itmData == 'E' || *itmData == 'e')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'S' или 's'
				else if (iPosition == 12)
				{
					if (*itmData == 'S' || *itmData == 's')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'S' или 's'
				else if (iPosition == 13)
				{
					if (*itmData == 'S' || *itmData == 's')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition > 13)
				{
					// Вставляем текстовый блок ///////////////////////////////
					insert_text_block(sTextSection);
					// Парзинг строки параметров
					parse_param_string(iPosition, TMPL_UNLESS, itmData, itmDataEnd, itmRollBackPos);
					// Токен закончился
					bEndToken = true; bInToken = false; iPosition = 0;
				}
			}
			// Для LOOP
			else if (eTokenType == TMPL_LOOP && iPosition > 9)
			{
				// 'O' или 'o'
				if (iPosition == 10)
				{
					if (*itmData == 'O' || *itmData == 'o')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'P' или 'p'
				else if (iPosition == 11)
				{
					if (*itmData == 'P' || *itmData == 'p')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition > 11)
				{
					// Вставляем текстовый блок ///////////////////////////////
					insert_text_block(sTextSection);
					// Парзинг строки параметров
					parse_param_string(iPosition, TMPL_LOOP, itmData, itmDataEnd, itmRollBackPos);
					// Токен закончился
					bEndToken = true; bInToken = false; iPosition = 0;
				}
			}
			// Для UDF
			else if (eTokenType == TMPL_UDF && iPosition > 9)
			{
				// В этой позиции должен стоять пробел
				if (iPosition == 11)
				{
					if (*itmData == ' ')  { iPosition++; }
					// Если пробела нет, значит - ошибка
					else                  { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition > 11)
				{
					// Вставляем текстовый блок ///////////////////////////////
					insert_text_block(sTextSection);
					// Парзинг строки параметров
					parse_param_string(iPosition, TMPL_UDF, itmData, itmDataEnd, itmRollBackPos);
					// Токен закончился
					bEndToken = true; bInToken = false; iPosition = 0;
				}
				else { iPosition++; }
			}
			// Для INCLUDE
			else if (eTokenType == TMPL_INCLUDE && iPosition > 9)
			{
				// 'C' или 'c'
				if (iPosition == 10)
				{
					if (*itmData == 'C' || *itmData == 'c')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'L' или 'l'
				else if (iPosition == 11)
				{
					if (*itmData == 'L' || *itmData == 'l')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'U' или 'U'
				else if (iPosition == 12)
				{
					if (*itmData == 'U' || *itmData == 'u')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'D' или 'd'
				else if (iPosition == 13)
				{
					if (*itmData == 'D' || *itmData == 'd')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'D' или 'd'
				else if (iPosition == 14)
				{
					if (*itmData == 'E' || *itmData == 'e')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition > 14)
				{
					// Вставляем текстовый блок ///////////////////////////////
					insert_text_block(sTextSection);
					// Парзинг строки параметров
					parse_param_string(iPosition, TMPL_INCLUDE, itmData, itmDataEnd, itmRollBackPos);
					// Токен закончился
					bEndToken = true; bInToken = false; iPosition = 0;
				}
			}
			// Для DECLARE
			else if (eTokenType == TMPL_DECLARE && iPosition > 9)
			{
				// 'C' или 'c'
				if (iPosition == 10)
				{
					if (*itmData == 'C' || *itmData == 'c')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'L' или 'l'
				else if (iPosition == 11)
				{
					if (*itmData == 'L' || *itmData == 'l')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'A' или 'a'
				else if (iPosition == 12)
				{
					if (*itmData == 'A' || *itmData == 'a')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'R' или 'r'
				else if (iPosition == 13)
				{
					if (*itmData == 'R' || *itmData == 'r')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'E' или 'e'
				else if (iPosition == 14)
				{
					if (*itmData == 'E' || *itmData == 'e')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition > 14)
				{
					// Вставляем текстовый блок ///////////////////////////////
					insert_text_block(sTextSection);
					// Парзинг строки параметров
					parse_param_string(iPosition, TMPL_DECLARE, itmData, itmDataEnd, itmRollBackPos);
					// Парзинг
					// Токен закончился
					bEndToken = true; bInToken = false; iPosition = 0;
				}
			}
			// Для BREAK
			else if (eTokenType == TMPL_BREAK && iPosition > 9)
			{
				// 'E' или 'e'
				if (iPosition == 10)
				{
					if (*itmData == 'E' || *itmData == 'e')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'A' или 'a'
				else if (iPosition == 11)
				{
					if (*itmData == 'A' || *itmData == 'a')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'K' или 'k'
				else if (iPosition == 12)
				{
					if (*itmData == 'K' || *itmData == 'k')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition > 12)
				{
					// Вставляем текстовый блок ///////////////////////////////
					insert_text_block(sTextSection);
					// Парзинг строки параметров
					parse_param_string(iPosition, TMPL_BREAK, itmData, itmDataEnd, itmRollBackPos);
					// Парзинг
					// Токен закончился
					bEndToken = true; bInToken = false; iPosition = 0;
				}
			}
			// Для COMMENT
			else if (eTokenType == TMPL_BREAK && iPosition > 9)
			{
				// 'M' или 'm'
				if (iPosition == 10 || iPosition == 11)
				{
					if (*itmData == 'M' || *itmData == 'm')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'E' или 'e'
				else if (iPosition == 12)
				{
					if (*itmData == 'E' || *itmData == 'e')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'N' или 'n'
				else if (iPosition == 13)
				{
					if (*itmData == 'N' || *itmData == 'n')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'T' или 't'
				else if (iPosition == 14)
				{
					if (*itmData == 'T' || *itmData == 't')  { iPosition++; }
					else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition > 14)
				{
					// Вставляем текстовый блок ///////////////////////////////
					insert_text_block(sTextSection);
					// Парзинг строки параметров
					parse_param_string(iPosition, TMPL_COMMENT, itmData, itmDataEnd, itmRollBackPos);
					// Парзинг
					// Токен закончился
					bEndToken = true; bInToken = false; iPosition = 0;
				}
			}
		}

		// Внутри закрывающего токена
		if (inCloseToken)
		{
			switch (iPosition)
			{
				// '/'
				case 2:
					 iPosition++;
					 break;
				// 'T' или 't'
				case 3:
					if (*itmData == 'T' || *itmData == 't') { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else                                    { inCloseToken = false; bDoRollback = true; }
					break;
				// 'M' или 'm'
				case 4:
					if (*itmData == 'M' || *itmData == 'm') { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else                                    { inCloseToken = false; bDoRollback = true; }
					break;
				// 'P' или 'p'
				case 5:
					if (*itmData == 'P' || *itmData == 'p') { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else                                    { inCloseToken = false; bDoRollback = true; }
					break;
				// 'L' или 'L'
				case 6:
					if (*itmData == 'L' || *itmData == 'l') { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else                                    { inCloseToken = false; bDoRollback = true; }
					break;
				// '_'
				case 7:
					if (*itmData == '_') { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else                                    { inCloseToken = false; bDoRollback = true; }
					break;
				// Позиция == 8
				// Для LOOP    - первая буква - 'L' или 'l'
				// Для IF      - первая буква - 'I' или 'i'
				// Для UNLESS  - первая буква - 'U' или 'u'
				// Для COMMENT - первая буква - 'C' или 'C'
				case 8:
					if      (*itmData == 'L' || *itmData == 'l') { eTokenType = TMPL_LOOP;    iPosition++; }
					else if (*itmData == 'I' || *itmData == 'i') { eTokenType = TMPL_IF;      iPosition++; }
					else if (*itmData == 'U' || *itmData == 'u') { eTokenType = TMPL_UNLESS;  iPosition++; }
					else if (*itmData == 'C' || *itmData == 'c') { eTokenType = TMPL_COMMENT; iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
					break;
				// Позиция == 9
				// Для LOOP    - вторая буква - 'O' или 'o'
				// Для IF      - вторая буква - 'F' или 'f'
				// Для UNLESS  - вторая буква - 'N' или 'n'
				// Для COMMENT - вторая буква - 'O' или 'o'
				case 9:
					if      ((*itmData == 'O' || *itmData == 'o') && (eTokenType == TMPL_LOOP ||
					                                                  eTokenType == TMPL_COMMENT)) { iPosition++; }
					else if ((*itmData == 'F' || *itmData == 'f') && eTokenType == TMPL_IF)        { iPosition++; }
					else if ((*itmData == 'N' || *itmData == 'n') && eTokenType == TMPL_UNLESS)    { iPosition++; }
					// Иначе - мы не внутри токена, надо откатываться
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
					break;
			}

			// Ветвление
			if (eTokenType == TMPL_IF && iPosition > 9)
			{
				// '>'
				if (iPosition == 11)
				{
					// Выходим из цикла
					if (*itmData == '>') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
					else                 { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else { iPosition++; }
			}
			// Цикл
			else if (eTokenType == TMPL_LOOP && iPosition > 9)
			{
				// Для LOOP - вторая буква - 'O' или 'o'
				if (iPosition == 11)
				{
					if (*itmData == 'O' || *itmData == 'o') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'P' или 'p'
				else if (iPosition == 12)
				{
					if (*itmData == 'P' || *itmData == 'p') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// '>'
				else if (iPosition == 13)
				{
					// Выходим из цикла
					if (*itmData == '>') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else { iPosition++; }
			}
			// Обратное условие
			else if (eTokenType == TMPL_UNLESS && iPosition > 9)
			{
				// Для UNLESS - вторая буква - 'L' или 'l'
				if (iPosition == 11)
				{
					if (*itmData == 'L' || *itmData == 'l') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'E' или 'E'
				else if (iPosition == 12)
				{
					if (*itmData == 'E' || *itmData == 'e') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'S' или 's'
				else if (iPosition == 13)
				{
					if (*itmData == 'S' || *itmData == 's') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'S' или 's'
				else if (iPosition == 14)
				{
					if (*itmData == 'S' || *itmData == 's') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// '>'
				else if (iPosition == 15)
				{
					// Выходим из цикла
					if (*itmData == '>') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else { iPosition++; }
			}
			// Комментарий
			else if (eTokenType == TMPL_COMMENT && iPosition > 9)
			{
				// Для COMMENT - вторая буква - 'O' или 'o'
				if (iPosition == 11)
				{
					if (*itmData == 'O' || *itmData == 'o') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// 'M' или 'm'
				else if (iPosition == 12 || iPosition == 13)
				{
					if (*itmData == 'M' || *itmData == 'm') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition == 14)
				{
					if (*itmData == 'E' || *itmData == 'e') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition == 15)
				{
					if (*itmData == 'N' || *itmData == 'n') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else if (iPosition == 16)
				{
					if (*itmData == 'T' || *itmData == 't') { iPosition++; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				// '>'
				else if (iPosition == 17)
				{
					// Выходим из цикла
					if (*itmData == '>') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
					// Откат
					else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
				}
				else { iPosition++; }
			}
		}

		// Превышаена максимальная длина токена, откатываемся
		if(iPosition == C_TEMPLATE_MAX_TOKEN_LEN)
		{
			iPosition = 0; bInToken = false; inCloseToken = false; bDoRollback = true;
			// Если установлена жесткая проверка на синтаксис, то падаем
			do_rollback_token(itmData, itmRollBackPos);
		}

		// Если мы вышли из любого токена, но откат необходим - откатываемся
		if ((!bInToken || !inCloseToken) && bDoRollback)
		{
			sTextSection.append(itmRollBackPos, itmData);
			iPosition = 0;
			bDoRollback = false;
		}

		// Не внутри токена и токен только что не закончился
		if (!bInToken && !inCloseToken && !bEndToken) { sTextSection += *itmData; }
		// Токен закончился
		if (bEndToken) { bEndToken = false; }

		// Позиция в строке
		// Табуляция
		if      (*itmData == '\t') { iPos += iTabLength; }
		// Перевод строки
		else if (*itmData == '\n') { iPos = 1; iLine++; }
		// Любой другой символ
		else                       { iPos++; }
		itmData++;
	}

	// Вставляем последний текстовый блок ///////////////////////////////
	insert_text_block(sTextSection);
	// //////////////////////////////////////////////////////////////////
	// Вышли
	sReturnType.token_type = eTokenType;
	sReturnType.parse_pos  = --itmData;
	sReturnType.line       = iLine;
	sReturnType.column     = iPos;

return sReturnType;
}

//
// Вывод данных
//
std::string & template_text::output(bool & bBreak)
{
	sTextData.erase();
	// Итерируем по стеку шаблонов
	v_stack::iterator itvStack = vStack.begin();

	bBreak = false;
	while (itvStack != vStack.end())
	{
		// Если дана пустая ссылка, выдаем вместо этого текстовый блок
		if (itvStack -> template_elem == NULL) { sTextData += itvStack -> elem_name; }
		// Рекуррентно выводим данные
		else
		{
			sTextData += itvStack -> template_elem -> output(bBreak);
			if (bBreak) { break; }
		}
		++itvStack;
	}
return sTextData;
}

//
// Действия, необходимые для отката
//
void template_text::do_rollback_token(std::string::const_iterator itmData, std::string::const_iterator itmRollBackPos)
{
	// Если включен режим строгой проверки синтаксиса, то падаем.
	if (bStrict)
	{
		std::string sWrongToken(itmRollBackPos, itmData);
		throw std::logic_error("Syntax error in " + sWrongToken + " at line "+ d2str<int>(iLine) + " column " + d2str<int>(iPos) + ".");
	}
}

//
// Описание фатальной ошибки
//
void template_text::fatal_parsing_error(const e_token_type &eExpectedToken, const e_token_type &eFoundToken)
{
	throw std::logic_error("Fatal: Logic error near at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos) + ". Expected /" + token2str(eExpectedToken) + ", but found /" + token2str(eFoundToken) + ".");
}

//
// Описание фатальной ошибки
//
std::string template_text::token2str(const e_token_type &eToken)
{
	std::string sToken;
	switch (eToken)
	{
		case TMPL_VAR:
			sToken.assign("TMPL_VAR");
			break;
		case TMPL_IF:
			sToken.assign("TMPL_IF");
			break;
		case TMPL_ELSE:
			sToken.assign("TMPL_ELSE");
			break;
		case TMPL_UNLESS:
			sToken.assign("TMPL_UNLESS");
			break;
		case TMPL_LOOP:
			sToken.assign("TMPL_LOOP");
			break;
		case TMPL_UDF:
			sToken.assign("TMPL_UDF");
			break;
		case TMPL_INCLUDE:
			sToken.assign("TMPL_INCLUDE");
			break;
		case TMPL_DECLARE:
			sToken.assign("TMPL_DECLARE");
			break;
		case TMPL_BREAK:
			sToken.assign("TMPL_BREAK");
			break;
		default:
			sToken.assign("Ouch! This should NOT happened!");
			break;
	}
return sToken;
}

//
// Вставляем текстовый блок
//
void template_text::insert_text_block(std::string & sTextSection)
{
	stack_ref sStackRef;
	sStackRef.elem_name.assign(sTextSection);
	sStackRef.template_elem = NULL;
	sStackRef.function      = NULL;
	vStack.push_back(sStackRef);
	sTextSection.erase();
}

//
// Парзинг параметров функции
//
void template_text::parse_param_string(unsigned int &iPosition, const e_token_type &eFoundToken, std::string::const_iterator & itmData, std::string::const_iterator itmDataEnd, std::string::const_iterator itmRollBackPos)
{
	using std::string;

	stack_ref sStackRef;
	sStackRef.function = NULL;

	bool bIsEOP       = false;
	bool bInParam     = false;
	bool bIsVariable  = false;
	bool bIsQuoted    = false;
	bool bFunctionSet = false;

	bool bLeftBracketFound  = false;
	bool bRightBracketFound = false;

	char chQuote = '\0';

	string sParam;
	string sFunctionName;
	string::const_iterator itsParamBegin = itmData;
	string::const_iterator itsParamEnd   = itmData;

	while (itmData != itmDataEnd)
	{
		// Внутри параметра
		if (bInParam)
		{
			// Если это переменная, то признак ее конца - пробельный символ
			if (bIsVariable)
			{
				if (*itmData == ' ' || *itmData == '\t' || *itmData == '\r' || *itmData == '\n' || *itmData == ',')
				{
					bIsEOP = true;
					sParam.assign(itsParamBegin, itmData);
				}
				else if (*itmData == '>')
				{
					if (bLeftBracketFound && !bRightBracketFound) { throw std::logic_error("Unbalanced left bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }

					bIsEOP = true;
					sParam.assign(itsParamBegin, itmData);
				}
				else if (*itmData == '(')
				{
					if (!bLeftBracketFound) { bLeftBracketFound = true; bIsEOP = true; sParam.assign(itsParamBegin, itmData); }
					else { throw std::logic_error("Extra left bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
				}
				else if (*itmData == ')')
				{
					if (!bRightBracketFound && bLeftBracketFound) { bRightBracketFound = true; bIsEOP = true; sParam.assign(itsParamBegin, itmData); }
					else { throw std::logic_error("Unbalanced right bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
				}
				else if (*itmData == '\'' || *itmData == '"') { throw std::logic_error("Syntax error in parameter name at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
			}
			else
			{
				// Квотированный символ
				if (bIsQuoted)
				{
					itsParamEnd = itmData;
					itsParamEnd --;

					if (*itmData == chQuote) { sParam.append(itsParamBegin, itsParamEnd); itsParamBegin = itmData; }
					else
					{
						sParam.append(itsParamBegin, itsParamEnd);
						if      (*itmData == 'n') { sParam += '\n'; }
						else if (*itmData == 'r') { sParam += '\r'; }
						else if (*itmData == 't') { sParam += '\t'; }
						else if (*itmData == 'b') { sParam += '\b'; }
						else if (*itmData == 'a') { sParam += '\a'; }
						itsParamBegin = itmData;
						++itsParamBegin;
					}
				}
				else if ((*itmData == '\'' || *itmData == '"') && (*itmData == chQuote)) { bIsEOP = true; sParam.append(itsParamBegin, itmData); }
			}
		}
		else
		{
			// Начало константы
			if (*itmData == '\'' || *itmData == '"') { chQuote = *itmData; bInParam = true; bIsVariable = false; itsParamBegin = itmData; itsParamBegin++; }
			// Начало переменной
			else if (*itmData == '(')
			{
				if (!bLeftBracketFound) { bLeftBracketFound = true; }
				else { throw std::logic_error("Extra left bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
			}
			else if (*itmData == ')')
			{
				if (!bRightBracketFound && bLeftBracketFound) { bRightBracketFound = true; }
				else { throw std::logic_error("Unbalanced right bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
			}
			else if (*itmData != ' ' && *itmData != '\t' && *itmData != '\r' && *itmData != '\n' && *itmData != ',')
			{
				bInParam = true; bIsVariable = true; itsParamBegin = itmData;
			}
		}
		// Найден конец параметра
		if (bIsEOP)
		{
			if (bLeftBracketFound)
			{
				// Устанавливаем функцию
				if (!bFunctionSet)
				{
					// Имя функции
					sFunctionName.assign(sParam);
					sParam.erase();
					// Ищем имя функции в фабрике
					sStackRef.function = NULL;
					try { sStackRef.function = pFactory -> get(sFunctionName); }
					// Падаем с конкретизацией строки/столбца.
					catch(std::exception &e) { throw std::logic_error("Line " + d2str<int>(iLine) + " column " + d2str<int>(iPos) + ": " + e.what()); }
					bFunctionSet = true;
				}
				else { sStackRef.function_parameters.push_back(function_param_data(sParam, bIsVariable)); sParam.erase(); }
			}
			else { sStackRef.function_parameters.push_back(function_param_data(sParam, bIsVariable)); sParam.erase(); }

			bIsEOP = false; bInParam = false; bIsVariable = false;
		}
		// Признак квотированности символа
		if (*itmData == '\\')
		{
			// Квотирование разрешено ТОЛЬКО внутри переменных
			if (bIsVariable || !bInParam) { throw std::logic_error("Quoting allowed only in constants at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
			if (!bIsQuoted) { bIsQuoted = true; }
			else            { bIsQuoted = false; }
		}
		else { bIsQuoted = false; }

		// Превышаена максимальная длина токена, откатываемся
		if (iPosition == C_TEMPLATE_MAX_TOKEN_LEN)
		{
			iPosition = 0;
			// Если установлена жесткая проверка на синтаксис, то падаем
			do_rollback_token(itmData, itmRollBackPos);
		}

		// Позиция в строке
		// Табуляция
		if      (*itmData == '\t') { iPos += iTabLength; }
		// Перевод строки
		else if (*itmData == '\n') { iPos = 1; iLine++; }
		// Любой другой символ
		else                       { iPos++; }

		// Найден конец тега
		if ((bIsVariable || !bInParam)  && *itmData == '>')
		{
			if (bLeftBracketFound && !bRightBracketFound) { throw std::logic_error("Unbalanced left bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
			break;
		}

		// Выходим, если найден конец тега
		if (bRightBracketFound && *itmData != ' ' && *itmData != '\t' && *itmData != ')' && *itmData != '>') { throw std::logic_error("Syntax error at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }

		// Максимальный размер токена
		iPosition ++;
		itmData ++;
	}

	// Пустые параметры не катят
	if (sStackRef.function_parameters.size() == 0) { throw std::logic_error("Need at least one parameter at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }

	// Если функция не найдена, проверяем количество параметров
	if (!bFunctionSet)
	{
		if (sStackRef.function_parameters.size() != 1 && eFoundToken != TMPL_LOOP && eFoundToken != TMPL_DECLARE) { throw std::logic_error("Only one parameter are allowed at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
		sFunctionName.assign(sStackRef.function_parameters[0].param);
	}
	// Имя параметра
	sStackRef.elem_name.assign(sFunctionName);
	template_ret_type sReturnType;
	switch (eFoundToken)
	{
		case TMPL_VAR:
			if (sStackRef.function_parameters.size() != 1) { throw std::logic_error("Only one parameter are allowed for TMPL_var at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
			sStackRef.elem_name.assign(sStackRef.function_parameters[0].param);

			sStackRef.template_elem = new template_var(sStackRef.function);
			sStackRef.function      = NULL;
			sReturnType.line        = iLine;
			sReturnType.column      = iPos;
			sReturnType.parse_pos   = itmData;
			sReturnType.token_type  = TMPL_VAR;
			break;

		case TMPL_IF:
			sStackRef.template_elem = new template_if(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars, true, vIncludeDir, pLoaderBase);
			sReturnType = sStackRef.template_elem -> parse_block(++itmData, itmDataEnd);
			break;

		case TMPL_UNLESS:
			sStackRef.template_elem = new template_if(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars, false, vIncludeDir, pLoaderBase);
			sReturnType = sStackRef.template_elem -> parse_block(++itmData, itmDataEnd);
			break;

		case TMPL_LOOP:
			{
				if (bFunctionSet) { throw std::logic_error("Cannot use TMPL_loop with function at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
				bool bMLoopContextVars = bLoopContextVars;
				bool bMGlobalVars      = bGlobalVars;

				// Ищем модификатор __CONTEXT_VARS__
				std::vector<function_param_data>::iterator itvFunctionParamData = sStackRef.function_parameters.begin();
				if (bLoopContextVars)
				{
					while (itvFunctionParamData != sStackRef.function_parameters.end())
					{
						if (itvFunctionParamData -> param == "__NO_CONTEXT_VARS__") { bMLoopContextVars = false; break; }
						itvFunctionParamData++;
					}
				}
				else
				{
					while (itvFunctionParamData != sStackRef.function_parameters.end())
					{
						if (itvFunctionParamData -> param == "__CONTEXT_VARS__") { bMLoopContextVars = true; break; }
						itvFunctionParamData++;
					}
				}

				// Ищем модификатор __GLOBAL_VARS__
				itvFunctionParamData = sStackRef.function_parameters.begin();
				if (bGlobalVars)
				{
					while (itvFunctionParamData != sStackRef.function_parameters.end())
					{
						if (itvFunctionParamData -> param == "__NO_GLOBAL_VARS__") { bMGlobalVars = false; break; }
						itvFunctionParamData++;
					}
				}
				else
				{
					while (itvFunctionParamData != sStackRef.function_parameters.end())
					{
						if (itvFunctionParamData -> param == "__GLOBAL_VARS__") { bMGlobalVars = true; break; }
						itvFunctionParamData++;
					}
				}
				sStackRef.elem_name.assign(sStackRef.function_parameters[sStackRef.function_parameters.size() - 1].param);
				sStackRef.template_elem = new template_loop(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bMLoopContextVars, bMGlobalVars, vIncludeDir, pLoaderBase);
				sReturnType = sStackRef.template_elem -> parse_block(++itmData, itmDataEnd);
			}

			break;

		case TMPL_UDF:
			sStackRef.template_elem = new template_udf(sStackRef, bGlobalVars);
			sReturnType.line        = iLine;
			sReturnType.column      = iPos;
			sReturnType.parse_pos   = itmData;
			sReturnType.token_type  = TMPL_UDF;
			break;

		case TMPL_INCLUDE:
			{
				if (bFunctionSet) { throw std::logic_error("Cannot use TMPL_include with function at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
				string sIncludeFile;
				check_include_file(sStackRef.function_parameters[0].param, sIncludeFile);

				// Если нету объекта-загрузчика, создаем объект
				if (!pLoaderBase)
				{
					pLoaderBase = new loader_base();
					bSelfCreatedLoader = true;
				}
				// Загружаем шаблон
				pLoaderBase -> load_file(sIncludeFile);
				std::string &sTemplate = pLoaderBase -> get_data();

				sStackRef.template_elem = new template_include(pFactory, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars, vIncludeDir, pLoaderBase);
				sStackRef.template_elem -> parse_block(sTemplate.begin(), sTemplate.end());

				sReturnType.line        = iLine;
				sReturnType.column      = iPos;
				sReturnType.parse_pos   = itmData;
				sReturnType.token_type  = TMPL_INCLUDE;
			}
			break;

		case TMPL_DECLARE:
			if (bFunctionSet) { throw std::logic_error("Cannot use TMPL_declare with function at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
			sStackRef.template_elem = new template_declare(sStackRef);
			sReturnType.line        = iLine;
			sReturnType.column      = iPos;
			sReturnType.parse_pos   = itmData;
			sReturnType.token_type  = TMPL_DECLARE;
			break;

		case TMPL_BREAK:
			sStackRef.template_elem = new template_break(sStackRef, bGlobalVars);
			sReturnType.line        = iLine;
			sReturnType.column      = iPos;
			sReturnType.parse_pos   = itmData;
			sReturnType.token_type  = TMPL_BREAK;
			break;
		default:
			throw std::logic_error("Ouch! This should not happened at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos));
	}

	iLine = sReturnType.line;
	iPos  = sReturnType.column;

	// Если тип возвращенного значения не совпадает с запрашиваемым, падаем
	if (eFoundToken != sReturnType.token_type)
	{
		// Удаляем ошибочный шаблон
		delete sStackRef.template_elem;
		fatal_parsing_error(eFoundToken, sReturnType.token_type);
	}

	vStack.push_back(sStackRef);
	itmData = sReturnType.parse_pos;
}

//
// Проверка пути файла
//
void template_text::check_include_file(const std::string & sParam, std::string &sIncludeFile)
{
	bool bIncludeDirFound = false;
	// Проверяем путь на предмет вхождения в INCLUDE_PATH
	v_include_dir::iterator itvIncludeDir = vIncludeDir.begin();
	while(itvIncludeDir != vIncludeDir.end())
	{
		if (itvIncludeDir -> length())
		{
			sIncludeFile.assign(sParam);
			if (sParam[0] != '/')
			{
				if ((*itvIncludeDir)[itvIncludeDir -> length() - 1] != '/') { sIncludeFile = '/' + sIncludeFile; }
				sIncludeFile = *itvIncludeDir + sIncludeFile;
			}

			// Вырезаем все последовательности "../"
			std::vector<std::string> vPath;
			bool bIsEscape = false;
			std::string::const_iterator itsIncludeFile = sIncludeFile.begin();
			std::string sTmpValue;
			while (itsIncludeFile != sIncludeFile.end())
			{
				sTmpValue += *itsIncludeFile;
				if (*itsIncludeFile == '/' && !bIsEscape)
				{
					if (sTmpValue.length() && !(vPath.size() != 0 && (sTmpValue == "/" || sTmpValue == "./"))) { vPath.push_back(sTmpValue); }
					sTmpValue.erase();
				}

				if (bIsEscape)               { bIsEscape  = false; }
				if (*itsIncludeFile == '\\') { bIsEscape = true; }
				itsIncludeFile++;
			}
			if (sTmpValue.length()) { vPath.push_back(sTmpValue); }

			std::vector<std::string> vNewPath;
			std::vector<std::string>::iterator itvPath = vPath.begin();
			while(itvPath != vPath.end())
			{
				if (*itvPath == "../")
				{
					if (vNewPath.size()) { vNewPath.pop_back(); }
					else { throw std::logic_error("Invalid Path for include: '" + sIncludeFile + "' at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos) + "."); }
				}
				else { vNewPath.push_back(*itvPath); }
				itvPath++;
			}
			sTmpValue.erase();
			itvPath = vNewPath.begin();
			while(itvPath != vNewPath.end())
			{
				sTmpValue += *itvPath;
				itvPath++;
			}
			if (sTmpValue.length() && sTmpValue.find(*itvIncludeDir) == 0) { bIncludeDirFound = true; sIncludeFile.assign(sTmpValue); break; }
			itvIncludeDir++;
		}
	}
	// Если нашли, то открываем файл
	if (!bIncludeDirFound) { throw std::logic_error("File: '" + sIncludeFile + "' not in IncludePath at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos) + "."); }
}

//
// Очистка шаблона
//
void template_text::clear_template()
{
	v_stack::iterator itvStack = vStack.begin();
	while (itvStack != vStack.end()) { delete itvStack -> template_elem; itvStack++; }
	v_stack vTMPStack; vStack.swap(vTMPStack);
}

//
// Деструктор
//
template_text::~template_text() throw()
{
	try
	{
		clear_template();

		// Удаляем factory, если она была создана внутри класса
		if (bSelfCreatedFactory)
		{
			pFactory -> clear();
			delete pFactory;
		}
		if (bSelfCreatedLoader) { delete pLoaderBase; }
	}
	catch(...) { ;; }
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс template_var
//

//
// Конструктор
//
template_var::template_var(udf_fn * pIFunction): pFunction(pIFunction)
{
	;;
}

//
// Получение типа
//
e_token_type template_var::get_type() { return TMPL_VAR; }

//
// Вывод данных
//
std::string & template_var::output(bool & bBreak)
{
	if (pFunction != NULL && sValue.length())
	{
		pFunction -> param(sValue);
		pFunction -> handler();
		sTmpValue.assign(pFunction -> result());
	}
	else { sTmpValue.assign(sValue); }
	sValue.erase();
return sTmpValue;
}

//
// Вставка данных
//
bool template_var::param(param_data * pParamData, param_data * pIRootParamData)
{
	if (pParamData != NULL) { sValue.assign(*(pParamData -> val())); }
	else { sValue.assign(""); }
return true;
}

//
// Деструктор
//
template_var::~template_var() throw() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс template_if
//

//
// Конструктор
//
template_if::template_if(udf_fn_factory * pIFactory, const int iILine, const int iIPos, const int iITabLength, const bool bIDebug, const bool bIStrict, const bool bILoopContextVars, const bool bIGlobalVars, const bool bIIsIf, const v_include_dir &vIIncludeDir, loader_base * pILoaderBase):
	pParamData(NULL),
	pRootParamData(NULL),
	pFactory(pIFactory),
	iLine(iILine),
	iPos(iIPos),
	iTabLength(iITabLength),
	bDebug(bIDebug),
	bStrict(bIStrict),
	bLoopContextVars(bILoopContextVars),
	bGlobalVars(bIGlobalVars),
	bIsIf(bIIsIf),
	vIncludeDir(vIIncludeDir),
	pLoaderBase(pILoaderBase)
{
	;;
}


//
// Получение типа
//
e_token_type template_if::get_type() { return bIsIf ? TMPL_IF : TMPL_UNLESS; }

//
// Вывод данных
//
std::string & template_if::output(bool & bBreak)
{
	sTextData.erase();
	if (pParamData && pRootParamData)
	{
		// Прямая ветка
		if (bFlag)                          { pTemplateIfText -> param(pParamData, pRootParamData);   sTextData += pTemplateIfText -> output(bBreak);   }
		// Обратная ветка
		else if (pTemplateElseText != NULL) { pTemplateElseText -> param(pParamData, pRootParamData); sTextData += pTemplateElseText -> output(bBreak); }
	}

// Возвращаем результат парзинга данных
return sTextData;
}

//
// Передача дополнительного пареметра
//
void template_if::additional_param(const bool & bIFlag) { bFlag = bIFlag; }

//
// Вставка данных (только аккумуляция ссылок)
//
bool template_if::param(param_data * pIParamData, param_data * pIRootParamData)
{
	pParamData     = pIParamData;
	pRootParamData = pIRootParamData;
return true;
}

//
// Парзинг при инициализации
//
template_ret_type template_if::parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd)
{
	// Парзинг при инициализации
	pTemplateIfText = new template_text(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars);
	static_cast<template_text *>(pTemplateIfText) -> set_include_dir(vIncludeDir);

	// Парзим шаблон
	template_ret_type sRetType = pTemplateIfText -> parse_block(itmData, itmDataEnd);

	// Есть обратная ветка?
	if (sRetType.token_type == TMPL_ELSE)
	{
		sRetType.parse_pos++;
		// Парзинг при инициализации
		pTemplateElseText = new template_text(pFactory, sRetType.line, sRetType.column, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars);
		static_cast<template_text *>(pTemplateElseText) -> set_include_dir(vIncludeDir);
		// Парзим шаблон
		sRetType = pTemplateElseText -> parse_block(sRetType.parse_pos, itmDataEnd);
	}
	else { pTemplateElseText = NULL; }

return sRetType;
}

//
// Деструктор
//
template_if::~template_if() throw()
{
	// Удаляем объект
	delete pTemplateIfText;
	if (pTemplateElseText) { delete pTemplateElseText; }
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс template_loop
//

//
// Конструктор
//
template_loop::template_loop(udf_fn_factory * pIFactory, const int iILine, const int iIPos, const int iITabLength, const bool bIDebug, const bool bIStrict, const bool bILoopContextVars, const bool bIGlobalVars, const v_include_dir &vIIncludeDir, loader_base * pILoaderBase):
	pTemplateText(NULL),
	pParamData(NULL),
	pRootParamData(NULL),
	pFactory(pIFactory),
	iLine(iILine),
	iPos(iIPos),
	iTabLength(iITabLength),
	bDebug(bIDebug),
	bStrict(bIStrict),
	bLoopContextVars(bILoopContextVars),
	bGlobalVars(bIGlobalVars),
	vIncludeDir(vIIncludeDir),
	pLoaderBase(pILoaderBase)
{
	;;
}

//
// Получение типа
//
e_token_type template_loop::get_type() { return TMPL_LOOP; }

//
// Вывод данных
//
std::string & template_loop::output(bool & bBreak)
{
	sTextData.erase();
	// Проходим в цикле по всем элементам переданного массива
	if (pParamData && pRootParamData)
	{
		t_param_array * pvArray = pParamData -> array();
		t_param_array::iterator itpvArray = pvArray -> begin();
		t_param_hash * pLoopParamHash;

		// Счетчик элементов
		int iCount = 1;
		t_param_array::iterator itpvLastElement = pvArray -> end();
		// Последний элемент массива
		Hasher oHasher;
		UINT_32  i__FIRST__   = oHasher("__FIRST__");
		UINT_32  i__INNER__   = oHasher("__INNER__");
		UINT_32  i__LAST__    = oHasher("__LAST__");
		UINT_32  i__ODD__     = oHasher("__ODD__");
		UINT_32  i__EVEN__    = oHasher("__EVEN__");
		UINT_32  i__SIZE__    = oHasher("__SIZE__");
		UINT_32  i__COUNTER__ = oHasher("__COUNTER__");

		param_data * pVarFirstLastInner = new param_data(param_data::VAR);
		param_data * pVarEvenOdd        = new param_data(param_data::VAR);
		param_data * pVarSize           = new param_data(param_data::VAR);

		pVarSize -> val() -> assign(d2str<int>(pvArray -> size()));
		param_data * pVarCounter        = new param_data(param_data::VAR);

		if (itpvLastElement != pvArray -> begin()) { itpvLastElement--; }
		while(itpvArray != pvArray -> end())
		{
			// Если используются контекстные переменные
			if (bLoopContextVars)
			{
				using CTPP::Pair;
				using std::string;
				pLoopParamHash = (*itpvArray) -> hash();
				// Вставляем переменные цикла
				// __FIRST__
//				param_data * pVarFirstLastInner;
				if (iCount == 1)
				{
					pVarFirstLastInner -> val() -> assign(d2str<int>(iCount));
					pLoopParamHash -> insert(Pair<string, param_data *>("__FIRST__", pVarFirstLastInner), i__FIRST__);
				}
				// __INNER__
				if (iCount != 1 && itpvArray != itpvLastElement)
				{
					pVarFirstLastInner -> val() -> assign(d2str<int>(iCount));
					pLoopParamHash -> insert(Pair<string, param_data *>("__INNER__", pVarFirstLastInner), i__INNER__);
				}
				// __LAST__
				else if (itpvArray == itpvLastElement)
				{
					pVarFirstLastInner -> val() -> assign(d2str<int>(iCount));
					pLoopParamHash -> insert(Pair<string, param_data *>("__LAST__", pVarFirstLastInner), i__LAST__);
				}

				// __ODD__
				if (iCount % 2 == 1)
				{
					pVarEvenOdd -> val() -> assign(d2str<int>(iCount));
					pLoopParamHash -> insert(Pair<string, param_data *>("__ODD__", pVarEvenOdd), i__ODD__);
				}
				// __EVEN__
				else
				{
					pVarEvenOdd -> val() -> assign(d2str<int>(iCount));
					pLoopParamHash -> insert(Pair<string, param_data *>("__EVEN__", pVarEvenOdd), i__EVEN__);
				}

				// __SIZE__
				pLoopParamHash -> insert(Pair<string, param_data *>("__SIZE__", pVarSize), i__SIZE__);

				// __COUNTER__
				pVarCounter -> val() -> assign(d2str<int>(iCount));
				pLoopParamHash -> insert(Pair<string, param_data *>("__COUNTER__", pVarCounter), i__COUNTER__);

				// Вставляем данные
				pTemplateText -> param(*itpvArray, pRootParamData);

				sTextData += pTemplateText -> output(bBreak);

				// __COUNTER__
				pLoopParamHash -> erase("__COUNTER__", i__COUNTER__);
				// __SIZE__
				pLoopParamHash -> erase("__SIZE__", i__SIZE__);

				// __FIRST__
				if (iCount == 1)                                 { pLoopParamHash -> erase("__FIRST__", i__FIRST__); }
				// __INNER__
				if (iCount != 1 && itpvArray != itpvLastElement) { pLoopParamHash -> erase("__INNER__", i__INNER__); }
				// __LAST__
				else if (itpvArray == itpvLastElement)           { pLoopParamHash -> erase("__LAST__", i__LAST__); }

				// __ODD__ / __EVEN__
				if (iCount % 2 == 1) { pLoopParamHash -> erase("__ODD__", i__ODD__); }
				else                 { pLoopParamHash -> erase("__EVEN__", i__EVEN__); }
			}
			// Контекстные переменные не используются
			else
			{
				// Вставляем данные
				pTemplateText -> param(*itpvArray, pRootParamData);
				sTextData += pTemplateText -> output(bBreak);
			}

			itpvArray++;
			iCount++;
		}
		delete pVarCounter;
		delete pVarSize;
		delete pVarFirstLastInner;
		delete pVarEvenOdd;
	}
// Возвращаем результат парзинга данных
return sTextData;
}

//
// Вставка данных
//
bool template_loop::param(param_data * pIParamData, param_data * pIRootParamData)
{
	pRootParamData = pIRootParamData;
	pParamData = pIParamData;
return true;
}

//
// Парзинг при инициализации
//
template_ret_type template_loop::parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd)
{
	// Парзинг при инициализации
	pTemplateText = new template_text(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars);
	static_cast<template_text *>(pTemplateText) -> set_include_dir(vIncludeDir);

	// Парзим шаблон
	template_ret_type sRetType = pTemplateText -> parse_block(itmData, itmDataEnd);

return sRetType;
}

//
// Деструктор
//
template_loop::~template_loop() throw() { delete pTemplateText; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс template_udf
//

//
// Конструктор
//
template_udf::template_udf(const stack_ref &sIStackRef, bool bIGlobalVars): sStackRef(sIStackRef), bGlobalVars(bIGlobalVars)
{
	;;
}

//
// Получение типа
//
e_token_type template_udf::get_type() { return TMPL_UDF; }

//
// Вывод данных
//
std::string & template_udf::output(bool & bBreak)
{
	if (pParamData && pRootParamData) { sValue = execute_udf_fn(sStackRef, pParamData, pRootParamData, bGlobalVars); }
return sValue;
}

//
// Вставка данных
//
bool template_udf::param(param_data * pIParamData, param_data * pIRootParamData)
{
	pParamData     = pIParamData;
	pRootParamData = pIRootParamData;
return true;
}

//
// Деструктор
//
template_udf::~template_udf() throw() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс template_udf
//

//
// Конструктор
//
template_include::template_include(udf_fn_factory * pIFactory, const int iITabLength, const bool bIDebug, const bool bIStrict, const bool bILoopContextVars, const bool bIGlobalVars, const v_include_dir &vIIncludeDir, loader_base * pILoaderBase)
{
	pTemplateText = new template_text(pIFactory, 1, 1, iITabLength, bIDebug, bIStrict, bILoopContextVars, bIGlobalVars);
	static_cast<template_text *>(pTemplateText) -> set_include_dir(vIIncludeDir);
	static_cast<template_text *>(pTemplateText) -> set_file_opener(pILoaderBase);
}

//
// Деструктор
//
template_include::~template_include() throw()
{
	try { delete pTemplateText; }
	catch(...) { ;; }
}

//
// Вывод данных
//
std::string & template_include::output(bool & bBreak) { return pTemplateText -> output(bBreak); }

//
// Получение типа
//
e_token_type template_include::get_type() { return TMPL_INCLUDE; }

//
// Парзинг при инициализации
//
template_ret_type template_include::parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd) { return pTemplateText -> parse_block(itmData, itmDataEnd); }

//
// Вставка данных
//
bool template_include::param(param_data * pIParamData, param_data * pIRootParamData)
{
	if (pIParamData && pIRootParamData) { pTemplateText -> param(pIParamData, pIRootParamData); }
return true;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Template_break
//

//
// Конструктор
//
template_break::template_break(const stack_ref &sIStackRef, bool bIGlobalVars): sStackRef(sIStackRef), bGlobalVars(bIGlobalVars)
{
	;;
}

//
// Деструктор
//
template_break::~template_break() throw() { ;; }

//
// Получение типа
//
e_token_type template_break::get_type() { return TMPL_BREAK; }

//
// Вставка данных
//
bool template_break::param(param_data * pParamData, param_data * pIRootParamData)
{
	bBreakFlag = false;
	if (pParamData && pIRootParamData)
	{
		std::string sValue = execute_udf_fn(sStackRef, pParamData, pIRootParamData, bGlobalVars);
		if (sValue.length())
		{
			bBreakFlag = true;
			return false;
		}
	}
return true;
}

//
// Вывод данных
//
std::string & template_break::output(bool & bBreak)
{
	bBreak = bBreakFlag;
	return sTMP;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Объявление переменной
//

//
// Конструктор
//
template_declare::template_declare(const stack_ref &sIStackRef)
{
	if (sIStackRef.function_parameters.size() != 2) { throw std::logic_error("TMPL_declare accept ONLY 2 parameters!"); }

	oParamName  = sIStackRef.function_parameters[0];
	oParamValue = sIStackRef.function_parameters[1];
}

	// Деструктор
template_declare::~template_declare() throw() { ;; }

// Получение типа
e_token_type template_declare::get_type() { return TMPL_DECLARE; }

// Вставка данных
bool template_declare::param(param_data * pParamData, param_data * pIRootParamData)
{
	param_data::e_value_type eType = pParamData -> get_value_type();
	if (eType == param_data::VAR || eType == param_data::ARRAY) { throw std::logic_error("TMPL_declare: given data is *NOT* hash!"); }

	std::string sValue;
	if (oParamValue.is_variable)
	{
		t_param_hash::iterator it = pParamData -> hash() -> find(oParamValue.param);

		if (it != pParamData -> hash() -> end()) { sValue.assign(*(it -> second -> val())); }
	}
	else { sValue.assign(oParamValue.param); }

	std::string sKeyName;
	if (oParamName.is_variable)
	{
		t_param_hash::iterator it = pParamData -> hash() -> find(oParamName.param);
		if (it == pParamData -> hash() -> end()) { return true; }

		sKeyName.assign(*(it -> second -> val()));
	}
	else  { sKeyName.assign(oParamName.param); }

	t_param_hash::iterator it = pParamData -> hash() -> find(sKeyName);
	if (it == pParamData -> hash() -> end())
	{

		param_data * oValue = new param_data();
		oValue -> val() -> assign(sValue);
		pParamData -> hash() -> insert(CTPP::Pair<std::string, param_data *>(oParamName.param, oValue));
	}
	else
	{
		it -> second -> val() -> assign(sValue);
	}
return true;
}

std::string & template_declare::output(bool & bBreak)
{
return sTMP;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace template_parser_ns
// End.
