/*
 * Copyright (c) 2004 - 2006 CTPP Dev Team
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
 *      param_data.hpp
 *
 * $CTPP$
 */
#ifndef _CTPP_HPP__
#define _CTPP_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif
// C Includes
// None, because STL is a MUST

// C++ Includes
#include <cstdio>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// Local Includes
#include <util/d2str.hpp>
#include <ctpp/param_data.hpp>
#include <ctpp/udf_fn.hpp>

namespace template_parser_ns
{
// ��� ������
enum e_token_type{TMPL_TEXT, TMPL_VAR, TMPL_IF, TMPL_ELSE, TMPL_UNLESS, TMPL_UDF, TMPL_INCLUDE};

// ������������ ����� ������
static const unsigned int C_TEMPLATE_MAX_TOKEN_LEN = 128;

class t_template;

//
// �������� ��� INCLUDE
//
typedef std::vector<std::string> v_include_dir;

//
// ��������� �������
//
struct function_param_data
{
	std::string             param;
	bool                    is_variable;
	function_param_data(const std::string &sParam = "", const bool bIsVariable = false);
};

//
// ����
//
struct stack_ref
{
	// �������� ��������
	std::string                      elem_name;
	// ������ ��� �������
	t_template                     * template_elem;
	// �������
	udf_fn                         * function;
	// ������ ������������ ������� ����������.
	std::vector<function_param_data> function_parameters;
};
typedef std::vector<stack_ref> v_stack;

//
// ������������ ��������
//
struct template_ret_type
{
	// ��� ������������� ������
	e_token_type                token_type;
	// ������� ��������
	std::string::const_iterator parse_pos;
	// ������ ��������
	int                         line;
	// ������� ��������
	int                         column;
};

//
// ����� ��� �������� ��������
//
class loader_base
{
public:
	// �������� �����
	virtual void load_file(const std::string &sFileName);

	// ��������� ������
	virtual std::string & get_data();

	// ����������
	virtual ~loader_base() throw();

protected:
	std::string sData;
};

//
// ������� ����� �������������
//
class t_template
{
public:
	// ��������� ����
	virtual e_token_type get_type() = 0;

	// ������� ��� �������������
	virtual template_ret_type parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// ������� ������
	virtual bool param(param_data * pParamData, param_data * pIRootParamData) = 0;

	// ����� ������
	virtual std::string & output(bool & bBreak) = 0;

	// ����������
	virtual ~t_template() throw() { ;; }

protected:

	// ���������� ���������������� �������
	std::string execute_udf_fn(stack_ref &sStackRef, param_data * pParamData, param_data * pRootParamData, bool bGlobalVars);
};

//
// ��������� ����
//
class template_text:
  public t_template
{
public:
	// �����������
	template_text(template_parser_ns::udf_fn_factory * pIFactory = NULL, const int iILine = 1, const int iIPos = 1, const int iITabLength = 9, const bool bIDebug = false, const bool bIStrict = true, const bool bILoopContextVars = false, const bool bIGlobalVars = false);

	// ��������� Factory
	udf_fn_factory * get_fn_factory();

	// ��������� include_dir
	void set_include_dir(const v_include_dir & vIIncludeDir);

	// ������-��������� ��������
	void set_file_opener(loader_base * pILoaderBase);

	// ������� ��� �������������
	void parse(const std::string &sTemplate);

	// ������� ��� �������������
	void parse(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// ������� ������
	bool param(param_data * pParamData);

	// ����� ������
	std::string & output(bool & bBreak);

	// ����� ������
	inline std::string & output()
	{
		bool bBreak = false;
		return output(bBreak);
	}

	// ������� �������
	void clear_template();

	// ����������
	~template_text() throw();
private:

	// ���������� �����������
	template_text(const template_text & oTemplateText);

	// ��������� ����
	e_token_type get_type();

	// ������� ��� �������������
	template_ret_type parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// ������� ������
	bool param(param_data * pIParamData, param_data * pIRootParamData);

	// �������� ��� include
	v_include_dir    vIncludeDir;

	// ���� ������
	v_stack          vStack;
	// ����� ������
	std::string      sTextData;
	// ��������
	param_data     * pParamData;
	// �������� ��������
	param_data     * pRootParamData;

	// �������
	udf_fn_factory * pFactory;
	// ����� ������
	int              iLine;
	// ����� �������
	int              iPos;
	// ����� �������
	int              iTabLength;
	// �����
	bool             bDebug;
	// ������� �������������� �������� �������
	bool             bStrict;
	// ����������� ����������
	bool             bLoopContextVars;
	// ���������� ����������
	bool             bGlobalVars;
	// Factory ���� ���������������� ������ ������
	bool             bSelfCreatedFactory;
	// ��������� ��� ��������������� ������ ������
	bool             bSelfCreatedLoader;
	// ����� ��� ������ ��������
	loader_base    * pLoaderBase;

	// ��������, ����������� ��� ������ ������ ������
	void             do_rollback_token(std::string::const_iterator itmData, std::string::const_iterator itmRollBackPos);
	// ��������� ������ ��������
	void             fatal_parsing_error(const e_token_type &eExpectedToken, const e_token_type &eFoundToken);
	// enum -> Human Readable
	std::string      token2str(const e_token_type &eToken);
	// ������� ���������� �����
	void insert_text_block(std::string & sTextSection);
	// ������� ���������� �������
	void parse_param_string(unsigned int &iPosition, const e_token_type &eFoundToken, std::string::const_iterator & itsData, std::string::const_iterator itsDataEnd, std::string::const_iterator itmRollBackPos);
	// �������� ���� �����
	void check_include_file(const std::string & sParam, std::string &sIncludeFile);
};


//
// ����������
//
class template_var:
  public t_template
{
public:
	// �����������
	template_var(udf_fn * pIFunction = NULL);

	// ����������
	~template_var() throw();
private:
	// ���������� �����������
	template_var(const template_var &oTemplateVar);

	// ��������� ����
	e_token_type get_type();

	// ������� ������
	bool param(param_data * pParamData, param_data * pIRootParamData);

	// ����� ������
	std::string & output(bool & bBreak);

	std::string   sValue;
	std::string   sTmpValue;
	// ���������������� �������
	udf_fn      * pFunction;
};

//
// ���������� �������
//
class template_if:
  public t_template
{
public:
	// �����������
	template_if(template_parser_ns::udf_fn_factory * pIFactory, const int iILine, const int iIPos, const int iITabLength, const bool bIDebug, const bool bIStrict, const bool bILoopContextVars, const bool bIGlobalVars, const bool bIIsIf, const v_include_dir &vIIncludeDir, loader_base * pILoaderBase);

	// ����������
	~template_if() throw();

private:
	friend class template_text;

	template_if(const template_if &oTemplateIf);

	// ��������� ����
	e_token_type get_type();

	// ������� ��� �������������
	template_ret_type parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// ������� ������
	bool param(param_data * pIParamData, param_data * pIRootParamData);

	// �������� ��������������� ���������
	void additional_param(const bool &bIFlag);

	// ����� ������
	std::string & output(bool & bBreak);

	// ���� ������ ������� �������
	t_template     * pTemplateIfText;
	// ���� ������ ��������� �������
	t_template     * pTemplateElseText;

	// ��������
	param_data     * pParamData;
	// �������� ��������
	param_data     * pRootParamData;
	// �������������� ��������
	bool             bFlag;

	// ��������� ��������
	std::string      sTextData;

	// �������
	udf_fn_factory * pFactory;
	// ����� ������
	int              iLine;
	// ����� �������
	int              iPos;
	// ����� �������
	int              iTabLength;
	// �����
	bool             bDebug;
	// ������� �������������� �������� �������
	bool             bStrict;
	// ����������� ����������
	bool             bLoopContextVars;
	// ���������� ����������
	bool             bGlobalVars;
	// �������������� ����� IF
	bool             bIsIf;
	// �������� ��� include
	const v_include_dir &vIncludeDir;
	// ����� ��� ������ ��������
	loader_base       * pLoaderBase;
};

//
// ���������������� �������
//
class template_udf:
  public t_template
{
public:
	// �����������
	template_udf(const stack_ref &sIStackRef, bool bIGlobalVars);

	// ����������
	~template_udf() throw();

private:
	// ���������� �����������
	template_udf(const template_udf & oTemplateUDF);

	// ��������� ����
	e_token_type get_type();

	// ������� ������
	bool param(param_data * pIParamData, param_data * pIRootParamData);

	// ����� ������
	std::string & output(bool & bBreak);

	// ���������������� �������
	stack_ref     sStackRef;
	// ������������� ���������� ����������
	bool          bGlobalVars;

	param_data  * pParamData;
	param_data  * pRootParamData;

	std::string   sValue;
};

//
// ��������
//
class template_include:
  public t_template
{
public:
	// �����������
	template_include(udf_fn_factory * pIFactory, const int iITabLength, const bool bIDebug, const bool bIStrict, const bool bILoopContextVars, const bool bIGlobalVars, const v_include_dir &vIIncludeDir, loader_base * pILoaderBase);

	// ����������
	~template_include() throw();
private:
	// ����� ������
	std::string & output(bool & bBreak);

	// ���������� �����������
	template_include(const template_include & oTemplateInclude);

	// ������� ��� �������������
	template_ret_type parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// ��������� ����
	e_token_type get_type();

	// ������� ������
	bool param(param_data * pIParamData, param_data * pIRootParamData);

	// ������������
	t_template         * pTemplateText;
};

} // namespace template_parser_ns
#endif // _CTPP_HPP__
// End.
