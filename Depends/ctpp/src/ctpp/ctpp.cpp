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
#include <functions/std_fn_list.hpp>

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
  template_ret_type sReturnType;
  unsigned int iPosition = 0;
  std::string::const_iterator itmRollBackPos = itmData;
  e_token_type eTokenType = TMPL_TEXT;
  std::string sTextSection;

  bool bInToken     = false;
  bool inCloseToken = false;
  bool bParseFlag   = true;
  bool bDoRollback  = false;
  bool bEndToken    = false;
  bool bPrevBracket = false;

  while (itmData != itmDataEnd && bParseFlag)
  {
    if (*itmData == '{' && *(itmData + 1) != ' ')
    {
      bPrevBracket = true;
      iPosition = 1;
      if (bInToken || inCloseToken)
      {
        if (bPrevBracket) { sTextSection.append(itmRollBackPos, itmData);       }
        else              { sTextSection.append(itmRollBackPos, (itmData - 1)); }
      }
      else { bInToken = true; }
      itmRollBackPos = itmData;
    }
    else { bPrevBracket = false; }
    if (bInToken)
    {
      switch (iPosition)
      {
        case 1:
          if (*itmData != '{') {
            bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos);
          }
          iPosition++;
          break;
        case 2:
          if      (*itmData == '$') { eTokenType = TMPL_VAR;     iPosition++; }
          else if (*itmData == 'i') { eTokenType = TMPL_IF;      iPosition++; }
          else if (*itmData == 'e') { eTokenType = TMPL_ELSE;    iPosition++; }
          else if (*itmData == 'u') { eTokenType = TMPL_UNLESS;  iPosition++; }
          else if (*itmData == 'f') { eTokenType = TMPL_UDF;     iPosition++; }
          else if (*itmData == '/') { inCloseToken = true; bInToken = false; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
        case 3:
          if      (eTokenType == TMPL_VAR)      { iPosition++; }
          else if (*itmData == 'f' && eTokenType == TMPL_IF)        { iPosition++; }
          else if (*itmData == 'l' && eTokenType == TMPL_ELSE)      { iPosition++; }
          else if (*itmData == 'n' && eTokenType == TMPL_UNLESS)    { iPosition++; }
          else if (*itmData == 'u' && eTokenType == TMPL_UDF)       { iPosition++; }
          else if (*itmData == 'n' && eTokenType == TMPL_IF)        { eTokenType = TMPL_INCLUDE; iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
      }
      // TMPL_VAR
      if (eTokenType == TMPL_VAR && iPosition > 3)
      {
        insert_text_block(sTextSection);
        parse_param_string(iPosition, TMPL_VAR, itmData, itmDataEnd, itmRollBackPos);
        bEndToken = true; bInToken = false; iPosition = 0;
      }
      // TMPL_IF
      else if (eTokenType == TMPL_IF && iPosition > 3)
      {
        if (iPosition == 4)
        {
          if (*itmData == 'f') { iPosition++; }
          else                 { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 4)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_IF, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_ELSE
      else if (eTokenType == TMPL_ELSE && iPosition > 3)
      {
        if (iPosition == 4)
        {
          if (*itmData == 'l') { iPosition++; }
          else                 { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 5)
        {
          if (*itmData == 's') { iPosition++; }
          else                 { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 5)
        {
          if (*itmData == 'e') { iPosition++; }
          else                 { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == '}') { bInToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else                 { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
      // TMPL_UNLESS
      else if (eTokenType == TMPL_UNLESS && iPosition > 3)
      {
        if (iPosition == 4)
        {
          if (*itmData == 'n')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 5)
        {
          if (*itmData == 'l')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'e')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7 || iPosition == 8)
        {
          if (*itmData == 's')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 8)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_UNLESS, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_UDF
      else if (eTokenType == TMPL_UDF && iPosition > 3)
      {
        if (iPosition == 4)
        {
          if (*itmData == 'u')  { iPosition++; }
          else                  { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 5)
        {
          if (*itmData == 'n')  { iPosition++; }
          else                  { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'c')  { iPosition++; }
          else                  { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 6)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_UDF, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
        else { iPosition++; }
      }
      // TMPL_INCLUDE
      else if (eTokenType == TMPL_INCLUDE && iPosition > 3)
      {
        if (iPosition == 4)
        {
          if (*itmData == 'n')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 5)
        {
          if (*itmData == 'c')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'l')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'u')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == 'd')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9)
        {
          if (*itmData == 'e')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 9)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_INCLUDE, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
    }

    if (inCloseToken)
    {
      switch (iPosition)
      {
        case 1:
        case 2:
           iPosition++;
           break;
        case 3:
          if      (*itmData == 'i') { eTokenType = TMPL_IF;      iPosition++; }
          else if (*itmData == 'u') { eTokenType = TMPL_UNLESS;  iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
        case 4:
          if      (*itmData == 'f' && eTokenType == TMPL_IF)        { iPosition++; }
          else if (*itmData == 'n' && eTokenType == TMPL_UNLESS)    { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
      }

      if (eTokenType == TMPL_IF && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'f') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == '}') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
      else if (eTokenType == TMPL_UNLESS && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'n') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'l') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'e') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8 || iPosition == 9)
        {
          if (*itmData == 's') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 10)
        {
          if (*itmData == '}') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
    }

		if(iPosition == C_TEMPLATE_MAX_TOKEN_LEN)
		{
			iPosition = 0; bInToken = false; inCloseToken = false; bDoRollback = true;
			do_rollback_token(itmData, itmRollBackPos);
		}

		if ((!bInToken || !inCloseToken) && bDoRollback)
		{
			sTextSection.append(itmRollBackPos, itmData);
			iPosition = 0;
			bDoRollback = false;
		}

		if (!bInToken && !inCloseToken && !bEndToken) { sTextSection += *itmData; }
		if (bEndToken) { bEndToken = false; }

		if      (*itmData == '\t') { iPos += iTabLength; }
		else if (*itmData == '\n') { iPos = 1; iLine++; }
		else                       { iPos++; }
		itmData++;
	}

	insert_text_block(sTextSection);
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
		case TMPL_UDF:
			sToken.assign("TMPL_UDF");
			break;
		case TMPL_INCLUDE:
			sToken.assign("TMPL_INCLUDE");
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
  e_token_type eFoundTokenl = eFoundToken;
  char chQuote = '\0';

  string sParam;
  string sFunctionName;
  string::const_iterator itsParamBegin = itmData;
  string::const_iterator itsParamEnd   = itmData;

  while (itmData != itmDataEnd)
  {
    if (bInParam)
    {
      if (bIsVariable)
      {
        if (*itmData == ' ' || *itmData == '\t' || *itmData == '\r' || *itmData == '\n' || *itmData == ',')
        {
          bIsEOP = true;
          sParam.assign(itsParamBegin, itmData);
        }
        else if (*itmData == '}')
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
          if (!bRightBracketFound && bLeftBracketFound) { bRightBracketFound = true; bIsEOP = true; sParam.assign(itsParamBegin, itmData);}
          else { throw std::logic_error("Unbalanced right bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
        }
        else if (*itmData == '\'' || *itmData == '"') { throw std::logic_error("Syntax error in parameter name at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
      }
      else
      {
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
      if (*itmData == '\'' || *itmData == '"') { chQuote = *itmData; bInParam = true; bIsVariable = false; itsParamBegin = itmData; itsParamBegin++; }
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
    if (bIsEOP)
    {
      if (bLeftBracketFound)
      {
        if (!bFunctionSet)
        {
          sFunctionName.assign(sParam);
          sParam.erase();
          sStackRef.function = NULL;
          try { sStackRef.function = pFactory -> get(sFunctionName); }
          catch(std::exception &e) { throw std::logic_error("Line " + d2str<int>(iLine) + " column " + d2str<int>(iPos) + ": " + e.what()); }
          bFunctionSet = true;
        }
        else { sStackRef.function_parameters.push_back(function_param_data(sParam, bIsVariable)); sParam.erase(); }
      }
      else { sStackRef.function_parameters.push_back(function_param_data(sParam, bIsVariable)); sParam.erase(); }

      bIsEOP = false; bInParam = false; bIsVariable = false;
    }
    if (*itmData == '\\')
    {
      if (bIsVariable || !bInParam) { throw std::logic_error("Quoting allowed only in constants at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
      if (!bIsQuoted) { bIsQuoted = true; }
      else            { bIsQuoted = false; }
    }
    else { bIsQuoted = false; }

    if (iPosition == C_TEMPLATE_MAX_TOKEN_LEN)
    {
      iPosition = 0;
      do_rollback_token(itmData, itmRollBackPos);
    }

    if      (*itmData == '\t') { iPos += iTabLength; }
    else if (*itmData == '\n') { iPos = 1; iLine++; }
    else                       { iPos++; }

    if ((bIsVariable || !bInParam)  && *itmData == '}')
    {
      if (bLeftBracketFound && !bRightBracketFound) { throw std::logic_error("Unbalanced left bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
      break;
    }

    if (bRightBracketFound && *itmData != ' ' && *itmData != '\t' && *itmData != ')' && *itmData != '}') { throw std::logic_error("Syntax error at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }

    iPosition ++;
    itmData ++;
  }

  if (!bFunctionSet)
  {
    if (sStackRef.function_parameters.size() == 0) { throw std::logic_error("Need at least one parameter at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }

    if (sStackRef.function_parameters.size() != 1) { throw std::logic_error("Only one parameter are allowed at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
    sFunctionName.assign(sStackRef.function_parameters[0].param);
  }
  sStackRef.elem_name.assign(sFunctionName);
  template_ret_type sReturnType;
  switch (eFoundToken)
  {
    case TMPL_VAR:
      sReturnType.line        = iLine;
      sReturnType.column      = iPos;
      sReturnType.parse_pos   = itmData;

      if (sStackRef.function_parameters.size() == 1 && !bLeftBracketFound) {
        sStackRef.elem_name.assign(sStackRef.function_parameters[0].param);

        sStackRef.template_elem = new template_var(sStackRef.function);
        sStackRef.function      = NULL;
        sReturnType.token_type  = TMPL_VAR;

      } else {
        sStackRef.template_elem = new template_udf(sStackRef, bGlobalVars);
        sReturnType.token_type  = TMPL_UDF;
        eFoundTokenl            = TMPL_UDF;
      }
      break;

    case TMPL_IF:
      sStackRef.template_elem = new template_if(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars, true, vIncludeDir, pLoaderBase);
      sReturnType = sStackRef.template_elem -> parse_block(++itmData, itmDataEnd);
      break;

    case TMPL_UNLESS:
      sStackRef.template_elem = new template_if(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars, false, vIncludeDir, pLoaderBase);
      sReturnType = sStackRef.template_elem -> parse_block(++itmData, itmDataEnd);
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

        if (!pLoaderBase)
        {
          pLoaderBase = new loader_base();
          bSelfCreatedLoader = true;
        }
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
    default:
      throw std::logic_error("Ouch! This should not happened at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos));
  }

  iLine = sReturnType.line;
  iPos  = sReturnType.column;

  if (eFoundTokenl != sReturnType.token_type)
  {
    delete sStackRef.template_elem;
    fatal_parsing_error(eFoundTokenl, sReturnType.token_type);
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
} // namespace template_parser_ns
// End.
