/*-------------------------------------------------------------------------
 *
 * udf_is_a.сpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Проверка на вхождение в множество
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/udf_is_a.hpp>

namespace template_parser_std_fn_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс udf_is_email
//

//
// Конструктор
//
udf_is_email::udf_is_email()
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params udf_is_email::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// Передача параметров
//
void udf_is_email::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

//
// Вычисление
//
void udf_is_email::handler()
{
	sResult.erase();
	bool bTypeTestPassed = true;
	std::vector<std::string>::const_iterator itvParams = vParams.begin();
	while (itvParams != vParams.end())
	{
		bool bDoggyFound = false;
		std::string::const_iterator itsValue = itvParams -> begin();
		while (itsValue != itvParams -> end())
		{
			const unsigned char ucTmp(*itsValue);

			if (ucTmp == '@')
			{
				if (bDoggyFound) { bTypeTestPassed = false; break; }
				bDoggyFound = true;
			}
			else
			{
				// Только 0-9a-zA-Z, '.', '-' и '_'
				if (!( (ucTmp >= 'a' && ucTmp <= 'z') ||
				       (ucTmp >= 'A' && ucTmp <= 'Z') ||
				       (ucTmp >= '0' && ucTmp <= '9') ||
				        ucTmp == '.' || ucTmp == '_' || ucTmp == '-')
		   		   ) { bTypeTestPassed = false; break; }
			}
			itsValue++;
		}
		if (!bTypeTestPassed) { break; }
		itvParams++;
	}

	if (bTypeTestPassed) { sResult.assign("1"); }
}

//
// Результат вычислений
//
std::string & udf_is_email::result() { return sResult; }

//
// Деструктор
//
udf_is_email::~udf_is_email() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс udf_is_alpha
//

//
// Конструктор
//
udf_is_alpha::udf_is_alpha()
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params udf_is_alpha::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// Передача параметров
//
void udf_is_alpha::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

//
// Вычисление
//
void udf_is_alpha::handler()
{
	sResult.erase();
	bool bTypeTestPassed = true;
	std::vector<std::string>::const_iterator itvParams = vParams.begin();
	while (itvParams != vParams.end())
	{
		std::string::const_iterator itsValue = itvParams -> begin();
		while (itsValue != itvParams -> end())
		{
			const unsigned char ucTmp(*itsValue);
			if (!( (ucTmp >= 'a' && ucTmp <= 'z') ||
			       (ucTmp >= 'A' && ucTmp <= 'Z'))
			   ) { bTypeTestPassed = false; break; }
			itsValue++;
		}
		if (!bTypeTestPassed) { break; }
		itvParams++;
	}

	if (bTypeTestPassed) { sResult.assign("1"); }
}

//
// Результат вычислений
//
std::string & udf_is_alpha::result() { return sResult; }

//
// Деструктор
//
udf_is_alpha::~udf_is_alpha() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс udf_is_alnum
//

//
// Конструктор
//
udf_is_alnum::udf_is_alnum()
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params udf_is_alnum::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// Передача параметров
//
void udf_is_alnum::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

//
// Вычисление
//
void udf_is_alnum::handler()
{
	sResult.erase();
	bool bTypeTestPassed = true;
	std::vector<std::string>::const_iterator itvParams = vParams.begin();
	while (itvParams != vParams.end())
	{
		std::string::const_iterator itsValue = itvParams -> begin();
		while (itsValue != itvParams -> end())
		{
			const unsigned char ucTmp(*itsValue);
			if (!( (ucTmp >= '0' && ucTmp <= '9') ||
			       (ucTmp >= 'a' && ucTmp <= 'z') ||
			       (ucTmp >= 'A' && ucTmp <= 'Z'))
			   ) { bTypeTestPassed = false; break; }
			itsValue++;
		}
		if (!bTypeTestPassed) { break; }
		itvParams++;
	}

	if (bTypeTestPassed) { sResult.assign("1"); }
}

//
// Результат вычислений
//
std::string & udf_is_alnum::result() { return sResult; }

//
// Деструктор
//
udf_is_alnum::~udf_is_alnum() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс udf_is_num
//

//
// Конструктор
//
udf_is_num::udf_is_num()
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params udf_is_num::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// Передача параметров
//
void udf_is_num::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

//
// Вычисление
//
void udf_is_num::handler()
{
	sResult.erase();
	bool bTypeTestPassed = true;
	std::vector<std::string>::const_iterator itvParams = vParams.begin();
	while (itvParams != vParams.end())
	{
		std::string::const_iterator itsValue = itvParams -> begin();
		while (itsValue != itvParams -> end())
		{
			const unsigned char ucTmp(*itsValue);
			if (!(ucTmp >= '0' && ucTmp <= '9')) { bTypeTestPassed = false; break; }
			itsValue++;
		}
		if (!bTypeTestPassed) { break; }
		itvParams++;
	}

	if (bTypeTestPassed) { sResult.assign("1"); }
}

//
// Результат вычислений
//
std::string & udf_is_num::result() { return sResult; }

//
// Деструктор
//
udf_is_num::~udf_is_num() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс udf_is_int
//

//
// Конструктор
//
udf_is_int::udf_is_int()
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params udf_is_int::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// Передача параметров
//
void udf_is_int::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

//
// Вычисление
//
void udf_is_int::handler()
{
	sResult.erase();
	bool bTypeTestPassed = true;
	std::vector<std::string>::const_iterator itvParams = vParams.begin();
	while (itvParams != vParams.end())
	{
		std::string::const_iterator itsValue = itvParams -> begin();
		if (itsValue != itvParams -> end())
		{
			unsigned char ucTmp(*itsValue);
			if ((ucTmp != '-') && (ucTmp != '+') && !(ucTmp >= '0' && ucTmp <= '9')) { bTypeTestPassed = false; break; }
			itsValue++;
			while (itsValue != itvParams -> end())
			{
				ucTmp = *itsValue;
				if (!(ucTmp >= '0' && ucTmp <= '9')) { bTypeTestPassed = false; break; }
				itsValue++;
			}
		}
		if (!bTypeTestPassed) { break; }
		itvParams++;
	}

	if (bTypeTestPassed) { sResult.assign("1"); }
}

//
// Результат вычислений
//
std::string & udf_is_int::result() { return sResult; }

//
// Деструктор
//
udf_is_int::~udf_is_int() { ;; }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс udf_is_float
//

//
// Конструктор
//
udf_is_float::udf_is_float()
{
	;;
}

//
// Количество принимаемых параметров
//
template_parser_ns::udf_fn::e_accept_params udf_is_float::accept_params() { return template_parser_ns::udf_fn::ANY_PARAMS; }

//
// Передача параметров
//
void udf_is_float::param(std::vector<std::string> &vIParams) { vParams = vIParams; }

//
// Вычисление
//
void udf_is_float::handler()
{
	sResult.erase();
	bool bTypeTestPassed = true;
	std::vector<std::string>::const_iterator itvParams = vParams.begin();
	while (itvParams != vParams.end())
	{
		std::string::const_iterator itsValue = itvParams -> begin();
		if (itsValue != itvParams -> end())
		{
			unsigned char ucTmp(*itsValue);
			if ((ucTmp != '-') && (ucTmp != '+') && !(ucTmp >= '0' && ucTmp <= '9')) { bTypeTestPassed = false; break; }
			itsValue++;
			bool bDotFound = false;
			bool bExpFound = false;
			bool bExpSignFound = false;
			while (itsValue != itvParams -> end())
			{
				ucTmp = *itsValue;
				if      (ucTmp == 'E' || ucTmp == 'e') { bExpFound = true; }
				else if (ucTmp == '.')
				{
					if (bExpFound || bDotFound) { bTypeTestPassed = false; break; }
					bDotFound = true;
				}
				else if (ucTmp == '+' || ucTmp == '-')
				{
					if (bExpFound && !bExpSignFound) { bExpSignFound = true; }
					else { bTypeTestPassed = false; break; }
				}
				else if (!(ucTmp >= '0' && ucTmp <= '9')) { bTypeTestPassed = false; break; }

				itsValue++;
			}
		}
		if (!bTypeTestPassed) { break; }
		itvParams++;
	}

	if (bTypeTestPassed) { sResult.assign("1"); }
}

//
// Результат вычислений
//
std::string & udf_is_float::result() { return sResult; }

//
// Деструктор
//
udf_is_float::~udf_is_float() { ;; }

} // namespace template_parser_std_fn_ns
// End.
