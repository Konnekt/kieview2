/*-------------------------------------------------------------------------
 *
 * istrue.cpp
 *
 *         ��������: BSD.
 *         
 *
 *         ���������� ������� ISTRUE.
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/istrue.hpp>

namespace template_parser_std_fn_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� istrue
//

//
// �����������
//
istrue::istrue() { ;; }

//
// ���� ��������
//
template_parser_ns::udf_fn::e_accept_params istrue::accept_params() { return template_parser_ns::udf_fn::ONE_PARAM; }

//
// ��������
//
void istrue::param(const std::string &sIParam) { sParam.assign(sIParam); }

//
// ����������
//
void istrue::handler()
{
	sResult.clear();
	if( !sParam.empty() )
	{
		if( sParam[0]=='1' ||
		    sParam[0]=='y' ||
		    sParam[0]=='Y' ||
		    sParam[0]=='t' ||
		    sParam[0]=='T') { sResult = "1"; }
	}
}

//
// ���������
//
std::string &istrue::result() { return sResult; }

//
// ����������
//
istrue::~istrue() { ;; }

} // namespace template_parser_std_fn_ns
// End.
