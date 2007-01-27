/*-------------------------------------------------------------------------
 *
 * htmlescape.cpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ���������� ������� NUM_FORMAT.
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/htmlescape.hpp>

namespace template_parser_std_fn_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� urlescape
//

//
// �����������
//
htmlescape::htmlescape()
{
	;;
}

//
// ���������� ����������� ����������
//
template_parser_ns::udf_fn::e_accept_params htmlescape::accept_params() { return template_parser_ns::udf_fn::ONE_PARAM; }

//
// �������� ����������
//
void htmlescape::param(const std::string &sIParam) { sParam.assign(sIParam); }


//
// ����������
//
void htmlescape::handler()
{
	sResult.erase();
	std::string::const_iterator itsParam = sParam.begin();
	int iI = 0;
	while(itsParam != sParam.end())
	{
		switch (*itsParam)
		{
			case '"':
				sResult += "&quot;";
				break;
			case '<':
				sResult += "&lt;";
				break;
			case '>':
				sResult += "&gt;";
				break;
			case '&':
				sResult += "&amp;";
				break;
			case '\'':
				sResult += "&#039";
				break;
			default:
				sResult += *itsParam;
		}			
		itsParam++; iI++;
	}
}

//
// ��������� ����������
//
std::string &htmlescape::result() { return sResult; }

//
// ����������
//
htmlescape::~htmlescape() { ;; }

} // namespace template_parser_std_fn_ns
// End.
