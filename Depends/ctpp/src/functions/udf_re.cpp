/*-------------------------------------------------------------------------
 *
 * udf_re.�pp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         �������� �� ��������� � ���������
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/udf_re.hpp>

namespace template_parser_std_fn_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� udf_re
//
#define C_MAX_RE_RESULTS 50

//
// �����������
//
udf_re::udf_re()
{
	;;
}

//
// ���������� ����������� ����������
//
template_parser_ns::udf_fn::e_accept_params udf_re::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// �������� ����������
//
void udf_re::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

//
// ����������
//
void udf_re::handler()
{
	sResult.erase();
/*	// ������ ��� �����������
	int aResults[C_MAX_RE_RESULTS];

	pcre * pPcre;

	// ���� ���������� ��������� � ����
	t_m_pcre::const_iterator itmRegExpPool = mRegExpPool.find(vParams[1]);
	if (itmRegExpPool != mRegExpPool.end()) { pPcre = itmRegExpPool -> second; }
	// ������� ��������� � ��������� ��� � ���
	else
	{
		const char * szErrStr;
		int          iErrChar;
		pPcre = pcre_compile(vParams[1].c_str(), PCRE_CASELESS | PCRE_MULTILINE, &szErrStr, &iErrChar, NULL);

		if (pPcre == NULL)
		{
			if (szErrStr != NULL) { throw std::logic_error("in template '" + vParams[1] + "', at " + d2str<int>(iErrChar) + ": " + szErrStr); }
			else { throw std::logic_error("ouch!"); }
		}
//		// ������� LRU ������
//		pPcre
//		// ������ ��� ������ LRU ��������
//		mLRU;
		mRegExpPool[vParams[1]] = pPcre;
	}

	int iPairsFound = pcre_exec(pPcre, NULL, vParams[0].c_str(), vParams[0].length(), 0, PCRE_NOTEMPTY, aResults, C_MAX_RE_RESULTS));
	if (iPairsFound > 0)
	{
		for(unsigned int iI = 0; iI < iPairsFound; iI ++)
		{
//			for(j=vector[i*2];j<vector[i*2+1];j++) { putchar(str2[j]); }
//
		}
	}
*/
}

//
// ��������� ����������
//
std::string & udf_re::result() { return sResult; }

//
// ����������
//
udf_re::~udf_re() { ;; }



} // namespace template_parser_std_fn_ns
// End.
