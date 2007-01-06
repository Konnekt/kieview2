/*-------------------------------------------------------------------------
 *
 * is_a.hpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Функции проверки типов.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _IS_A_HPP__
#define _IS_A_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#include <string>
// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{

//
// Проверка на email
//
class udf_is_email:
  public template_parser_ns::udf_fn
{
public:
	// Конструктор
	udf_is_email();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(std::vector<std::string> &vIParams);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~udf_is_email();
private:
	// Результат
	std::string                  sResult;
	// Параметр
	std::vector<std::string>     vParams;
};

//
// Проверка на наличие только латинских букв
//
class udf_is_alpha:
  public template_parser_ns::udf_fn
{
public:
	// Конструктор
	udf_is_alpha();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(std::vector<std::string> &vIParams);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~udf_is_alpha();
private:
	// Результат
	std::string                  sResult;
	// Параметр
	std::vector<std::string>     vParams;
};

//
// Проверка на наличие только латинских букв
//
class udf_is_alnum:
  public template_parser_ns::udf_fn
{
public:
	// Конструктор
	udf_is_alnum();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(std::vector<std::string> &vIParams);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~udf_is_alnum();
private:
	// Результат
	std::string                  sResult;
	// Параметр
	std::vector<std::string>     vParams;
};

//
// Проверка на наличие только латинских букв
//
class udf_is_num:
  public template_parser_ns::udf_fn
{
public:
	// Конструктор
	udf_is_num();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(std::vector<std::string> &vIParams);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~udf_is_num();
private:
	// Результат
	std::string                  sResult;
	// Параметр
	std::vector<std::string>     vParams;
};

//
// Проверка на наличие только латинских букв и цифр
//
class udf_is_int:
  public template_parser_ns::udf_fn
{
public:
	// Конструктор
	udf_is_int();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(std::vector<std::string> &vIParams);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~udf_is_int();
private:
	// Результат
	std::string                  sResult;
	// Параметр
	std::vector<std::string>     vParams;
};

//
// Проверка на наличие только латинских букв и цифр
//
class udf_is_float:
  public template_parser_ns::udf_fn
{
public:
	// Конструктор
	udf_is_float();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(std::vector<std::string> &vIParams);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~udf_is_float();
private:
	// Результат
	std::string                  sResult;
	// Параметр
	std::vector<std::string>     vParams;
};

} // namespace template_parser_std_fn_ns
#endif // _IS_A_HPP__
// End.
