#ifdef WIN32
#pragma warning (disable : 4786)
#endif

// C++ Includes
#include <iostream>
#include <stdio.h>

// Local Includes
#include <ctpp/ctpp.hpp>
int main(void)
{
	using namespace template_parser_ns;
	using namespace std;

	// Создаем структуру данных
	param_data * pData = new param_data(param_data::HASH);
/*
	for (int iI = 0; iI < 42; iI++)
	{
		char szKey[120];
		char szVal[120];
		snprintf(szKey, 119, "key_%d.", iI);
		snprintf(szVal, 119, "val_%d.", iI);
		fprintf(stderr, "'%s' => '%s'\n", szKey, szVal);
		pData -> hash_insert_new_var(szKey, szVal);
	}

	t_param_hash * pH = pData -> hash();
	t_param_hash::iterator i = pH -> begin();
	while(i != pH -> end())
	{
		fprintf(stderr, "'%s' => '-'\n", i -> first.c_str());
		++i;
	}
*/

	// Вставляем глобальный параметр
	pData -> hash_insert_new_var("global_param", "Global Parameter");

	// Вставляем цикл
	param_data * pArray = pData -> hash_insert_new_array("EMPLOYEE_INFO");

	// Внутри цикла - хеш с параметрами
	param_data * pHash = pArray -> array_insert_new_hash();
	// Параметр "NAME"
	pHash -> hash_insert_new_var("NAME", "Иванов Иван");
	// Параметр "JOB"
	pHash -> hash_insert_new_var("JOB", "Архитектор");

	pHash = pArray -> array_insert_new_hash();
	pHash -> hash_insert_new_var("NAME", "Петров Петр");
	pHash -> hash_insert_new_var("JOB", "Строитель");
	// Подменяем глобальный параметр
	pHash -> hash_insert_new_var("global_param", "Replaced Parameter");

//	try
//	{
		// Шаблонизатор
		template_text oTemplate;

		// Загрузчик вложений
		loader_base * pLoaderBase = new loader_base();

		// Каталог для вложений
		v_include_dir vIncludeDir;
		vIncludeDir.push_back("./tmpl");
		oTemplate.set_include_dir(vIncludeDir);
		// Текст шаблона
		pLoaderBase -> load_file("./tmpl/3_1.tmpl");
		// Парзим шаблон
		oTemplate.parse(pLoaderBase -> get_data());
		// Накладываем параметры на шаблон
		oTemplate.param(pData);
		string sOutput;
		// Получаем результат
		bool bDummy = true;;
		sOutput.assign(oTemplate.output(bDummy));
		cout << sOutput << endl;
//	}
//	catch(exception &e) { cout << e.what() << endl; exit(1); }

return 0;
}
