/*-------------------------------------------------------------------------
 *
 * value_in_set.hpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Проверка на вхождение элемента в множество.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _VALUE_IN_SET_HPP__
#define _VALUE_IN_SET_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{

class value_in_set:
  public template_parser_ns::udf_fn
{
public:
	// Конструктор
	value_in_set();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(std::vector<std::string> &vIParams);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~value_in_set();
private:
	// Результат
	std::string                  sResult;
	// Параметр
	std::vector<std::string>     vParams;
};

} // namespace template_parser_std_fn_ns
#endif // _VALUE_IN_SET_HPP__
// End.
