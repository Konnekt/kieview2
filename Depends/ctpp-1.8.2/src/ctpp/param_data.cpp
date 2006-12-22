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
#include <ctpp/param_data.hpp>

namespace template_parser_ns
{

//
// �����������
//
param_data::param_data(e_value_type eIType): parent(NULL), type(eIType), undef(true)
{
	switch (type)
	{
		case VAR:
			u.str_val = new t_param_str;
//cerr << "VAR" << endl;
			break;
		case ARRAY:
			u.array_val = new t_param_array;
//cerr << "ARRAY" << endl;
			break;
		case HASH:
			u.hash_val = new t_param_hash(14);
//cerr << "HASH" << endl;
			break;
		default:
			throw std::logic_error("Unknown variable type!");
	}
}
//
// ����������
//
param_data::~param_data() throw()
{
	try
	{
		switch (type)
		{
			// ����������
			case VAR:
				{ delete u.str_val; }
//cerr << "~VAR" << endl;
				break;
			// ������
			case ARRAY:
				{
					t_param_array::iterator itArr = u.array_val -> begin();
					while (itArr != u.array_val -> end()) { delete *itArr; itArr++; }
					delete u.array_val;
				}
//cerr << "~ARRAY" << endl;
				break;
			// ���
			case HASH:
				{
					t_param_hash::iterator itArr = u.hash_val -> begin();
					while (itArr != u.hash_val -> end()) { delete itArr -> second; itArr++; }
					delete u.hash_val;
				}
//cerr << "~HASH" << endl;
				break;
			// �� ����� ���� ������
			default:
				;; // ���!
		}
	}
	catch(...) { ;; }
}

//
// ��������� �� ���������� ���� VAL
//
t_param_str * param_data::val()
{
	// ��� - ������?
	if (type == VAR) { return u.str_val; }
	// ���-���� ������
	throw std::logic_error("ValType is not VAL");
}

//
// ��������� �� ���������� ���� ARRAY
//
t_param_array * param_data::array()
{
	// ��� - ������?
	if (type == ARRAY) { return u.array_val; }
	// ���-���� ������
	throw std::logic_error("ValType is not ARRAY");
}

//
// ��������� �� ���������� ���� HASH
//
t_param_hash * param_data::hash()
{
	// ��� - ������?
	if (type == HASH) { return u.hash_val; }
	// ���-���� ������
	throw std::logic_error("ValType is not HASH");
}

//
// ��������� ����
//
param_data::e_value_type param_data::get_value_type() { return type; }

//
// ���������� ������ VALUE � HASH
//
param_data * param_data::hash_insert_new_var(const std::string &sKey, const std::string &sValue)
{
	param_data* pVar;
	if(type == HASH)
	{
		pVar = new param_data(VAR);
		pVar -> val() -> assign(sValue);
		// ���� ���������� ��� ����, ������� ������
		// ���� ���������� ��� -- ���������
		t_param_hash::iterator itmHash = hash() -> find(sKey);
		if (itmHash == hash() -> end())
		{
			hash() -> insert(CTPP::Pair<std::string, param_data *>(sKey, pVar));
		}
		else
		{
			delete itmHash -> second;
			itmHash -> second = pVar;
		}
	}
	else { throw std::logic_error("ValType is not HASH"); }
return pVar;
}

//
// ������� ������� � ���
//
param_data * param_data::hash_insert_new_array(const std::string &sKey)
{
	param_data* pArr = NULL;
	if(type == HASH)
	{
		pArr = new param_data(ARRAY);
		// ���� ���������� ��� ����, ������� ������
		// ���� ���������� ��� -- ���������
		t_param_hash::iterator itmHash = hash() -> find(sKey);
		if (itmHash == hash() -> end()) { hash() -> insert(CTPP::Pair<std::string, param_data *>(sKey, pArr)); }
		else
		{
			delete itmHash -> second;
			itmHash -> second = pArr;
		}
	}
	else { throw std::logic_error("ValType is not HASH"); }
return pArr;
}

//
// ������� ���������� � ������
//
param_data * param_data::array_insert_new_var(const std::string &sKey)
{
	param_data* pVar = NULL;
	if(type == ARRAY)
	{
		pVar = new param_data(VAR);
		pVar -> val() -> assign(sKey);
		array() -> push_back(pVar);
	}
	else { throw std::logic_error("ValType is not ARRAY"); }
return pVar;
}

//
// ������� ���� � ������
//
param_data * param_data::array_insert_new_hash()
{
	param_data* pHash = NULL;
	if(type == ARRAY)
	{
		pHash = new param_data(HASH);
		array() -> push_back(pHash);
	}
	else { throw std::logic_error("ValType is not ARRAY"); }
return pHash;
}

//
// �������� ���������� �� ����
//
void param_data::hash_erase_var(const std::string &sKey)
{
	if(type == HASH)
	{
		t_param_hash::iterator itmHash = hash() -> find(sKey);
		if (itmHash != hash() -> end())
		{
			delete itmHash -> second;
			hash() -> erase(itmHash -> first);
		}
	}
	else { throw std::logic_error("ValType is not HASH"); }
}


} // namespace template_parser_ns
// End.
