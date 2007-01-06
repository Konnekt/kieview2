/*-------------------------------------------------------------------------
 *
 * value_in_set.сpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Проверка на вхождение в множество
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/value_in_set.hpp>

namespace template_parser_std_fn_ns
{

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс value_in_set

//
// Конструктор
//
value_in_set::value_in_set()
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params value_in_set::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// Передача параметров
//
void value_in_set::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

void value_in_set::handler()
{
	sResult.erase();
	if (vParams.size() > 1)
	{
		std::vector<std::string>::iterator itvParams = vParams.begin();
		itvParams++;
		while (itvParams != vParams.end())
		{
			if (*itvParams == vParams[0]) { sResult.assign("1"); break; }
			itvParams++;
		}
	}
}

//
// Результат вычислений
//
std::string & value_in_set::result() { return sResult; }

//
// Деструктор
//
value_in_set::~value_in_set() { ;; }

} // namespace template_parser_std_fn_ns
// End.
