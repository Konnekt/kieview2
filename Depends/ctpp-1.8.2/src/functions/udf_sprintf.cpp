/*-------------------------------------------------------------------------
 *
 * udf_sprintf.сpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Проверка на вхождение в множество
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/udf_sprintf.hpp>

#include <stdio.h>

namespace template_parser_std_fn_ns
{

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс udf_sprintf

//
// Конструктор
//
udf_sprintf::udf_sprintf()
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params udf_sprintf::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// Передача параметров
//
void udf_sprintf::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

void udf_sprintf::handler()
{
	sResult.erase();
}

//
// Результат вычислений
//
std::string & udf_sprintf::result() { return sResult; }

//
// Деструктор
//
udf_sprintf::~udf_sprintf() { ;; }

} // namespace template_parser_std_fn_ns
// End.
