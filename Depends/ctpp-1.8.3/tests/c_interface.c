/*-------------------------------------------------------------------------
 *
 * c_interface.c
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         Пример использования С враппера
 *
 *
 *-------------------------------------------------------------------------
 */
// C++ Includes
#include <stdio.h>
#include <stdlib.h>

// Local Includes
#include <ctpp/ctpp_c.h>

// Шаблон
static const char * szTemplate = "
<TMPL_var global_param>
<TMPL_loop __CONTEXT_VARS__ __GLOBAL_VARS__ EMPLOYEE_INFO>
	<TMPL_var NAME> - <TMPL_var JOB>
	<TMPL_var global_param>
	<TMPL_if IN_SET(NAME, \"Иванов Иван\")> TEST_ANY_PARAM </TMPL_if>
</TMPL_loop>
";

//
// Проверка на наличие ошибок в шаблонизаторе
//
void check_error_CTPP(CTPP * pCTPP)
{
	if (pCTPP -> error_code != 0)
	{
		printf("Error: %s\n", pCTPP -> error);
		template_text_destroy(pCTPP);
		exit(1);
	}
}

//
// Проверка на наличие ошибок в передаваемых данных
//
void check_error_PD(P_DATA * pParamData)
{
	if (pParamData -> error_code != 0)
	{
		printf("Error: %s\n", pParamData -> error);
		pd_destroy(pParamData);
		exit(1);
	}
}

int main(void)
{
	CTPP   * pCTPP;
	P_DATA * pParamData;
	P_DATA * pArray;
	P_DATA * pHash;

	const char   * szOutput;
	long           lResultLength;

	// Создаем структуру данных
	pParamData = pd_init(V_HASH);
	// Проверяем на предмет ошибки
	check_error_PD(pParamData);
	// Вставляем глобальный параметр
	pd_hash_insert_new_var(pParamData, "global_param", "Global Parameter");

	// Вставляем цикл
	pArray = pd_hash_insert_new_array(pParamData, "EMPLOYEE_INFO");

	// Внутри цикла - хеш с параметрами
	pHash = pd_array_insert_new_hash(pArray);
	// Параметр "NAME"
	pd_hash_insert_new_var(pHash, "NAME", "Иванов Иван");
	// Параметр "JOB"
	pd_hash_insert_new_var(pHash, "JOB", "Архитектор");

	pHash = pd_array_insert_new_hash(pArray);
	pd_hash_insert_new_var(pHash, "NAME", "Петров Петр");
	pd_hash_insert_new_var(pHash, "JOB", "Строитель");

	// Подменяем глобальный параметр
	pd_hash_insert_new_var(pHash, "global_param", "Replaced Parameter");

	// Шаблонизатор
	pCTPP = template_text_init();

	// Парзим шаблон
	template_text_parse(pCTPP, szTemplate);
	check_error_CTPP(pCTPP);

	// Накладываем параметры на шаблон
	template_text_param(pCTPP, pParamData);
	check_error_CTPP(pCTPP);

	// Получаем результат
	template_text_output(pCTPP, &szOutput, &lResultLength);
	check_error_CTPP(pCTPP);

	// Выводим
	printf("%s\n", szOutput);

	// Уничтожение движка
	template_text_destroy(pCTPP);
return 0;
}
