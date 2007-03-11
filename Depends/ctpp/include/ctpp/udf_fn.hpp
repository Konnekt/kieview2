/*-------------------------------------------------------------------------
 *
 * udf_fn.hpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Классы, необходимые для работы внешних пользовательских функций.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _UDF_FN_HPP__
#define _UDF_FN_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

// C Includes

// C++ Includes
#include <map>
#include <vector>
#include <string>
#include <stdexcept>

// Local Includes

namespace template_parser_ns
{
//
// Параметры, передаваемые функции
//
typedef std::vector<std::string> udf_fn_param;

//
// Функция, определяемая пользоваталем
//
class udf_fn
{
public:
	// Количество принимаемых параметров
	enum e_accept_params {NO_PARAMS, ONE_PARAM, TWO_PARAMS, THREE_PARAMS, ANY_PARAMS};

	// Передача параметров
	virtual void param();
	virtual void param(udf_fn_param & vParams);
	virtual void param(const std::string &sParam);
	virtual void param(const std::string &sParam1, const std::string &sParam2);
	virtual void param(const std::string &sParam1, const std::string &sParam2, const std::string &sParam3);

	// Количество принимаемых параметров
	virtual e_accept_params accept_params() = 0;

	// Вычисление
	virtual void handler() = 0;

	// Результат вычислений
	virtual std::string & result() = 0;

	// Деструктор
	virtual ~udf_fn() { ;; }
};

//
// Инсталлятор функций, определенных пользователем
//
class udf_fn_factory
{
public:
	// Установка пользовательской функции
	void install_udf_fn(const std::string &sFunctionName, udf_fn * pUDFFn);
	// Удаление пользовательской функции
	void remove_udf_fn(const std::string &sFunctionName);
	// Получение ссылки на функцию
	udf_fn * get(const std::string &sFunctionName);
	// Удаление всех функций
	void clear();
	// Деструктор
	virtual ~udf_fn_factory();
private:

	// Тип factory
	typedef std::map<std::string, udf_fn *>      m_functions_factory;
	// Factory
	m_functions_factory                          mFunctionsFactory;
};

} // namespace template_parser_ns
#endif // _UDF_FN_HPP__
// End.
