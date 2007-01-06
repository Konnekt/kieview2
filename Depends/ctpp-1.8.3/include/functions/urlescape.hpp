/*-------------------------------------------------------------------------
 *
 * urlescape.hpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Реализация функции URLESCAPE.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _URLESCAPE_HPP__
#define _URLESCAPE_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

// C++ Includes
#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{
class urlescape:
  public template_parser_ns::udf_fn
{
public:
	// Конструктор
	urlescape();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(const std::string &sParam);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~urlescape();
private:
	// Результат
	std::string     sResult;
	// Параметр
	std::string     sParam;
};
} // namespace template_parser_std_fn_ns
#endif // _URLESCAPE_HPP__
// End.
