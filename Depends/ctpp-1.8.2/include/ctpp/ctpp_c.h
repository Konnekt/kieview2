/*-------------------------------------------------------------------------
 *
 * ctpp_c.h
 *
 *         Лицензия: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         С-Враппер
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef _CTPP_C_H__
#define _CTPP_C_H__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

//
// Типы
//
#define V_VAL   0
#define V_ARRAY 1
#define V_HASH  2

//
// Объект шаблонизатора
//
typedef struct
{
	void       * p_ctpp;
	const char * error;
	int          error_code;
} C_CTPP;

//
// Объект передаваемых данных
//
typedef struct
{
	void       * p_param_data;
	const char * error;
	int          error_code;
} P_DATA;

//
// Инициализация движка
//
C_CTPP * template_text_init(void);

//
// Каталоги для вложений
//
int template_text_set_include_dirs(C_CTPP * pCTPP, const char ** pszDirs);

//
// Парзинг шаблона
//
int template_text_parse(C_CTPP * pCTPP, const char * szTemplate);

//
// Вставка переменных
//
int template_text_param(C_CTPP * pCTPP, P_DATA * pData);

//
// Вывод результата
//
int template_text_output(C_CTPP * pCTPP, const char ** szOutput, long * lResultLength);

//
// Очистка шаблона
//
int template_text_clear_template(C_CTPP * pCTPP);

//
// Отключение движка
//
void template_text_destroy(C_CTPP * pCTPP);

// /////////////////////////////////////////////////////////////////////////////

//
// Инициализация движка
//
P_DATA * pd_init(int iType);

//
// Тип переменной
//
int pd_get_value_type(P_DATA * pParamData);

//
// Вставка переменной в хеш
//
P_DATA * pd_hash_insert_new_var(P_DATA * pParamData, const char * szKey, const char * szValue);

//
// Вставка массива в хеш
//
P_DATA * pd_hash_insert_new_array(P_DATA * pParamData, const char * szKey);

//
// Вставка переменной в массив
//
P_DATA * pd_array_insert_new_var(P_DATA * pParamData, const char * szKey);

//
// Вставка хеша в массив
//
P_DATA * pd_array_insert_new_hash(P_DATA * pParamData);

//
// Удаление переменной из хеша
//
int pd_hash_erase_var(P_DATA * pParamData, const char * szKey);

//
// Отключение движка
//
void pd_destroy(P_DATA * pParamData);

#if defined(__cplusplus)
}
#endif

#endif // _CTPP_C_H__
// End.
