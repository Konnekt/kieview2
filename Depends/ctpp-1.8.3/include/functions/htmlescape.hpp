/*-------------------------------------------------------------------------
 *
 * htmlescape.hpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ���������� ������� NUM_FORMAT.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _HTMLESCAPE_HPP__
#define _HTMLESCAPE_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

// C++ Includes
#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{
class htmlescape:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	htmlescape();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(const std::string &sParam);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~htmlescape();
private:
	// ���������
	std::string     sResult;
	// ��������
	std::string     sParam;

};
} // namespace template_parser_std_fn_ns
#endif // _HTMLESCAPE_HPP__
// End.
