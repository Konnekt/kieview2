/*-------------------------------------------------------------------------
 *
 * std_fn_list.сpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Список стандартных функций
 *
 *
 *-------------------------------------------------------------------------
 */
#include <functions/std_fn_list.hpp>
namespace template_parser_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс std_fn_list
//

//
// Установка всех стандартных функций
//
void std_fn_list::set_all(udf_fn_factory * pFactory)
{
	using namespace template_parser_std_fn_ns;

	udf_fn * pFunction = new urlescape();
	pFactory -> install_udf_fn("URLESCAPE",  pFunction);

	pFunction = new num_format();
	pFactory -> install_udf_fn("NUM_FORMAT", pFunction);

	pFunction = new htmlescape();
	pFactory -> install_udf_fn("HTMLESCAPE", pFunction);

	pFunction = new value_in_set();
	pFactory -> install_udf_fn("IN_SET",     pFunction);

	pFunction = new href_param();
	pFactory -> install_udf_fn("HREF_PARAM", pFunction);

	pFunction = new form_param();
	pFactory -> install_udf_fn("FORM_PARAM", pFunction);

	pFunction = new udf_is_email();
	pFactory -> install_udf_fn("ISEMAIL",    pFunction);

	pFunction = new udf_is_alpha();
	pFactory -> install_udf_fn("ISALPHA",    pFunction);

	pFunction = new udf_is_alnum();
	pFactory -> install_udf_fn("ISALNUM",    pFunction);

	pFunction = new udf_is_num();
	pFactory -> install_udf_fn("ISNUM",      pFunction);

	pFunction = new udf_is_int();
	pFactory -> install_udf_fn("ISINT",      pFunction);

	pFunction = new udf_is_float();
	pFactory -> install_udf_fn("ISFLOAT",    pFunction);

//	pFunction = new udf_sprintf();
//	pFactory -> install_udf_fn("SPRINTF",    pFunction);

//	pFunction = new udf_re();
//	pFactory -> install_udf_fn("RE",         pFunction);

	pFunction = new istrue();
	pFactory -> install_udf_fn("ISTRUE",     pFunction);
}

//
// Удаление всех стандартных функций
//
void std_fn_list::remove_all(udf_fn_factory * pFactory)
{
	using namespace template_parser_std_fn_ns;

	pFactory -> remove_udf_fn("URLESCAPE");

	pFactory -> remove_udf_fn("NUM_FORMAT");

	pFactory -> remove_udf_fn("HTMLESCAPE");

	pFactory -> remove_udf_fn("IN_SET");

	pFactory -> remove_udf_fn("HREF_PARAM");

	pFactory -> remove_udf_fn("FORM_PARAM");

	pFactory -> remove_udf_fn("ISEMAIL");

	pFactory -> remove_udf_fn("ISALPHA");

	pFactory -> remove_udf_fn("ISALNUM");

	pFactory -> remove_udf_fn("ISNUM");

	pFactory -> remove_udf_fn("ISINT");

	pFactory -> remove_udf_fn("ISFLOAT");
	
	pFactory -> remove_udf_fn("ISTRUE");
}

} // namespace template_parser_ns
// End.
