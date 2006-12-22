/*-------------------------------------------------------------------------
 *
 * is_a.hpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ������� �������� �����.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _IS_A_HPP__
#define _IS_A_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{

//
// �������� �� email
//
class udf_is_email:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	udf_is_email();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(std::vector<std::string> &vIParams);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~udf_is_email();
private:
	// ���������
	std::string                  sResult;
	// ��������
	std::vector<std::string>     vParams;
};

//
// �������� �� ������� ������ ��������� ����
//
class udf_is_alpha:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	udf_is_alpha();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(std::vector<std::string> &vIParams);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~udf_is_alpha();
private:
	// ���������
	std::string                  sResult;
	// ��������
	std::vector<std::string>     vParams;
};

//
// �������� �� ������� ������ ��������� ����
//
class udf_is_alnum:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	udf_is_alnum();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(std::vector<std::string> &vIParams);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~udf_is_alnum();
private:
	// ���������
	std::string                  sResult;
	// ��������
	std::vector<std::string>     vParams;
};

//
// �������� �� ������� ������ ��������� ����
//
class udf_is_num:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	udf_is_num();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(std::vector<std::string> &vIParams);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~udf_is_num();
private:
	// ���������
	std::string                  sResult;
	// ��������
	std::vector<std::string>     vParams;
};

//
// �������� �� ������� ������ ��������� ���� � ����
//
class udf_is_int:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	udf_is_int();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(std::vector<std::string> &vIParams);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~udf_is_int();
private:
	// ���������
	std::string                  sResult;
	// ��������
	std::vector<std::string>     vParams;
};

//
// �������� �� ������� ������ ��������� ���� � ����
//
class udf_is_float:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	udf_is_float();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(std::vector<std::string> &vIParams);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~udf_is_float();
private:
	// ���������
	std::string                  sResult;
	// ��������
	std::vector<std::string>     vParams;
};

} // namespace template_parser_std_fn_ns
#endif // _IS_A_HPP__
// End.
