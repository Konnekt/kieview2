/*-------------------------------------------------------------------------
 *
 * istrue.hpp
 *
 *         ��������: BSD.
 *         
 *
 *         ���������� ������� ISTRUE.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _ISTRUE_HPP__
#define _ISTRUE_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

// C++ Includes
#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{

class istrue:
  public template_parser_ns::udf_fn
{
public:
	// �����������
	istrue();
	
	// ���� ��������
	template_parser_ns::udf_fn::e_accept_params accept_params();

	// ��������
	void param(const std::string &sParam);

	// ����������
	void handler();

	// ���������
	std::string &result();

	// ����������
	~istrue();
private:
	std::string     sResult;
	std::string     sParam;
};

} // namespace template_parser_std_fn_ns
#endif // _ISTRUE_HPP__
// End.
