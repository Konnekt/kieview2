/*-------------------------------------------------------------------------
 *
 * udf_fn.cpp
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Классы, необходимые для работы внешних пользовательских функций.
 *
 *
 *-------------------------------------------------------------------------
 */
#include <ctpp/udf_fn.hpp>

namespace template_parser_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс udf_fn
//
void udf_fn::param() { ;; }
void udf_fn::param(udf_fn_param & vParams) { ;; }
void udf_fn::param(const std::string &sParam) { ;; }
void udf_fn::param(const std::string &sParam1, const std::string &sParam2) { ;; }
void udf_fn::param(const std::string &sParam1, const std::string &sParam2, const std::string &sParam3) { ;; }

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Класс udf_fn_factory
//

//
// Установка пользовательской функции
//
void udf_fn_factory::install_udf_fn(const std::string &sFunctionName, udf_fn * pUDFFn)
{
	using std::pair;
	using std::string;

	m_functions_factory::iterator itmFunctionsFactory = mFunctionsFactory.find(sFunctionName);
	// Заменяем функцию
	if (itmFunctionsFactory != mFunctionsFactory.end()) { itmFunctionsFactory -> second = pUDFFn; }
	// Вставляем функцию
	else { mFunctionsFactory.insert(pair<string, udf_fn *>(sFunctionName, pUDFFn)); }
}

//
// Удаление пользовательской функции
//
void udf_fn_factory::remove_udf_fn(const std::string &sFunctionName) { mFunctionsFactory.erase(sFunctionName); }

//
// Удаление всех функций
//

void udf_fn_factory::clear()
{
	using std::pair;
	using std::string;

	m_functions_factory::iterator itmFunctionsFactory = mFunctionsFactory.begin();
	while( itmFunctionsFactory != mFunctionsFactory.end() )
	{

		if (itmFunctionsFactory -> second != NULL) { delete (itmFunctionsFactory -> second); }
                itmFunctionsFactory++;
	}
	mFunctionsFactory.clear();

	{
		m_functions_factory mTmpFactory;
		mFunctionsFactory.swap(mTmpFactory);
	}
}

//
// Получение ссылки на функцию
//
udf_fn * udf_fn_factory::get(const std::string &sFunctionName)
{
	m_functions_factory::iterator itmFunctionsFactory = mFunctionsFactory.find(sFunctionName);
	// Заменяем функцию
	if (itmFunctionsFactory != mFunctionsFactory.end()) { return itmFunctionsFactory -> second; }

	// Падаем, если ничего не найдено.
	throw std::logic_error("User defined function `" + sFunctionName + "' not exist in this factory.");
}

//
// Деструктор
//
udf_fn_factory::~udf_fn_factory()
{
	;;
}

} // namespace template_parser_ns
// End.
