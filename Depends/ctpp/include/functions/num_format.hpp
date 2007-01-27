/*-------------------------------------------------------------------------
 *
 * num_format.hpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ���������� ������� NUM_FORMAT.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _NUM_FORMAT_HPP__
#define _NUM_FORMAT_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

// C++ Includes
#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{
class num_format:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	num_format();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	void param(const std::string &sParam);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~num_format();
private:
	// ���������
	int             iPeriodNum;
	char            chDelimiter;

	// ���������
	std::string     sResult;
	// ��������
	std::string     sParam;

};
} // namespace template_parser_std_fn_ns
#endif // _NUM_FORMAT_HPP__
// End.
