/*-------------------------------------------------------------------------
 *
 * value_in_set.�pp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         �������� �� ��������� � ���������
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/value_in_set.hpp>

namespace template_parser_std_fn_ns
{

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� value_in_set

//
// �����������
//
value_in_set::value_in_set()
{
	;;
}

//
// ���������� ����������� ����������
//
template_parser_ns::udf_fn::e_accept_params value_in_set::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// �������� ����������
//
void value_in_set::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

void value_in_set::handler()
{
	sResult.erase();
	if (vParams.size() > 1)
	{
		std::vector<std::string>::iterator itvParams = vParams.begin();
		itvParams++;
		while (itvParams != vParams.end())
		{
			if (*itvParams == vParams[0]) { sResult.assign("1"); break; }
			itvParams++;
		}
	}
}

//
// ��������� ����������
//
std::string & value_in_set::result() { return sResult; }

//
// ����������
//
value_in_set::~value_in_set() { ;; }

} // namespace template_parser_std_fn_ns
// End.
