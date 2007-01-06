/*-------------------------------------------------------------------------
 *
 * udf_re.hpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ������� �������� �����.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _UDF_RE_HPP__
#define _UDF_RE_HPP__ 1

// C Includes
//#include <pcre.h>

// C++ Includes
#include <string>
#include <map>

// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{

//
// �������� �� email
//
class udf_re:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	udf_re();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(std::vector<std::string> &vIParams);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~udf_re();
private:
//	typedef std::map<std::string, pcre *> t_m_pcre;
	// ���������
	std::string                sResult;
	// ��������
	std::vector<std::string>   vParams;

//	// ��� ���������� ���������
//	t_m_pcre                   mRegExpPool;
//	// ������ ��� ������ LRU ��������
//	std::map<int, std::string> mLRU;
};

} // namespace template_parser_std_fn_ns
#endif // _UDF_RE_HPP__
// End.
