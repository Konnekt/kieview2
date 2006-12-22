/*-------------------------------------------------------------------------
 *
 * num_format.cpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Реализация функции NUM_FORMAT.
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/num_format.hpp>

namespace template_parser_std_fn_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс urlescape
//

//
// Конструктор
//
num_format::num_format(): iPeriodNum(3), chDelimiter(',')
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params num_format::accept_params() { return template_parser_ns::udf_fn::ONE_PARAM; }

//
// Передача параметров
//
void num_format::param(const std::string &sIParam) { sParam.assign(sIParam); }


//
// Вычисление
//
void num_format::handler()
{
	// Смещение первой запятой
	int iOffset = sParam.length() % iPeriodNum;
	if (iOffset == 0) { iOffset = iPeriodNum; }

	std::string::const_iterator itsParam = sParam.begin();
	int iI = 0;
	sResult.erase();
	while(itsParam != sParam.end())
	{
		if (iI == iOffset) { sResult += chDelimiter; iOffset += iPeriodNum; }
		sResult += *itsParam;
		itsParam++; iI++;
	}
}

//
// Результат вычислений
//
std::string &num_format::result() { return sResult; }

//
// Деструктор
//
num_format::~num_format() { ;; }

} // namespace template_parser_std_fn_ns
// End.
