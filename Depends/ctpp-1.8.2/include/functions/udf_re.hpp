/*-------------------------------------------------------------------------
 *
 * udf_re.hpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Функции проверки типов.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _UDF_RE_HPP__
#define _UDF_RE_HPP__ 1

// C Includes
//#include <pcre.h>

// C++ Includes
#include <string>
#include <map>

// Local Includes
#include <ctpp/udf_fn.hpp>

namespace template_parser_std_fn_ns
{

//
// Проверка на email
//
class udf_re:
  public template_parser_ns::udf_fn
{
public:
	// Конструктор
	udf_re();
	// Количество принимаемых параметров
	template_parser_ns::udf_fn::e_accept_params accept_params();
	// Передача параметров
	void param(std::vector<std::string> &vIParams);
	// Вычисление
	void handler();
	// Результат вычислений
	std::string &result();
	// Деструктор
	~udf_re();
private:
//	typedef std::map<std::string, pcre *> t_m_pcre;
	// Результат
	std::string                sResult;
	// Параметр
	std::vector<std::string>   vParams;

//	// Пул регулярных выражений
//	t_m_pcre                   mRegExpPool;
//	// Индекс для поиска LRU шаблонов
//	std::map<int, std::string> mLRU;
};

} // namespace template_parser_std_fn_ns
#endif // _UDF_RE_HPP__
// End.
