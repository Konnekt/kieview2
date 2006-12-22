/*-------------------------------------------------------------------------
 *
 * num_format.hpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Реализация функции NUM_FORMAT.
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
	// Конструктор
	num_format();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(const std::string &sParam);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~num_format();
private:
	// Результат
	int             iPeriodNum;
	char            chDelimiter;

	// Результат
	std::string     sResult;
	// Параметр
	std::string     sParam;

};
} // namespace template_parser_std_fn_ns
#endif // _NUM_FORMAT_HPP__
// End.
