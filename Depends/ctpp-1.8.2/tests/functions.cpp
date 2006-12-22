// C++ Includes
#include <iostream>

// Local Includes
#include <ctpp/ctpp.hpp>

int main(void)
{
	using namespace template_parser_ns;
	using namespace std;

	param_data * pData = new param_data(param_data::HASH);

	pData -> hash_insert_new_var("EMAIL", "abc@mail.ru");
	pData -> hash_insert_new_var("ALPHA", "abcdefg");
	pData -> hash_insert_new_var("NUM",   "123");
	pData -> hash_insert_new_var("ALNUM", "123abc456xyz");

	pData -> hash_insert_new_var("INT_PLUS", "+123456");
	pData -> hash_insert_new_var("INT", "123456");

	pData -> hash_insert_new_var("FLOAT", "123456.57");

	pData -> hash_insert_new_var("FLOAT_EXP", "-123456.57E+33");
	pData -> hash_insert_new_var("FLOAT_EXP_1", "-0.9277E33");
	pData -> hash_insert_new_var("FLOAT_EXP_2", "0.9277E33.3");

	// Шаблон
	string sTemplate("
<TMPL_if ISEMAIL(EMAIL)>ISEMAIL#1 OK</TMPL_if>
<TMPL_if ISEMAIL(ALPHA)>ISEMAIL#2 OK</TMPL_if>
<TMPL_if ISEMAIL(NUM)>ISEMAIL#3 OK</TMPL_if>
<TMPL_if ISEMAIL(FLOAT)>ISEMAIL#4 OK</TMPL_if>
<TMPL_unless ISEMAIL(FLOAT_EXP)>ISEMAIL#5 OK</TMPL_unless>

<TMPL_if ISALPHA(ALPHA)>ISALPHA#1 OK</TMPL_if>
<TMPL_unless ISALPHA(NUM)>ISALPHA#2 OK</TMPL_unless>

<TMPL_if ISNUM(NUM)>ISNUM#1   OK</TMPL_if>
<TMPL_unless ISNUM(INT_PLUS)>ISNUM#2   OK</TMPL_unless>

<TMPL_if ISALNUM(ALPHA)>ISALNUM#1 OK</TMPL_if>
<TMPL_if ISALNUM(NUM)>ISALNUM#2 OK</TMPL_if>
<TMPL_if ISALNUM(ALNUM)>ISALNUM#3 OK</TMPL_if>
<TMPL_unless ISALNUM(EMAIL)>ISALNUM#4 OK</TMPL_unless>

<TMPL_if ISINT(INT)>ISINT#1   OK</TMPL_if>
<TMPL_if ISINT(INT_PLUS)>ISINT#2   OK</TMPL_if>
<TMPL_unless ISINT(FLOAT)>ISINT#3   OK</TMPL_unless>

<TMPL_if ISFLOAT(FLOAT)>ISFLOAT#1 OK</TMPL_if>
<TMPL_if ISFLOAT(FLOAT_EXP)>ISFLOAT#2 OK</TMPL_if>
<TMPL_if ISFLOAT(INT)>ISFLOAT#3 OK</TMPL_if>
<TMPL_if ISFLOAT(INT_PLUS)>ISFLOAT#4 OK</TMPL_if>
<TMPL_unless ISFLOAT(EMAIL)>ISFLOAT#5 OK</TMPL_unless>
ISFLOAT#6 <TMPL_if ISFLOAT(EMAIL)>ERR<TMPL_else>OK</TMPL_if>
ISFLOAT#7 <TMPL_unless ISFLOAT(FLOAT_EXP_1)>ERR<TMPL_else>OK</TMPL_unless>
ISFLOAT#8 <TMPL_if ISFLOAT(FLOAT_EXP_2)>ERR<TMPL_else>OK</TMPL_if>
	");

	try
	{
		template_text pTemplate;
		pTemplate.parse(sTemplate);
		pTemplate.param(pData);
		cout << pTemplate.output() << endl;;
		delete pData;
	}
	catch(exception &e) { cout << e.what() << endl; exit(1); }
}
