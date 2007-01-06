// C++ Includes
#include <iostream>
#include <fstream>

// Local Includes
#include <ctpp/ctpp.hpp>

int main(void)
{
	using namespace template_parser_ns;
	using namespace std;

	param_data * pData = new param_data(param_data::HASH);

	pData -> hash_insert_new_var("username", "Vasya");

	t_template * pTemplate = new template_text();

        // Шаблон
        string sTemplate;
        // Загружаем шаблон
        fstream ifs;
        ifs.open("tmpl/2_1.tmpl", std::ios::in);
        if (!ifs) { cout << "Cannot open file." << endl; }
	char szBuffer[1025];
	while (!ifs.eof())
	{
		ifs.getline(szBuffer, 1024);
		sTemplate += szBuffer;
		sTemplate += "\n";
	}
	ifs.close();
	string::iterator itmData = sTemplate.begin();
	string::iterator itmDataEnd = sTemplate.end();

	try { pTemplate -> parse(itmData, itmDataEnd); }
	catch(exception &e) { cout << e.what() << endl; exit(1); }


	try { pTemplate -> param(pData, NULL); }
	catch(exception &e) { cout << e.what() << endl; exit(1); }

	string sOutput;
	try { sOutput.assign(pTemplate -> output()); }
	catch(exception &e) { cout << e.what() << endl; exit(1); }

	cout << sOutput << endl;
}