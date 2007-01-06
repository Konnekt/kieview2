/*-------------------------------------------------------------------------
 *
 * udf_sprintf.hpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Реализация функции URLESCAPE.
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
	// Конструктор
	udf_sprintf();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	virtual void param(std::vector<std::string> & vParams);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~udf_sprintf();
private:
	// Результат
	std::string     sResult;
	// Параметры
	std::vector<std::string> vParams;
};
} // namespace template_parser_std_fn_ns
#endif // _UDF_SPRINTF_HPP__
// End.
