/*-------------------------------------------------------------------------
 *
 * num_format.cpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ���������� ������� NUM_FORMAT.
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/num_format.hpp>

namespace template_parser_std_fn_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� urlescape
//

//
// �����������
//
num_format::num_format(): iPeriodNum(3), chDelimiter(',')
{
	;;
}

//
// ���������� ����������� ����������
//
template_parser_ns::udf_fn::e_accept_params num_format::accept_params() { return template_parser_ns::udf_fn::ONE_PARAM; }

//
// �������� ����������
//
void num_format::param(const std::string &sIParam) { sParam.assign(sIParam); }


//
// ����������
//
void num_format::handler()
{
	// �������� ������ �������
	int iOffset = sParam.length() % iPeriodNum;
	if (iOffset == 0) { iOffset = iPeriodNum; }

	std::string::const_iterator itsParam = sParam.begin();
	int iI = 0;
	sResult.erase();
	while(itsParam != sParam.end())
	{
		if (iI == iOffset) { sResult += chDelimiter; iOffset += iPeriodNum; }
		sResult += *itsParam;
		itsParam++; iI++;
	}
}

//
// ��������� ����������
//
std::string &num_format::result() { return sResult; }

//
// ����������
//
num_format::~num_format() { ;; }

} // namespace template_parser_std_fn_ns
// End.
