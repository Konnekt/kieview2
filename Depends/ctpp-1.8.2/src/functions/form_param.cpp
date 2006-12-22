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
#include <functions/form_param.hpp>

namespace template_parser_std_fn_ns
{

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс form_param
//

//
// Конструктор
//
form_param::form_param() { ;; }

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params form_param::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// Передача параметров
//
void form_param::param(std::vector<std::string> &vParams)
{
	if (vParams.size() > 1)
	{
		sParam1.assign(vParams[0]);
		sParam2.assign(vParams[1]);

		if (vParams.size() > 2 && (!vParams[2].length() || (vParams[2].length() && (vParams[2][0] == 'n' || vParams[2][0] == 'N')))) { bIgnoreEmptyData = false; }
		else { bIgnoreEmptyData = true; }

		if (vParams.size() > 3 && vParams[3].length()) { sParam3.assign(vParams[3]); }
		else { sParam3.assign("hidden"); }
	}
	else { sParam1.erase(); sParam2.erase(); sParam3.erase(); bIgnoreEmptyData = true; }
}

//
// Вычисление
//
void form_param::handler()
{
	sResult.erase();
	if (!bIgnoreEmptyData || (bIgnoreEmptyData && sParam2.length()))
	{
		sResult.assign("<input type=\"");
		sResult += sParam3;
		sResult += "\" name=\"";
		sResult += escape_string(sParam1);
		sResult += "\" value=\"";
		sResult += escape_string(sParam2);
		sResult += "\">";
	}
}

//
// ESCAPE
//
std::string form_param::escape_string(const std::string &sParam)
{
	std::string sResult;
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
return sResult;
}

//
// Результат вычислений
//
std::string &form_param::result() { return sResult; }

//
// Деструктор
//
form_param::~form_param() { ;; }

} // namespace template_parser_std_fn_ns
// End.
