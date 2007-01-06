/*-------------------------------------------------------------------------
 *
 * form_param.hpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         �������� �� ��������� �������� � ���������.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _FORM_PARAM_HPP__
#define _FORM_PARAM_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{

class form_param:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	form_param();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(std::vector<std::string> &vIParams);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~form_param();
private:
	// ���������
	std::string                  sResult;
	// ��������
	std::vector<std::string>     vParams;
	// ������������ ������ ������
	bool                         bIgnoreEmptyData;
	// �������� 1
	std::string                  sParam1;
	// �������� 2
	std::string                  sParam2;
	// �������� 3
	std::string                  sParam3;

	// ESCAPE
	std::string escape_string(const std::string &sParam);
};

} // namespace template_parser_std_fn_ns
#endif // _FORM_PARAM_HPP__
// End.
