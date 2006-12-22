/*-------------------------------------------------------------------------
 *
 * udf_sprintf.hpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ���������� ������� URLESCAPE.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _UDF_SPRINTF_HPP__
#define _UDF_SPRINTF_HPP__ 1

// C++ Includes
#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{
class udf_sprintf:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	udf_sprintf();
	// ���������� ����������� ����������
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// �������� ����������
	virtual void param(std::vector<std::string> & vParams);
	// ����������
	void handler();
	// ��������� ����������
	std::string &result();
	// ����������
	~udf_sprintf();
private:
	// ���������
	std::string     sResult;
	// ���������
	std::vector<std::string> vParams;
};
} // namespace template_parser_std_fn_ns
#endif // _UDF_SPRINTF_HPP__
// End.
