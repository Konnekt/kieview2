/*
 * Copyright (c) 2004 - 2006 CTPP Dev Team
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the CTPP Team nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      param_data.hpp
 *
 * $CTPP$
 */
#ifndef _CTPP_HPP__
#define _CTPP_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif
// C Includes
// None, because STL is a MUST

// C++ Includes
#include <cstdio>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// Local Includes
#include <util/d2str.hpp>
#include <ctpp/param_data.hpp>
#include <ctpp/udf_fn.hpp>

namespace template_parser_ns
{
// Тип токена
enum e_token_type{TMPL_TEXT, TMPL_VAR, TMPL_IF, TMPL_ELSE, TMPL_UNLESS, TMPL_LOOP, TMPL_UDF, TMPL_INCLUDE, TMPL_DECLARE, TMPL_BREAK, TMPL_COMMENT};

// Максимальная длина токена
static const unsigned int C_TEMPLATE_MAX_TOKEN_LEN = 128;

class t_template;

//
// Каталоги для INCLUDE
//
typedef std::vector<std::string> v_include_dir;

//
// Параметры функции
//
struct function_param_data
{
	std::string             param;
	bool                    is_variable;
	function_param_data(const std::string &sParam = "", const bool bIsVariable = false);
};

//
// Стек
//
struct stack_ref
{
	// Название элемента
	std::string                      elem_name;
	// Шаблон для элмента
	t_template                     * template_elem;
	// Функция
	udf_fn                         * function;
	// Список передаваемых функции параметров.
	std::vector<function_param_data> function_parameters;
};
typedef std::vector<stack_ref> v_stack;

//
// Возвращаемое значение
//
struct template_ret_type
{
	// Тип возвращаемого токена
	e_token_type                token_type;
	// Позиция парзинга
	std::string::const_iterator parse_pos;
	// Строка парзинга
	int                         line;
	// Столбец парзинга
	int                         column;
};

//
// Класс для загрузки вложений
//
class loader_base
{
public:
	// Загрузка файла
	virtual void load_file(const std::string &sFileName);

	// Получение данных
	virtual std::string & get_data();

	// Деструктор
	virtual ~loader_base() throw();

protected:
	std::string sData;
};

//
// Базовый класс шаблонизатора
//
class t_template
{
public:
	// Получение типа
	virtual e_token_type get_type() = 0;

	// Парзинг при инициализации
	virtual template_ret_type parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// Вставка данных
	virtual bool param(param_data * pParamData, param_data * pIRootParamData) = 0;

	// Вывод данных
	virtual std::string & output(bool & bBreak) = 0;

	// Деструктор
	virtual ~t_template() throw() { ;; }

protected:

	// Выполнение пользовательской функции
	std::string execute_udf_fn(stack_ref &sStackRef, param_data * pParamData, param_data * pRootParamData, bool bGlobalVars);
};

//
// Текстовый блок
//
class template_text:
  public t_template
{
public:
	// Конструктор
	template_text(template_parser_ns::udf_fn_factory * pIFactory = NULL, const int iILine = 1, const int iIPos = 1, const int iITabLength = 9, const bool bIDebug = false, const bool bIStrict = true, const bool bILoopContextVars = false, const bool bIGlobalVars = false);

	// Получение Factory
	udf_fn_factory * get_fn_factory();

	// Установка include_dir
	void set_include_dir(const v_include_dir & vIIncludeDir);

	// Объект-загрузчик вложений
	void set_file_opener(loader_base * pILoaderBase);

	// Парзинг при инициализации
	void parse(const std::string &sTemplate);

	// Парзинг при инициализации
	void parse(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// Вставка данных
	bool param(param_data * pParamData);

	// Вывод данных
	std::string & output(bool & bBreak);

	// Вывод данных
	inline std::string & output()
	{
		bool bBreak = false;
		return output(bBreak);
	}

	// Очистка шаблона
	void clear_template();

	// Деструктор
	~template_text() throw();
protected:

	// Копирующий конструктор
	template_text(const template_text & oTemplateText);

	// Получение типа
	e_token_type get_type();

	// Парзинг при инициализации
	virtual template_ret_type parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// Вставка данных
	bool param(param_data * pIParamData, param_data * pIRootParamData);

	// Каталоги для include
	v_include_dir    vIncludeDir;

	// Стек данных
	v_stack          vStack;
	// часть данных
	std::string      sTextData;
	// Параметр
	param_data     * pParamData;
	// Корневой параметр
	param_data     * pRootParamData;

	// Функции
	udf_fn_factory * pFactory;
	// Номер строки
	int              iLine;
	// Номер столбца
	int              iPos;
	// Длина пробела
	int              iTabLength;
	// Дебаг
	bool             bDebug;
	// Строгая синтаксическая проверка шаблона
	bool             bStrict;
	// Контекстные переменные
	bool             bLoopContextVars;
	// Глобальные переменные
	bool             bGlobalVars;
	// Factory была инициализирована внутри класса
	bool             bSelfCreatedFactory;
	// Загрузчик был инициализирован внутри класса
	bool             bSelfCreatedLoader;
	// Класс для чтения вложений
	loader_base    * pLoaderBase;

	// Действия, необходимые для отката внутри токена
	void             do_rollback_token(std::string::const_iterator itmData, std::string::const_iterator itmRollBackPos);
	// Фатальная ошибка парзинга
	void             fatal_parsing_error(const e_token_type &eExpectedToken, const e_token_type &eFoundToken);
	// enum -> Human Readable
	std::string      token2str(const e_token_type &eToken);
	// Вставка текстового блока
	void insert_text_block(std::string & sTextSection);
	// Парзинг параметров функции
	void parse_param_string(unsigned int &iPosition, const e_token_type &eFoundToken, std::string::const_iterator & itsData, std::string::const_iterator itsDataEnd, std::string::const_iterator itmRollBackPos);
	// Проверка пути файла
	void check_include_file(const std::string & sParam, std::string &sIncludeFile);
};


//
// Переменная
//
class template_var:
  public t_template
{
public:
	// Конструктор
	template_var(udf_fn * pIFunction = NULL);

	// Деструктор
	~template_var() throw();
private:
	// Копирующий конструктор
	template_var(const template_var &oTemplateVar);

	// Получение типа
	e_token_type get_type();

	// Вставка данных
	bool param(param_data * pParamData, param_data * pIRootParamData);

	// Вывод данных
	std::string & output(bool & bBreak);

	std::string   sValue;
	std::string   sTmpValue;
	// Пользовательская функция
	udf_fn      * pFunction;
};

//
// Выполнение условия
//
class template_if:
  public t_template
{
public:
	// Конструктор
	template_if(template_parser_ns::udf_fn_factory * pIFactory, const int iILine, const int iIPos, const int iITabLength, const bool bIDebug, const bool bIStrict, const bool bILoopContextVars, const bool bIGlobalVars, const bool bIIsIf, const v_include_dir &vIIncludeDir, loader_base * pILoaderBase);

	// Деструктор
	~template_if() throw();

private:
	friend class template_text;

	template_if(const template_if &oTemplateIf);

	// Получение типа
	e_token_type get_type();

	// Парзинг при инициализации
	template_ret_type parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// Вставка данных
	bool param(param_data * pIParamData, param_data * pIRootParamData);

	// Передача дополнительного пареметра
	void additional_param(const bool &bIFlag);

	// Вывод данных
	std::string & output(bool & bBreak);

	// Стек данных прямого условия
	t_template     * pTemplateIfText;
	// Стек данных обратного условия
	t_template     * pTemplateElseText;

	// Параметр
	param_data     * pParamData;
	// Корневой параметр
	param_data     * pRootParamData;
	// Дополнительный параметр
	bool             bFlag;

	// Результат парзинга
	std::string      sTextData;

	// Функции
	udf_fn_factory * pFactory;
	// Номер строки
	int              iLine;
	// Номер столбца
	int              iPos;
	// Длина пробела
	int              iTabLength;
	// Дебаг
	bool             bDebug;
	// Строгая синтаксическая проверка шаблона
	bool             bStrict;
	// Контекстные переменные
	bool             bLoopContextVars;
	// Глобальные переменные
	bool             bGlobalVars;
	// Обрабатывается ветка IF
	bool             bIsIf;
	// Каталоги для include
	const v_include_dir &vIncludeDir;
	// Класс для чтения вложений
	loader_base       * pLoaderBase;
};

//
// Цикл
//
class template_loop:
  public t_template
{
public:
	// Конструктор
	template_loop(template_parser_ns::udf_fn_factory * pIFactory, const int iILine, const int iIPos, const int iITabLength, const bool bIDebug, const bool bIStrict, const bool bILoopContextVars, const bool bIGlobalVars,	const v_include_dir &vIIncludeDir, loader_base * pILoaderBase);

	// Деструктор
	~template_loop() throw();

private:
	// Копирующий конструктор
	template_loop(const template_loop &oTemplateLoop);

	// Получение типа
	e_token_type get_type();

	// Вставка данных
	bool param(param_data * pIParamData, param_data * pIRootParamData);

	// Парзинг при инициализации
	template_ret_type parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// Вывод данных
	std::string & output(bool & bBreak);

	// Стек данных
	t_template     * pTemplateText;

	// Параметр
	param_data     * pParamData;
	// Корневой параметр
	param_data     * pRootParamData;

	// Результат парзинга
	std::string     sTextData;

	// Функции
	udf_fn_factory * pFactory;
	// Номер строки
	int              iLine;
	// Номер столбца
	int              iPos;
	// Длина пробела
	int              iTabLength;
	// Дебаг
	bool             bDebug;
	// Строгая синтаксическая проверка шаблона
	bool             bStrict;
	// Контекстные переменные
	bool             bLoopContextVars;
	// Глобальные переменные
	bool             bGlobalVars;
	// Каталоги для include
	const v_include_dir &vIncludeDir;
	// Класс для чтения вложений
	loader_base       * pLoaderBase;
};

//
// Пользовательская функция
//
class template_udf:
  public t_template
{
public:
	// Конструктор
	template_udf(const stack_ref &sIStackRef, bool bIGlobalVars);

	// Деструктор
	~template_udf() throw();

private:
	// Копирующий конструктор
	template_udf(const template_udf & oTemplateUDF);

	// Получение типа
	e_token_type get_type();

	// Вставка данных
	bool param(param_data * pIParamData, param_data * pIRootParamData);

	// Вывод данных
	std::string & output(bool & bBreak);

	// Пользовательская функция
	stack_ref     sStackRef;
	// Использование глобальных переменных
	bool          bGlobalVars;

	param_data  * pParamData;
	param_data  * pRootParamData;

	std::string   sValue;
};

//
// Вложение
//
class template_include:
  public t_template
{
public:
	// Конструктор
	template_include(udf_fn_factory * pIFactory, const int iITabLength, const bool bIDebug, const bool bIStrict, const bool bILoopContextVars, const bool bIGlobalVars, const v_include_dir &vIIncludeDir, loader_base * pILoaderBase);

	// Деструктор
	~template_include() throw();
private:
	// Вывод данных
	std::string & output(bool & bBreak);

	// Копирующий конструктор
	template_include(const template_include & oTemplateInclude);

	// Парзинг при инициализации
	template_ret_type parse_block(std::string::const_iterator itmData, std::string::const_iterator itmDataEnd);

	// Получение типа
	e_token_type get_type();

	// Вставка данных
	bool param(param_data * pIParamData, param_data * pIRootParamData);

	// Шаблонизатор
	t_template         * pTemplateText;
};


//
// Переменная
//
class template_break:
  public t_template
{
public:
	// Конструктор
	template_break(const stack_ref &sIStackRef, bool bIGlobalVars);

	// Деструктор
	~template_break() throw();
private:
	// Копирующий конструктор
	template_break(const template_break &oTemplateVar);

	// Получение типа
	e_token_type get_type();

	// Вставка данных
	bool param(param_data * pParamData, param_data * pIRootParamData);

	// Вывод данных
	std::string & output(bool & bBreak);

	// Пользовательская функция
	stack_ref         sStackRef;
	bool              bGlobalVars;
	bool              bBreakFlag;

	std::string sTMP;
};

//
// Объявление переменной
//
class template_declare:
  public t_template
{
public:
	// Конструктор
	template_declare(const stack_ref &sIStackRef);

	// Деструктор
	~template_declare() throw();
private:
	// Копирующий конструктор
	template_declare(const template_declare &oTemplate);

	// Получение типа
	e_token_type get_type();

	// Вставка данных
	bool param(param_data * pParamData, param_data * pIRootParamData);

	// Вывод данных
	std::string & output(bool & bBreak);

//	param_data  * pParamData;

	function_param_data oParamName;
	function_param_data oParamValue;

	std::string sTMP;
};

} // namespace template_parser_ns
#endif // _CTPP_HPP__
// End.
