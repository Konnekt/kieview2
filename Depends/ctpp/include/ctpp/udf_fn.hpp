/*-------------------------------------------------------------------------
 *
 * udf_fn.hpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ������, ����������� ��� ������ ������� ���������������� �������.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _UDF_FN_HPP__
#define _UDF_FN_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

// C Includes

// C++ Includes
#include <map>
#include <vector>
#include <string>
#include <stdexcept>

// Local Includes

namespace template_parser_ns
{
//
// ���������, ������������ �������
//
typedef std::vector<std::string> udf_fn_param;

//
// �������, ������������ �������������
//
class udf_fn
{
public:
	// ���������� ����������� ����������
	enum e_accept_params {NO_PARAMS, ONE_PARAM, TWO_PARAMS, THREE_PARAMS, ANY_PARAMS};

	// �������� ����������
	virtual void param();
	virtual void param(udf_fn_param & vParams);
	virtual void param(const std::string &sParam);
	virtual void param(const std::string &sParam1, const std::string &sParam2);
	virtual void param(const std::string &sParam1, const std::string &sParam2, const std::string &sParam3);

	// ���������� ����������� ����������
	virtual e_accept_params accept_params() = 0;

	// ����������
	virtual void handler() = 0;

	// ��������� ����������
	virtual std::string & result() = 0;

	// ����������
	virtual ~udf_fn() { ;; }
};

//
// ����������� �������, ������������ �������������
//
class udf_fn_factory
{
public:
	// ��������� ���������������� �������
	void install_udf_fn(const std::string &sFunctionName, udf_fn * pUDFFn);
	// �������� ���������������� �������
	void remove_udf_fn(const std::string &sFunctionName);
	// ��������� ������ �� �������
	udf_fn * get(const std::string &sFunctionName);
	// �������� ���� �������
	void clear();
	// ����������
	virtual ~udf_fn_factory();
private:

	// ��� factory
	typedef std::map<std::string, udf_fn *>      m_functions_factory;
	// Factory
	m_functions_factory                          mFunctionsFactory;
};

} // namespace template_parser_ns
#endif // _UDF_FN_HPP__
// End.
