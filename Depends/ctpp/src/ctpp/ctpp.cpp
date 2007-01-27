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
// ��������� function_param_data
//

//
// �����������
//
function_param_data::function_param_data(const std::string &sParam, const bool bIsVariable): param(sParam), is_variable(bIsVariable) { ;; }

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� loader_base
//

//
// �������� �����
//
void loader_base::load_file(const std::string &sFileName)
{
	char szBuffer[16385];
	sData.erase();

	FILE * fData = fopen(sFileName.c_str(), "r");
	if (fData == NULL) { throw std::logic_error("Cannot open file '" + sFileName + "' for reading."); }

	// ������ �� �����
	while(!feof(fData))
	{
		size_t iSize = fread(szBuffer, 1, 16384, fData);
		szBuffer[iSize] = '\0';
		sData += szBuffer;
	}
	fclose(fData);
}

//
// ��������� ������
//
std::string & loader_base::get_data() { return sData; }

//
// ����������
//
loader_base::~loader_base() throw() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� t_template.
//

//
// ������� ��� �������������
//
template_ret_type t_template::parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd) { throw std::logic_error("method parse must be defined"); }

//
// ���������� ���������������� �������
//
std::string t_template::execute_udf_fn(stack_ref &sStackRef, param_data * pParamData, param_data * pRootParamData, bool bGlobalVars)
{
	// �������� ���
	t_param_hash * pvHash = pParamData -> hash();
	t_param_hash::iterator itpvHash;

	using std::string;
	using std::vector;

	// ������� ��������� ����� ���� ��������
	if (sStackRef.function -> accept_params() == udf_fn::ONE_PARAM)
	{
		// ������� �������� ������� > 1 ���������
		int iParamCount = sStackRef.function_parameters.size();
		if (iParamCount > 1) { throw std::logic_error("Attempt to call function with more than 1 parameter."); }

		if (iParamCount == 1)
		{
			// ���� ��������
			if (sStackRef.function_parameters[0].is_variable)
			{
				itpvHash = pvHash -> find(sStackRef.function_parameters[0].param);
				if (itpvHash != pvHash -> end()) { sStackRef.function -> param(*(itpvHash -> second -> val())); }
				else if (bGlobalVars)
				{
					itpvHash = pRootParamData -> hash() -> find(sStackRef.function_parameters[0].param);
					if (itpvHash != pRootParamData -> hash() -> end()) { sStackRef.function -> param(*(itpvHash -> second -> val())); }
					// �������
					else { sStackRef.function -> param(""); }
				}
				// �������
				else { sStackRef.function -> param(""); }
			}
			// ���� ���������
			else { sStackRef.function -> param(sStackRef.function_parameters[0].param); }
		}
		// �������
		else { sStackRef.function -> param(""); }
	}
	// ������� ��������� ��� ���������
	else if (sStackRef.function -> accept_params() == udf_fn::TWO_PARAMS)
	{
		// ������� �������� ������� > 2 ����������
		int iParamCount = sStackRef.function_parameters.size();
		if (iParamCount > 2) { throw std::logic_error("Attempt to call function with more than 2 parameters."); }

		string sFirstParam("");
		string sSecondParam("");
		// ����� 1 ��� 2 ���������
		if (iParamCount >= 1)
		{
			// ���� ��������
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
			// ���� ���������
			else { sFirstParam.assign(sStackRef.function_parameters[0].param); }
		}
		// ������ 2 ���������
		if (iParamCount == 2)
		{
			// ���� ��������
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
			// ���� ���������
			else { sSecondParam.assign(sStackRef.function_parameters[1].param); }
		}
		sStackRef.function -> param(sFirstParam, sSecondParam);
	}
	else if (sStackRef.function -> accept_params() == udf_fn::THREE_PARAMS)
	{
		// ������� �������� ������� > 3 ����������
		int iParamCount = sStackRef.function_parameters.size();
		if (iParamCount > 3) { throw std::logic_error("Attempt to call function with more than 3 parameters."); }

		string sFirstParam("");
		string sSecondParam("");
		string sThirdParam("");
		// ����� 1 ��� 2 ���������
		if (iParamCount >= 1)
		{
			// ���� ��������
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
			// ���� ���������
			else { sFirstParam.assign(sStackRef.function_parameters[0].param); }

		}
		// ������ 2 ���������
		if (iParamCount >= 2)
		{
			// ���� ��������
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
			// ���� ���������
			else { sSecondParam.assign(sStackRef.function_parameters[1].param); }

		}
		// ������ 3 ���������
		if (iParamCount == 3)
		{
			// ���� ��������
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
			// ���� ���������
			else { sThirdParam.assign(sStackRef.function_parameters[2].param); }
		}
		sStackRef.function -> param(sFirstParam, sSecondParam, sThirdParam);
	}
	// ������������ ���������� ����������
	else if (sStackRef.function -> accept_params() == udf_fn::ANY_PARAMS)
	{
		std::vector<function_param_data>::iterator itvParamNames = sStackRef.function_parameters.begin();
		vector<string> vParams;
		while (itvParamNames != sStackRef.function_parameters.end())
		{
			string sParamData("");
			// ���� ��������
			if (itvParamNames -> is_variable)
			{
				// ��������� ����������
				itpvHash = pvHash -> find(itvParamNames -> param);

				if (itpvHash != pvHash -> end()) { sParamData.assign(*(itpvHash -> second -> val())); }
				else if (bGlobalVars)
				{
					itpvHash = pRootParamData -> hash() -> find(itvParamNames -> param);
					if (itpvHash != pRootParamData -> hash() -> end()) { sParamData.assign(*(itpvHash -> second -> val())); }
				}
			}
			// ���� ���������
			else { sParamData.assign(itvParamNames -> param); }

			vParams.push_back(sParamData);
			++itvParamNames;
		}
		sStackRef.function -> param(vParams);
	}
	// �� ����� ���� ������
	else { throw std::logic_error("Function can accept ONLY 1, 2, 3 or > 3 paramaters."); }

	// ��������� �������
	sStackRef.function -> handler();
	// ��������� ��������� ?

return sStackRef.function -> result();
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� template_text
//

//
// �����������
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
	// ���� �� ������ Factory ��� ����������� �������, �� ������� ��
	// � ��������� � ��� ��� ����������� �������
	if (pFactory == NULL)
	{
		pFactory = new udf_fn_factory();
		std_fn_list oList;
		oList.set_all(pFactory);
		bSelfCreatedFactory = true;
	}
}

//
// ��������� Factory ��� ��������� �������������� �������.
//
udf_fn_factory * template_text::get_fn_factory() { return pFactory; }

//
// ��������� include_dir
//
void template_text::set_include_dir(const v_include_dir & vIIncludeDir) { vIncludeDir = vIIncludeDir; }

//
// ��������� ���������� ������
//
void template_text::set_file_opener(loader_base * pILoaderBase) { pLoaderBase = pILoaderBase; }

//
// ��������� ����
//
e_token_type template_text::get_type() { return TMPL_TEXT; }

//
// ������� ������
//
bool template_text::param(param_data * pParamData) { return param(pParamData, pParamData); }

//
// ������� ������
//
bool template_text::param(param_data * pIParamData, param_data * pIRootParamData)
{
	using std::string;
	using std::vector;

	if (pIParamData && pIRootParamData)
	{
		pParamData = pIParamData;
		pRootParamData = pIRootParamData;
		// �������� ���
		t_param_hash * pvHash = pParamData -> hash();
		t_param_hash::iterator itpvHash;

		// ��������� �� ����� ��������
		v_stack::iterator itvStack = vStack.begin();
		while (itvStack != vStack.end())
		{
			// ���� �� ����� ������
			if (itvStack -> template_elem)
			{
				// ���� �� ����� ������ ���� TMPL_var, �� ��������� �� ������� ���������� � ��������� ������� ���������,
				// � ���� �������� ���������� ���������� - �� � ���������� �������.
				if (itvStack -> template_elem -> get_type() == TMPL_VAR)
				{
					// ������� - ��������� ����������
					itpvHash = pvHash -> find(itvStack -> elem_name);
					if (itpvHash != pvHash -> end()) { itvStack -> template_elem -> param(itpvHash -> second, pRootParamData); }
					// ����� - ���������� ����������
					else if (bGlobalVars)
					{
						itpvHash = pRootParamData -> hash() -> find(itvStack -> elem_name);
						// ��� �������
						if (itpvHash != pRootParamData -> hash() -> end()) { itvStack -> template_elem -> param(itpvHash -> second, pRootParamData); }
					}
				}
				// ���� �� ����� ������ ���� TMPL_loop, �� ��������� �� ������� ������� � ��������� ������� ���������
				else if (itvStack -> template_elem -> get_type() == TMPL_LOOP)
				{
					itpvHash = pvHash -> find(itvStack -> elem_name);
					if (itpvHash != pvHash -> end()) { itvStack -> template_elem -> param(itpvHash -> second, pRootParamData); }
					else                             { itvStack -> template_elem -> param(NULL, NULL); }
				}
				// ��� ��������� �� ������ ��������� ������� � ��������� ��������� �� ���������� �� ����������
				else if (itvStack -> template_elem -> get_type() == TMPL_IF || itvStack -> template_elem -> get_type() == TMPL_UNLESS)
				{
					bool bVarFound = false;
					// ���� ������ �������
					if (itvStack -> function)
					{
						if (execute_udf_fn(*itvStack, pParamData, pRootParamData, bGlobalVars).length()) { bVarFound = true; }
					}
					// ���������������� ������� ���, �������� ����.
					else
					{
						bool bSymbolFound = false;
						itpvHash = pvHash -> find(itvStack -> elem_name);
						// ���� ������ � ��������� ������� ����
						if (itpvHash != pvHash -> end()) { bSymbolFound = true; }
						// ������ �� �������; �������� ������ � ���������� ������� ����
						else if ( bGlobalVars)
						{
							itpvHash = pRootParamData -> hash() -> find(itvStack -> elem_name);
							if (itpvHash != pRootParamData -> hash() -> end()) { bSymbolFound = true; }
						}

						// ������ ������; ��������� �� ����������
						if (bSymbolFound)
						{
							// ��� �������� ��������� ����� ������� ���������� ��� ������
							if (itpvHash -> second -> get_value_type() == param_data::VAR)
							{
								// ������ ��������
								if (itpvHash -> second -> val() -> length() != 0) { bVarFound = true; }
								// ������ ������
								// else { bVarFound = false; }
							}
							else if (itpvHash -> second -> get_value_type() == param_data::ARRAY)
							{
								// ������ ��������
								if (itpvHash -> second -> array() -> size() != 0) { bVarFound = true; }
								// ������ ������
								// else { bVarFound = false; }
							}
							else if (itpvHash -> second -> get_value_type() == param_data::HASH)
							{
								// ��� ��������
								if (itpvHash -> second -> hash() -> size() != 0) { bVarFound = true; }
								// ��� ������
								// else { bVarFound = false; }
							}
							// ��� ���� ������ ����� ��������� ������ ������� ���������� �� �����
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
				// ��� ���������������� ������� ������ �������� ������ ���������
				else if (itvStack -> template_elem -> get_type() == TMPL_UDF)     { itvStack -> template_elem -> param(pParamData, pRootParamData); }
				// ��������, etc
				else if (itvStack -> template_elem -> get_type() == TMPL_INCLUDE) { itvStack -> template_elem -> param(pParamData, pRootParamData); }
				// Declare
				else if (itvStack -> template_elem -> get_type() == TMPL_DECLARE) { itvStack -> template_elem -> param(pParamData, pRootParamData); }
				// Break
				else if (itvStack -> template_elem -> get_type() == TMPL_BREAK)
				{
					if (itvStack -> template_elem -> param(pParamData, pRootParamData) == false) { return false; }
				}
				// ������, ��������� �� ����� ���� ������
				else { throw std::logic_error("Error in template_text::param NO SUCH TYPE: " + token2str(itvStack -> template_elem -> get_type())); }
			}
			++itvStack;
		}
	}
return true;
}

//
// ������� ��� �������������
//
void template_text::parse(const std::string &sTemplate) { parse_block(sTemplate.begin(), sTemplate.end()); }

//
// ������� ��� �������������
//
void template_text::parse(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd) { parse_block(itmData, itmDataEnd); }

//
// ������� ������
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
    if (*itmData == '{' && *(itmData + 1) == '{')
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
        case 2:
          if (*itmData != '{') {
            bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos);
          }
          iPosition++;
          break;
        case 3:
          if      (*itmData == '$') { eTokenType = TMPL_VAR;     iPosition++; }
          else if (*itmData == 'i') { eTokenType = TMPL_IF;      iPosition++; }
          else if (*itmData == 'e') { eTokenType = TMPL_ELSE;    iPosition++; }
          else if (*itmData == 'u') { eTokenType = TMPL_UNLESS;  iPosition++; }
          else if (*itmData == 'b') { eTokenType = TMPL_BREAK;   iPosition++; }
          else if (*itmData == 'c') { eTokenType = TMPL_COMMENT; iPosition++; }
          else if (*itmData == 'f') { eTokenType = TMPL_UDF;     iPosition++; }
          else if (*itmData == '/') { inCloseToken = true; bInToken = false; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
        case 4:
          if      (eTokenType == TMPL_VAR)      { iPosition++; }
          else if (*itmData == 'a' && eTokenType == TMPL_ELSE)      { eTokenType = TMPL_LOOP; iPosition++; }
          else if (*itmData == 'f' && eTokenType == TMPL_IF)        { iPosition++; }
          else if (*itmData == 'l' && eTokenType == TMPL_ELSE)      { iPosition++; }
          else if (*itmData == 'n' && eTokenType == TMPL_UNLESS)    { iPosition++; }
          else if (*itmData == 'u' && eTokenType == TMPL_UDF)       { iPosition++; }
          else if (*itmData == 'n' && eTokenType == TMPL_IF)        { eTokenType = TMPL_INCLUDE; iPosition++; }
          else if (*itmData == 'r' && eTokenType == TMPL_BREAK)     { iPosition++; }
          else if (*itmData == 'o' && eTokenType == TMPL_COMMENT)   { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
      }
      // TMPL_VAR
      if (eTokenType == TMPL_VAR && iPosition > 4)
      {
        insert_text_block(sTextSection);
        parse_param_string(iPosition, TMPL_VAR, itmData, itmDataEnd, itmRollBackPos);
        bEndToken = true; bInToken = false; iPosition = 0;
      }
      // TMPL_IF
      else if (eTokenType == TMPL_IF && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'f') { iPosition++; }
          else                 { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 5)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_IF, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_ELSE
      else if (eTokenType == TMPL_ELSE && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 's') { iPosition++; }
          else                 { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'e') { iPosition++; }
          else                 { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == '}') { iPosition++; }
          else                 { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == '}') { bInToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else                 { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
      // TMPL_UNLESS
      else if (eTokenType == TMPL_UNLESS && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'n')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'l')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'e')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8 || iPosition == 9)
        {
          if (*itmData == 's')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 9)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_UNLESS, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_LOOP
      else if (eTokenType == TMPL_LOOP && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'a')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'c')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'h')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 7)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_LOOP, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_UDF
      else if (eTokenType == TMPL_UDF && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'u')  { iPosition++; }
          else                  { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'n')  { iPosition++; }
          else                  { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'c')  { iPosition++; }
          else                  { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 7)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_UDF, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
        else { iPosition++; }
      }
      // TMPL_INCLUDE
      else if (eTokenType == TMPL_INCLUDE && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'n')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'c')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'l')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == 'u')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9)
        {
          if (*itmData == 'd')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 10)
        {
          if (*itmData == 'e')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 10)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_INCLUDE, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_BREAK
      else if (eTokenType == TMPL_BREAK && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'r')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'e')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'a')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == 'k')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 8)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_BREAK, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_COMMENT
      else if (eTokenType == TMPL_COMMENT && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'o')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6 || iPosition == 7)
        {
          if (*itmData == 'm')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == 'e')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9)
        {
          if (*itmData == 'n')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 10)
        {
          if (*itmData == 't')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 10)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_COMMENT, itmData, itmDataEnd, itmRollBackPos);
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
        case 3:
           iPosition++;
           break;
        case 4:
          if      (*itmData == 'e') { eTokenType = TMPL_LOOP;    iPosition++; }
          else if (*itmData == 'i') { eTokenType = TMPL_IF;      iPosition++; }
          else if (*itmData == 'u') { eTokenType = TMPL_UNLESS;  iPosition++; }
          else if (*itmData == 'c') { eTokenType = TMPL_COMMENT; iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
        case 5:
          if      (*itmData == 'a' && eTokenType == TMPL_LOOP)      { iPosition++; }
          else if (*itmData == 'f' && eTokenType == TMPL_IF)        { iPosition++; }
          else if (*itmData == 'n' && eTokenType == TMPL_UNLESS)    { iPosition++; }
          else if (*itmData == 'o' && eTokenType == TMPL_COMMENT)   { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
      }

      if (eTokenType == TMPL_IF && iPosition > 5)
      {
        if (iPosition == 6)
        {
          if (*itmData == 'f') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == '}') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == '}') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
      else if (eTokenType == TMPL_LOOP && iPosition > 5)
      {
        if (iPosition == 6)
        {
          if (*itmData == 'a') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'c') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == 'h') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9)
        {
          if (*itmData == '}') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 10)
        {
          if (*itmData == '}') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
      else if (eTokenType == TMPL_UNLESS && iPosition > 5)
      {
        if (iPosition == 6)
        {
          if (*itmData == 'n') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'l') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == 'e') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9 || iPosition == 10)
        {
          if (*itmData == 's') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 11)
        {
          if (*itmData == '}') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 12)
        {
          if (*itmData == '}') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
      else if (eTokenType == TMPL_COMMENT && iPosition > 5)
      {
        if (iPosition == 6)
        {
          if (*itmData == 'o') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7 || iPosition == 8)
        {
          if (*itmData == 'm') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9)
        {
          if (*itmData == 'e') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 10)
        {
          if (*itmData == 'n') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 11)
        {
          if (*itmData == 't') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 12)
        {
          if (*itmData == '}') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 13)
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
// ����� ������
//
std::string & template_text::output(bool & bBreak)
{
	sTextData.erase();
	// ��������� �� ����� ��������
	v_stack::iterator itvStack = vStack.begin();

	bBreak = false;
	while (itvStack != vStack.end())
	{
		// ���� ���� ������ ������, ������ ������ ����� ��������� ����
		if (itvStack -> template_elem == NULL) { sTextData += itvStack -> elem_name; }
		// ����������� ������� ������
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
// ��������, ����������� ��� ������
//
void template_text::do_rollback_token(std::string::const_iterator itmData, std::string::const_iterator itmRollBackPos)
{
	// ���� ������� ����� ������� �������� ����������, �� ������.
	if (bStrict)
	{
		std::string sWrongToken(itmRollBackPos, itmData);
		throw std::logic_error("Syntax error in " + sWrongToken + " at line "+ d2str<int>(iLine) + " column " + d2str<int>(iPos) + ".");
	}
}

//
// �������� ��������� ������
//
void template_text::fatal_parsing_error(const e_token_type &eExpectedToken, const e_token_type &eFoundToken)
{
	throw std::logic_error("Fatal: Logic error near at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos) + ". Expected /" + token2str(eExpectedToken) + ", but found /" + token2str(eFoundToken) + ".");
}

//
// �������� ��������� ������
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
// ��������� ��������� ����
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
// ������� ���������� �������
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
      *itmData++;
      break;
    }

    if (bRightBracketFound && *itmData != ' ' && *itmData != '\t' && *itmData != ')' && *itmData != '}') { throw std::logic_error("Syntax error at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }

    iPosition ++;
    itmData ++;
  }

  if (sStackRef.function_parameters.size() == 0) { throw std::logic_error("Need at least one parameter at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }

  if (!bFunctionSet)
  {
    if (sStackRef.function_parameters.size() != 1 && eFoundToken != TMPL_LOOP && eFoundToken != TMPL_DECLARE) { throw std::logic_error("Only one parameter are allowed at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
    sFunctionName.assign(sStackRef.function_parameters[0].param);
  }
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

  if (eFoundToken != sReturnType.token_type)
  {
    delete sStackRef.template_elem;
    fatal_parsing_error(eFoundToken, sReturnType.token_type);
  }

  vStack.push_back(sStackRef);
  itmData = sReturnType.parse_pos;
}

//
// �������� ���� �����
//
void template_text::check_include_file(const std::string & sParam, std::string &sIncludeFile)
{
	bool bIncludeDirFound = false;
	// ��������� ���� �� ������� ��������� � INCLUDE_PATH
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

			// �������� ��� ������������������ "../"
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
	// ���� �����, �� ��������� ����
	if (!bIncludeDirFound) { throw std::logic_error("File: '" + sIncludeFile + "' not in IncludePath at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos) + "."); }
}

//
// ������� �������
//
void template_text::clear_template()
{
	v_stack::iterator itvStack = vStack.begin();
	while (itvStack != vStack.end()) { delete itvStack -> template_elem; itvStack++; }
	v_stack vTMPStack; vStack.swap(vTMPStack);
}

//
// ����������
//
template_text::~template_text() throw()
{
	try
	{
		clear_template();

		// ������� factory, ���� ��� ���� ������� ������ ������
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
// ����� template_var
//

//
// �����������
//
template_var::template_var(udf_fn * pIFunction): pFunction(pIFunction)
{
	;;
}

//
// ��������� ����
//
e_token_type template_var::get_type() { return TMPL_VAR; }

//
// ����� ������
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
// ������� ������
//
bool template_var::param(param_data * pParamData, param_data * pIRootParamData)
{
	if (pParamData != NULL) { sValue.assign(*(pParamData -> val())); }
	else { sValue.assign(""); }
return true;
}

//
// ����������
//
template_var::~template_var() throw() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� template_if
//

//
// �����������
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
// ��������� ����
//
e_token_type template_if::get_type() { return bIsIf ? TMPL_IF : TMPL_UNLESS; }

//
// ����� ������
//
std::string & template_if::output(bool & bBreak)
{
	sTextData.erase();
	if (pParamData && pRootParamData)
	{
		// ������ �����
		if (bFlag)                          { pTemplateIfText -> param(pParamData, pRootParamData);   sTextData += pTemplateIfText -> output(bBreak);   }
		// �������� �����
		else if (pTemplateElseText != NULL) { pTemplateElseText -> param(pParamData, pRootParamData); sTextData += pTemplateElseText -> output(bBreak); }
	}

// ���������� ��������� �������� ������
return sTextData;
}

//
// �������� ��������������� ���������
//
void template_if::additional_param(const bool & bIFlag) { bFlag = bIFlag; }

//
// ������� ������ (������ ����������� ������)
//
bool template_if::param(param_data * pIParamData, param_data * pIRootParamData)
{
	pParamData     = pIParamData;
	pRootParamData = pIRootParamData;
return true;
}

//
// ������� ��� �������������
//
template_ret_type template_if::parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd)
{
	// ������� ��� �������������
	pTemplateIfText = new template_text(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars);
	static_cast<template_text *>(pTemplateIfText) -> set_include_dir(vIncludeDir);

	// ������ ������
	template_ret_type sRetType = pTemplateIfText -> parse_block(itmData, itmDataEnd);

	// ���� �������� �����?
	if (sRetType.token_type == TMPL_ELSE)
	{
		sRetType.parse_pos++;
		// ������� ��� �������������
		pTemplateElseText = new template_text(pFactory, sRetType.line, sRetType.column, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars);
		static_cast<template_text *>(pTemplateElseText) -> set_include_dir(vIncludeDir);
		// ������ ������
		sRetType = pTemplateElseText -> parse_block(sRetType.parse_pos, itmDataEnd);
	}
	else { pTemplateElseText = NULL; }

return sRetType;
}

//
// ����������
//
template_if::~template_if() throw()
{
	// ������� ������
	delete pTemplateIfText;
	if (pTemplateElseText) { delete pTemplateElseText; }
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� template_loop
//

//
// �����������
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
// ��������� ����
//
e_token_type template_loop::get_type() { return TMPL_LOOP; }

//
// ����� ������
//
std::string & template_loop::output(bool & bBreak)
{
	sTextData.erase();
	// �������� � ����� �� ���� ��������� ����������� �������
	if (pParamData && pRootParamData)
	{
		t_param_array * pvArray = pParamData -> array();
		t_param_array::iterator itpvArray = pvArray -> begin();
		t_param_hash * pLoopParamHash;

		// ������� ���������
		int iCount = 1;
		t_param_array::iterator itpvLastElement = pvArray -> end();
		// ��������� ������� �������
		Hasher oHasher;
		UINT_64  i__FIRST__   = oHasher("__FIRST__");
		UINT_64  i__INNER__   = oHasher("__INNER__");
		UINT_64  i__LAST__    = oHasher("__LAST__");
		UINT_64  i__ODD__     = oHasher("__ODD__");
		UINT_64  i__EVEN__    = oHasher("__EVEN__");
		UINT_64  i__SIZE__    = oHasher("__SIZE__");
		UINT_64  i__COUNTER__ = oHasher("__COUNTER__");

		param_data * pVarFirstLastInner = new param_data(param_data::VAR);
		param_data * pVarEvenOdd        = new param_data(param_data::VAR);
		param_data * pVarSize           = new param_data(param_data::VAR);

		pVarSize -> val() -> assign(d2str<int>(pvArray -> size()));
		param_data * pVarCounter        = new param_data(param_data::VAR);

		if (itpvLastElement != pvArray -> begin()) { itpvLastElement--; }
		while(itpvArray != pvArray -> end())
		{
			// ���� ������������ ����������� ����������
			if (bLoopContextVars)
			{
				using CTPP::Pair;
				using std::string;
				pLoopParamHash = (*itpvArray) -> hash();
				// ��������� ���������� �����
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

				// ��������� ������
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
			// ����������� ���������� �� ������������
			else
			{
				// ��������� ������
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
// ���������� ��������� �������� ������
return sTextData;
}

//
// ������� ������
//
bool template_loop::param(param_data * pIParamData, param_data * pIRootParamData)
{
	pRootParamData = pIRootParamData;
	pParamData = pIParamData;
return true;
}

//
// ������� ��� �������������
//
template_ret_type template_loop::parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd)
{
	// ������� ��� �������������
	pTemplateText = new template_text(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars);
	static_cast<template_text *>(pTemplateText) -> set_include_dir(vIncludeDir);

	// ������ ������
	template_ret_type sRetType = pTemplateText -> parse_block(itmData, itmDataEnd);

return sRetType;
}

//
// ����������
//
template_loop::~template_loop() throw() { delete pTemplateText; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� template_udf
//

//
// �����������
//
template_udf::template_udf(const stack_ref &sIStackRef, bool bIGlobalVars): sStackRef(sIStackRef), bGlobalVars(bIGlobalVars)
{
	;;
}

//
// ��������� ����
//
e_token_type template_udf::get_type() { return TMPL_UDF; }

//
// ����� ������
//
std::string & template_udf::output(bool & bBreak)
{
	if (pParamData && pRootParamData) { sValue = execute_udf_fn(sStackRef, pParamData, pRootParamData, bGlobalVars); }
return sValue;
}

//
// ������� ������
//
bool template_udf::param(param_data * pIParamData, param_data * pIRootParamData)
{
	pParamData     = pIParamData;
	pRootParamData = pIRootParamData;
return true;
}

//
// ����������
//
template_udf::~template_udf() throw() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� template_udf
//

//
// �����������
//
template_include::template_include(udf_fn_factory * pIFactory, const int iITabLength, const bool bIDebug, const bool bIStrict, const bool bILoopContextVars, const bool bIGlobalVars, const v_include_dir &vIIncludeDir, loader_base * pILoaderBase)
{
	pTemplateText = new template_text(pIFactory, 1, 1, iITabLength, bIDebug, bIStrict, bILoopContextVars, bIGlobalVars);
	static_cast<template_text *>(pTemplateText) -> set_include_dir(vIIncludeDir);
	static_cast<template_text *>(pTemplateText) -> set_file_opener(pILoaderBase);
}

//
// ����������
//
template_include::~template_include() throw()
{
	try { delete pTemplateText; }
	catch(...) { ;; }
}

//
// ����� ������
//
std::string & template_include::output(bool & bBreak) { return pTemplateText -> output(bBreak); }

//
// ��������� ����
//
e_token_type template_include::get_type() { return TMPL_INCLUDE; }

//
// ������� ��� �������������
//
template_ret_type template_include::parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd) { return pTemplateText -> parse_block(itmData, itmDataEnd); }

//
// ������� ������
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
// �����������
//
template_break::template_break(const stack_ref &sIStackRef, bool bIGlobalVars): sStackRef(sIStackRef), bGlobalVars(bIGlobalVars)
{
	;;
}

//
// ����������
//
template_break::~template_break() throw() { ;; }

//
// ��������� ����
//
e_token_type template_break::get_type() { return TMPL_BREAK; }

//
// ������� ������
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
// ����� ������
//
std::string & template_break::output(bool & bBreak)
{
	bBreak = bBreakFlag;
	return sTMP;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// ���������� ����������
//

//
// �����������
//
template_declare::template_declare(const stack_ref &sIStackRef)
{
	if (sIStackRef.function_parameters.size() != 2) { throw std::logic_error("TMPL_declare accept ONLY 2 parameters!"); }

	oParamName  = sIStackRef.function_parameters[0];
	oParamValue = sIStackRef.function_parameters[1];
}

	// ����������
template_declare::~template_declare() throw() { ;; }

// ��������� ����
e_token_type template_declare::get_type() { return TMPL_DECLARE; }

// ������� ������
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
