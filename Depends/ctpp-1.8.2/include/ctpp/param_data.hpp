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
#ifndef _PARAM_DATA_HPP__
#define _PARAM_DATA_HPP__ 1

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

// C Includes

// C++ Includes
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

// Local Includes
#include <ctpp/Hash.hpp>
#include <ctpp/Types.h>


namespace template_parser_ns
{

class param_data;
// Строка
typedef std::string                         t_param_str;
// Массив
typedef std::vector<param_data *>           t_param_array;


//
// Hasher
//
struct Hasher
{
	// Operator ()
	inline TokenHash_t operator ()(const std::string & sStr) const
	{
		TokenHash_t        ulHash = 5381;
		const char       * sKey = sStr.c_str();
		const char       * sEnd = sKey + sStr.size();

		while (sKey != sEnd)
		{
			ulHash += (ulHash << 5);
			ulHash ^= (TokenHash_t) *sKey++;
		}
	return ulHash;
	}
};

//
// Comparator
//
struct Comparator
{
	// Operator ()
	inline bool operator ()(const std::string & arg1, const std::string & arg2) const { return arg1 == arg2; }

};

//
// Хеш
//
typedef CTPP::Hash<std::string, param_data *, Hasher, Comparator> t_param_hash;

//
// Hash Pair
//
typedef CTPP::Pair<std::string, param_data *>                  HashPair;

//
// Передаваемые параметры
//
class param_data
{
public:
	enum e_value_type { VAR = 10, ARRAY, HASH };

	// Конструктор
	param_data(e_value_type eIType = VAR);

	// Указатель на переменную типа VAL
	t_param_str   *      val();
	// Указатель на переменную типа ARRAY
	t_param_array *      array();
	// Указатель на переменную типа HASH
	t_param_hash  *      hash();

	// Тип переменной
	e_value_type get_value_type();

	// Вставка переменной в хеш
	param_data * hash_insert_new_var(const std::string &sKey, const std::string &sValue);

	// Вставка массива в хеш
	param_data * hash_insert_new_array(const std::string &sKey);

	// Вставка переменной в массив
	param_data * array_insert_new_var(const std::string &sKey);

	// Вставка хеша в массив
	param_data * array_insert_new_hash();

	// Удаление переменной из хеша
	void hash_erase_var(const std::string &sKey);

	// Деструктор
	~param_data() throw();

private:
	// Родитель
	param_data                 * parent;
	// Тип значение
	e_value_type                 type;
	// Значение

	union
	{
		// Указатель на переменную типа VAL
		t_param_str   *      str_val;
		// Указатель на переменную типа ARRAY
		t_param_array *      array_val;
		// Указатель на переменную типа HASH
		t_param_hash  *      hash_val;
	}
	u;
	// Undef
	bool                         undef;
};
} // namespace template_parser_ns
#endif // _PARAM_DATA_HPP__
// End.
