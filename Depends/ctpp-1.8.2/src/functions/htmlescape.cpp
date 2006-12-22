/*-------------------------------------------------------------------------
 *
 * htmlescape.cpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Реализация функции NUM_FORMAT.
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/htmlescape.hpp>

namespace template_parser_std_fn_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс urlescape
//

//
// Конструктор
//
htmlescape::htmlescape()
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params htmlescape::accept_params() { return template_parser_ns::udf_fn::ONE_PARAM; }

//
// Передача параметров
//
void htmlescape::param(const std::string &sIParam) { sParam.assign(sIParam); }


//
// Вычисление
//
void htmlescape::handler()
{
	sResult.erase();
	std::string::const_iterator itsParam = sParam.begin();
	int iI = 0;
	while(itsParam != sParam.end())
	{
		switch (*itsParam)
		{
			case '"':
				sResult += "&quot;";
				break;
			case '<':
				sResult += "&lt;";
				break;
			case '>':
				sResult += "&gt;";
				break;
			case '&':
				sResult += "&amp;";
				break;
			case '\'':
				sResult += "&#039";
				break;
			default:
				sResult += *itsParam;
		}			
		itsParam++; iI++;
	}
}

//
// Результат вычислений
//
std::string &htmlescape::result() { return sResult; }

//
// Деструктор
//
htmlescape::~htmlescape() { ;; }

} // namespace template_parser_std_fn_ns
// End.
