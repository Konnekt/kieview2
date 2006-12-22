/*-------------------------------------------------------------------------
 *
 * c_interface.c
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         ������ ������������� � ��������
 *
 *
 *-------------------------------------------------------------------------
 */
// C++ Includes
#include <stdio.h>
#include <stdlib.h>

// Local Includes
#include <ctpp/ctpp_c.h>

// ������
static const char * szTemplate = "
<TMPL_var global_param>
<TMPL_loop __CONTEXT_VARS__ __GLOBAL_VARS__ EMPLOYEE_INFO>
	<TMPL_var NAME> - <TMPL_var JOB>
	<TMPL_var global_param>
	<TMPL_if IN_SET(NAME, \"������ ����\")> TEST_ANY_PARAM </TMPL_if>
</TMPL_loop>
";

//
// �������� �� ������� ������ � �������������
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
// �������� �� ������� ������ � ������������ ������
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

	// ������� ��������� ������
	pParamData = pd_init(V_HASH);
	// ��������� �� ������� ������
	check_error_PD(pParamData);
	// ��������� ���������� ��������
	pd_hash_insert_new_var(pParamData, "global_param", "Global Parameter");

	// ��������� ����
	pArray = pd_hash_insert_new_array(pParamData, "EMPLOYEE_INFO");

	// ������ ����� - ��� � �����������
	pHash = pd_array_insert_new_hash(pArray);
	// �������� "NAME"
	pd_hash_insert_new_var(pHash, "NAME", "������ ����");
	// �������� "JOB"
	pd_hash_insert_new_var(pHash, "JOB", "����������");

	pHash = pd_array_insert_new_hash(pArray);
	pd_hash_insert_new_var(pHash, "NAME", "������ ����");
	pd_hash_insert_new_var(pHash, "JOB", "���������");

	// ��������� ���������� ��������
	pd_hash_insert_new_var(pHash, "global_param", "Replaced Parameter");

	// ������������
	pCTPP = template_text_init();

	// ������ ������
	template_text_parse(pCTPP, szTemplate);
	check_error_CTPP(pCTPP);

	// ����������� ��������� �� ������
	template_text_param(pCTPP, pParamData);
	check_error_CTPP(pCTPP);

	// �������� ���������
	template_text_output(pCTPP, &szOutput, &lResultLength);
	check_error_CTPP(pCTPP);

	// �������
	printf("%s\n", szOutput);

	// ����������� ������
	template_text_destroy(pCTPP);
return 0;
}
