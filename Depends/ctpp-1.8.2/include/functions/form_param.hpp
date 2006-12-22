/*-------------------------------------------------------------------------
 *
 * form_param.hpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Проверка на вхождение элемента в множество.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _FORM_PARAM_HPP__
#define _FORM_PARAM_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{

class form_param:
  public template_parser_ns::udf_fn
{
public:
	// Конструктор
	form_param();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(std::vector<std::string> &vIParams);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~form_param();
private:
	// Результат
	std::string                  sResult;
	// Параметр
	std::vector<std::string>     vParams;
	// Игнорировать пустые данные
	bool                         bIgnoreEmptyData;
	// Параметр 1
	std::string                  sParam1;
	// Параметр 2
	std::string                  sParam2;
	// Параметр 3
	std::string                  sParam3;

	// ESCAPE
	std::string escape_string(const std::string &sParam);
};

} // namespace template_parser_std_fn_ns
#endif // _FORM_PARAM_HPP__
// End.
