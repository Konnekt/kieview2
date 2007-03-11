/*-------------------------------------------------------------------------
 *
 * udf_fn.cpp
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ������, ����������� ��� ������ ������� ���������������� �������.
 *
 *
 *-------------------------------------------------------------------------
 */
#include <ctpp/udf_fn.hpp>

namespace template_parser_ns
{
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� udf_fn
//
void udf_fn::param() { ;; }
void udf_fn::param(udf_fn_param & vParams) { ;; }
void udf_fn::param(const std::string &sParam) { ;; }
void udf_fn::param(const std::string &sParam1, const std::string &sParam2) { ;; }
void udf_fn::param(const std::string &sParam1, const std::string &sParam2, const std::string &sParam3) { ;; }

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����� udf_fn_factory
//

//
// ��������� ���������������� �������
//
void udf_fn_factory::install_udf_fn(const std::string &sFunctionName, udf_fn * pUDFFn)
{
	using std::pair;
	using std::string;

	m_functions_factory::iterator itmFunctionsFactory = mFunctionsFactory.find(sFunctionName);
	// �������� �������
	if (itmFunctionsFactory != mFunctionsFactory.end()) { itmFunctionsFactory -> second = pUDFFn; }
	// ��������� �������
	else { mFunctionsFactory.insert(pair<string, udf_fn *>(sFunctionName, pUDFFn)); }
}

//
// �������� ���������������� �������
//
void udf_fn_factory::remove_udf_fn(const std::string &sFunctionName) { mFunctionsFactory.erase(sFunctionName); }

//
// �������� ���� �������
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
// ��������� ������ �� �������
//
udf_fn * udf_fn_factory::get(const std::string &sFunctionName)
{
	m_functions_factory::iterator itmFunctionsFactory = mFunctionsFactory.find(sFunctionName);
	// �������� �������
	if (itmFunctionsFactory != mFunctionsFactory.end()) { return itmFunctionsFactory -> second; }

	// ������, ���� ������ �� �������.
	throw std::logic_error("User defined function `" + sFunctionName + "' not exist in this factory.");
}

//
// ����������
//
udf_fn_factory::~udf_fn_factory()
{
	;;
}

} // namespace template_parser_ns
// End.
