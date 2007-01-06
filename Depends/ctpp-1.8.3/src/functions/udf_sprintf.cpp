/*-------------------------------------------------------------------------
 *
 * udf_sprintf.�pp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         �������� �� ��������� � ���������
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/udf_sprintf.hpp>

#include <stdio.h>

namespace template_parser_std_fn_ns
{

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� udf_sprintf

//
// �����������
//
udf_sprintf::udf_sprintf()
{
	;;
}

//
// ���������� ����������� ����������
//
template_parser_ns::udf_fn::e_accept_params udf_sprintf::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// �������� ����������
//
void udf_sprintf::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

void udf_sprintf::handler()
{
	sResult.erase();
}

//
// ��������� ����������
//
std::string & udf_sprintf::result() { return sResult; }

//
// ����������
//
udf_sprintf::~udf_sprintf() { ;; }

} // namespace template_parser_std_fn_ns
// End.
