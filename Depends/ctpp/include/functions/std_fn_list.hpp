/*-------------------------------------------------------------------------
 *
 * std_fn_list.hpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ������ ����������� �������
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _STD_FN_LIST_HPP__
#define _STD_FN_LIST_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#include <ctpp/udf_fn.hpp>

// ������ ���� �������
#include <functions/urlescape.hpp>
#include <functions/num_format.hpp>
#include <functions/htmlescape.hpp>
#include <functions/value_in_set.hpp>
#include <functions/href_param.hpp>
#include <functions/form_param.hpp>
#include <functions/udf_is_a.hpp>
#include <functions/istrue.hpp>

namespace template_parser_ns
{
//
// ������ ����������� ���������������� �������
//
class std_fn_list
{
public:
	// ��������� ���� ����������� �������
	void set_all(udf_fn_factory * pFactory);
	void remove_all(udf_fn_factory * pFactory);
};
} // namespace template_parser_ns
#endif // _STD_FN_LIST_HPP__
// End.
