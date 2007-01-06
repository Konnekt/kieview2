/*-------------------------------------------------------------------------
 *
 * ctpp_c.cpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         С-Враппер
 *
 *
 *-------------------------------------------------------------------------
 */

// Local Includes
#include <ctpp/ctpp_c.h>
#include <ctpp/ctpp.hpp>


//
// Инициализация движка
//
C_CTPP * template_text_init(void)
{
	C_CTPP * pCTPP = new C_CTPP;
	using namespace template_parser_ns;
	try
	{
		template_text * pTemplate = new template_text();
		pCTPP -> p_ctpp     = pTemplate;
		pCTPP -> error      = NULL;
		pCTPP -> error_code = 0;
	}
	catch(std::exception &e)
	{
		pCTPP -> p_ctpp     = NULL;
		pCTPP -> error      = strdup(e.what());
		pCTPP -> error_code = -1;
	}
	catch(...)
	{
		pCTPP -> p_ctpp     = NULL;
		pCTPP -> error      = strdup("Ouch!");
		pCTPP -> error_code = -1;
	}

return pCTPP;
}

//
// Парзинг шаблона
//
int template_text_set_include_dirs(C_CTPP * pCTPP, const char ** pszDirs)
{
	using namespace template_parser_ns;
	if (pCTPP -> error != NULL)  { free((void *)(pCTPP -> error)); }
	if (pszDirs)
	{
		pCTPP -> error_code = 0;
		try
		{
			v_include_dir vIncludeDir;
			int iI = 0;
			while(pszDirs[iI])
			{
				if (*pszDirs[iI]) { vIncludeDir.push_back(pszDirs[iI]); }
				iI++;
			}
			static_cast<template_text *>(pCTPP -> p_ctpp) -> set_include_dir(vIncludeDir);
		}
		catch(std::exception &e) { pCTPP -> error = strdup(e.what()); pCTPP -> error_code = -1; }
		catch(...)               { pCTPP -> error = strdup("Ouch!");  pCTPP -> error_code = -1; }
	}
	else { pCTPP -> error = strdup("pszDirs == NULL"); pCTPP -> error_code = -1; }

return pCTPP -> error_code;
}

//
// Парзинг шаблона
//
int template_text_parse(C_CTPP * pCTPP, const char * szTemplate)
{
	using namespace template_parser_ns;
	pCTPP -> error_code = 0;
	if (pCTPP -> error != NULL)  { free((void *)(pCTPP -> error)); }
	try
	{
		static_cast<template_text *>(pCTPP -> p_ctpp) -> parse(szTemplate);
	}
	catch(std::exception &e) { pCTPP -> error = strdup(e.what()); pCTPP -> error_code = -1; }
	catch(...)               { pCTPP -> error = strdup("Ouch!");  pCTPP -> error_code = -1; }
return pCTPP -> error_code;
}

//
// Вставка переменных
//
int template_text_param(C_CTPP * pCTPP, P_DATA * pData)
{
	using namespace template_parser_ns;
	pCTPP -> error_code = 0;
	if (pCTPP -> error != NULL)  { free((void *)(pCTPP -> error)); }
	try
	{
		static_cast<template_text *>(pCTPP -> p_ctpp) -> param(static_cast<param_data *>(pData -> p_param_data));
	}
	catch(std::exception &e) { pCTPP -> error = strdup(e.what()); pCTPP -> error_code = -1; }
	catch(...)               { pCTPP -> error = strdup("Ouch!");  pCTPP -> error_code = -1; }
return pCTPP -> error_code;
}

//
// Вывод результата
//
int template_text_output(C_CTPP * pCTPP, const char ** szOutput, long * lResultLength)
{
	using namespace template_parser_ns;
	*szOutput      = NULL;
	*lResultLength = 0;
	pCTPP -> error_code = 0;
	if (pCTPP -> error != NULL)  { free((void *)(pCTPP -> error)); }
	try
	{
		bool bDummy = true;
		std::string &sOutput = static_cast<template_text *>(pCTPP -> p_ctpp) -> output(bDummy);
		*szOutput       = sOutput.c_str();
		*lResultLength  = sOutput.length();
	}
	catch(std::exception &e) { pCTPP -> error = strdup(e.what()); pCTPP -> error_code = -1; }
	catch(...)               { pCTPP -> error = strdup("Ouch!");  pCTPP -> error_code = -1; }
return pCTPP -> error_code;
}

//
// Очистка шаблона
//
int template_text_clear_template(C_CTPP * pCTPP)
{
	using namespace template_parser_ns;
	pCTPP -> error_code = 0;
	if (pCTPP -> error != NULL)  { free((void *)(pCTPP -> error)); }
	try
	{
		static_cast<template_text *>(pCTPP -> p_ctpp) -> clear_template();
	}
	catch(std::exception &e) { pCTPP -> error = strdup(e.what()); pCTPP -> error_code = -1; }
	catch(...)               { pCTPP -> error = strdup("Ouch!");  pCTPP -> error_code = -1; }
return pCTPP -> error_code;
}

//
// Отключение движка
//
void template_text_destroy(C_CTPP * pCTPP)
{
	using namespace template_parser_ns;
	if (pCTPP -> error != NULL)  { free((void *)(pCTPP -> error)); }
	if (pCTPP -> p_ctpp != NULL) { delete static_cast<template_text *>(pCTPP -> p_ctpp); }

	delete pCTPP;
}

// /////////////////////////////////////////////////////////////////////////////

//
// Инициализация движка
//
P_DATA * pd_init(int iType)
{
	P_DATA * pParamData = new P_DATA;
	using namespace template_parser_ns;
	try
	{
		param_data::e_value_type eValType;
		switch (iType)
		{
			case V_VAL:
				eValType = param_data::VAR;
				break;
			case V_ARRAY:
				eValType = param_data::ARRAY;
				break;
			case V_HASH:
				eValType = param_data::HASH;
				break;
			default:
				throw std::logic_error("pd_init: This should NOT happened!");
		}
		param_data * pData = new param_data(eValType);
		pParamData -> p_param_data = pData;
		pParamData -> error        = NULL;
		pParamData -> error_code   = 0;
	}
	catch(std::exception &e)
	{
		pParamData -> p_param_data = NULL;
		pParamData -> error        = strdup(e.what());
		pParamData -> error_code   = -1;
	}
	catch(...)
	{
		pParamData -> p_param_data = NULL;
		pParamData -> error        = strdup("Ouch!");
		pParamData -> error_code   = -1;
	}

return pParamData;
}

//
// Тип переменной
//
int pd_get_value_type(P_DATA * pParamData)
{
	using namespace template_parser_ns;
	pParamData -> error_code = 0;
	if (pParamData -> error != NULL)  { free((void *)(pParamData -> error)); }
	try
	{
		param_data::e_value_type eValType = static_cast<param_data *>(pParamData -> p_param_data) -> get_value_type();
		switch(eValType)
		{
			case param_data::VAR:
				return V_VAL;
				break;
			case param_data::ARRAY:
				return V_ARRAY;
				break;
			case param_data::HASH:
				return V_HASH;
				break;
		}
	}
	catch(std::exception &e) { pParamData -> error = strdup(e.what()); pParamData -> error_code = -1; }
	catch(...)               { pParamData -> error = strdup("Ouch!");  pParamData -> error_code = -1; }
return pParamData -> error_code;
}

//
// Вставка переменной в хеш
//
P_DATA * pd_hash_insert_new_var(P_DATA * pParamData, const char * szKey, const char * szValue)
{
	using namespace template_parser_ns;
	P_DATA * pNewParamData = NULL;
	if (szKey && *szKey && szValue && *szValue)
	{
		pNewParamData = new P_DATA;
		pNewParamData -> error_code   = 0;
		pNewParamData -> error        = NULL;
		pNewParamData -> p_param_data = NULL;

		pParamData -> error_code = 0;
		if (pParamData -> error != NULL)  { free((void *)(pParamData -> error)); }
		try
		{
			pNewParamData -> p_param_data = static_cast<param_data *>(pParamData -> p_param_data) -> hash_insert_new_var(szKey, szValue);
		}
		catch(std::exception &e) { pParamData -> error = strdup(e.what()); pParamData -> error_code = -1; }
		catch(...)               { pParamData -> error = strdup("Ouch!");  pParamData -> error_code = -1; }

		if (pParamData -> error_code == -1)
		{
			delete pNewParamData;
			pNewParamData = NULL;
		}
	}

return pNewParamData;
}

//
// Вставка массива в хеш
//
P_DATA * pd_hash_insert_new_array(P_DATA * pParamData, const char * szKey)
{
	using namespace template_parser_ns;
	P_DATA * pNewParamData = NULL;
	if (szKey && *szKey)
	{
		pNewParamData = new P_DATA;
		pNewParamData -> error_code   = 0;
		pNewParamData -> error        = NULL;
		pNewParamData -> p_param_data = NULL;

		pParamData -> error_code = 0;
		if (pParamData -> error != NULL)  { free((void *)(pParamData -> error)); }
		try
		{
			pNewParamData -> p_param_data = static_cast<param_data *>(pParamData -> p_param_data) -> hash_insert_new_array(szKey);
		}
		catch(std::exception &e) { pParamData -> error = strdup(e.what()); pParamData -> error_code = -1; }
		catch(...)               { pParamData -> error = strdup("Ouch!");  pParamData -> error_code = -1; }

		if (pParamData -> error_code == -1)
		{
			delete pNewParamData;
			pNewParamData = NULL;
		}
	}

return pNewParamData;
}

//
// Вставка переменной в массив
//
P_DATA * pd_array_insert_new_var(P_DATA * pParamData, const char * szKey)
{
	using namespace template_parser_ns;
	P_DATA * pNewParamData = NULL;
	if (szKey && *szKey)
	{
		pNewParamData = new P_DATA;
		pNewParamData -> error_code   = 0;
		pNewParamData -> error        = NULL;
		pNewParamData -> p_param_data = NULL;

		pParamData -> error_code = 0;
		if (pParamData -> error != NULL)  { free((void *)(pParamData -> error)); }
		try
		{
			pNewParamData -> p_param_data = static_cast<param_data *>(pParamData -> p_param_data) -> array_insert_new_var(szKey);
		}
		catch(std::exception &e) { pParamData -> error = strdup(e.what()); pParamData -> error_code = -1; }
		catch(...)               { pParamData -> error = strdup("Ouch!");  pParamData -> error_code = -1; }

		if (pParamData -> error_code == -1)
		{
			delete pNewParamData;
			pNewParamData = NULL;
		}
	}

return pNewParamData;
}

//
// Вставка хеша в массив
//
P_DATA * pd_array_insert_new_hash(P_DATA * pParamData)
{
	using namespace template_parser_ns;
	P_DATA * pNewParamData = new P_DATA;

	pNewParamData -> error_code   = 0;
	pNewParamData -> error        = NULL;
	pNewParamData -> p_param_data = NULL;

	pParamData -> error_code = 0;
	if (pParamData -> error != NULL)  { free((void *)(pParamData -> error)); }
	try
	{
		pNewParamData -> p_param_data = static_cast<param_data *>(pParamData -> p_param_data) -> array_insert_new_hash();
	}
	catch(std::exception &e) { pParamData -> error = strdup(e.what()); pParamData -> error_code = -1; }
	catch(...)               { pParamData -> error = strdup("Ouch!");  pParamData -> error_code = -1; }

	if (pParamData -> error_code == -1)
	{
		delete pNewParamData;
		pNewParamData = NULL;
	}

return pNewParamData;
}

//
// Удаление переменной из хеша
//
int pd_hash_erase_var(P_DATA * pParamData, const char * szKey)
{
	using namespace template_parser_ns;
	pParamData -> error_code = 0;
	if (pParamData -> error != NULL)  { free((void *)(pParamData -> error)); }
	if (szKey && *szKey)
	{
		try
		{
			static_cast<param_data *>(pParamData -> p_param_data) -> hash_erase_var(szKey);
		}
		catch(std::exception &e) { pParamData -> error = strdup(e.what()); pParamData -> error_code = -1; }
		catch(...)               { pParamData -> error = strdup("Ouch!");  pParamData -> error_code = -1; }
	}
return pParamData -> error_code;
}

//
// Отключение движка
//
void pd_destroy(P_DATA * pParamData)
{
	using namespace template_parser_ns;
	if (pParamData -> error != NULL)        { free((void *)(pParamData -> error)); }
	if (pParamData -> p_param_data != NULL) { delete static_cast<param_data *>(pParamData -> p_param_data); }

	delete pParamData;
}
// End.
