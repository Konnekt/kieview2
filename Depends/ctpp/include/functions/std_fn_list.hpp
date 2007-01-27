/*-------------------------------------------------------------------------
 *
 * std_fn_list.hpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Список стандартных функций
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

// Список всех функций
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
// Список стандартных пользовательских функций
//
class std_fn_list
{
public:
	// Установка всех стандартных функций
	void set_all(udf_fn_factory * pFactory);
	void remove_all(udf_fn_factory * pFactory);
};
} // namespace template_parser_ns
#endif // _STD_FN_LIST_HPP__
// End.
