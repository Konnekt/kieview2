/*-------------------------------------------------------------------------
 *
 * href_param.hpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Проверка на вхождение элемента в множество.
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/href_param.hpp>

namespace template_parser_std_fn_ns
{

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс href_param
//

//
// Конструктор
//
href_param::href_param() { ;; }

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params href_param::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// Передача параметров
//
void href_param::param(std::vector<std::string> &vParams)
{
	if (vParams.size() > 1)
	{
		sParam1.assign(vParams[0]);
		sParam2.assign(vParams[1]);

		if (vParams.size() > 2 && (!vParams[2].length() || (vParams[2].length() && (vParams[2][0] == 'n' || vParams[2][0] == 'N')))) { bIgnoreEmptyData = false; }
		else { bIgnoreEmptyData = true; }
	}
	else { sParam1.erase(); sParam2.erase(); bIgnoreEmptyData = true; }
}

//
// Вычисление
//
void href_param::handler()
{
	sResult.erase();
	if (!bIgnoreEmptyData || (bIgnoreEmptyData && sParam2.length()))
	{
		sResult.assign("&");
		sResult += escape_string(sParam1);
		sResult += '=';
		sResult += escape_string(sParam2);
	}
}

//
// ESCAPE
//
std::string href_param::escape_string(const std::string &sParam)
{
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
		{ sTmpResult += chParam; }
		else if (chParam == ' ') { sTmpResult += '+'; }
		else
		{
			if (chParam != 0)
			{
				sTmpResult += '%';
				sTmpResult += szEscape[( chParam >> 4) & 0x0F];
				sTmpResult += szEscape[ chParam & 0x0F];
			}
		}
		itsParam++;
	}
return sTmpResult;
}

//
// Результат вычислений
//
std::string &href_param::result() { return sResult; }

//
// Деструктор
//
href_param::~href_param() { ;; }

} // namespace template_parser_std_fn_ns
// End.
