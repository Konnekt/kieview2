/*-------------------------------------------------------------------------
 *
 * value_in_set.hpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         �������� �� ��������� �������� � ���������.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _VALUE_IN_SET_HPP__
#define _VALUE_IN_SET_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{

class value_in_set:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	value_in_set();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(std::vector<std::string> &vIParams);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~value_in_set();
private:
	// ���������
	std::string                  sResult;
	// ��������
	std::vector<std::string>     vParams;
};

} // namespace template_parser_std_fn_ns
#endif // _VALUE_IN_SET_HPP__
// End.
