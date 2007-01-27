/*-------------------------------------------------------------------------
 *
 * urlescape.cpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Реализация функции URLESCAPE.
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/urlescape.hpp>

namespace template_parser_std_fn_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс urlescape
//

//
// Конструктор
//
urlescape::urlescape()
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params urlescape::accept_params() { return template_parser_ns::udf_fn::ONE_PARAM; }

//
// Передача параметров
//
void urlescape::param(const std::string &sIParam) { sParam.assign(sIParam); }


//
// Вычисление
//
void urlescape::handler()
{
	sResult.erase();
	static const char           * szEscape = "0123456789ABCDEF";
	std::string::const_iterator   itsParam = sParam.begin();
	std::string sTmpResult;
	while (itsParam != sParam.end())
	{
		char chParam = *itsParam;
		if (( chParam >= 'a' && chParam <= 'z') ||
		   (  chParam >= 'A' && chParam <= 'Z') ||
		   (  chParam >= '0' && chParam <= '9') ||
		      chParam == '-' || chParam == '_'  || chParam == '.'
		)
		{ sResult += chParam; }
		else if (chParam == ' ') { sResult += '+'; }
		else
		{
			if (chParam != 0)
			{
				sResult += '%';
				sResult += szEscape[( chParam >> 4) & 0x0F];
				sResult += szEscape[ chParam & 0x0F];
			}
		}
		itsParam++;
	}
}

//
// Результат вычислений
//
std::string &urlescape::result() { return sResult; }

//
// Деструктор
//
urlescape::~urlescape() { ;; }

} // namespace template_parser_std_fn_ns
// End.
