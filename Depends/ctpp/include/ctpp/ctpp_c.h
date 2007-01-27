/*-------------------------------------------------------------------------
 *
 * ctpp_c.h
 *
 *         ��������: BSD.
 *         Copyright (c) 2004 Andrei V. Shetuhin
 *
 *         �-�������
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
// ����
//
#define V_VAL   0
#define V_ARRAY 1
#define V_HASH  2

//
// ������ �������������
//
typedef struct
{
	void       * p_ctpp;
	const char * error;
	int          error_code;
} C_CTPP;

//
// ������ ������������ ������
//
typedef struct
{
	void       * p_param_data;
	const char * error;
	int          error_code;
} P_DATA;

//
// ������������� ������
//
C_CTPP * template_text_init(void);

//
// �������� ��� ��������
//
int template_text_set_include_dirs(C_CTPP * pCTPP, const char ** pszDirs);

//
// ������� �������
//
int template_text_parse(C_CTPP * pCTPP, const char * szTemplate);

//
// ������� ����������
//
int template_text_param(C_CTPP * pCTPP, P_DATA * pData);

//
// ����� ����������
//
int template_text_output(C_CTPP * pCTPP, const char ** szOutput, long * lResultLength);

//
// ������� �������
//
int template_text_clear_template(C_CTPP * pCTPP);

//
// ���������� ������
//
void template_text_destroy(C_CTPP * pCTPP);

// /////////////////////////////////////////////////////////////////////////////

//
// ������������� ������
//
P_DATA * pd_init(int iType);

//
// ��� ����������
//
int pd_get_value_type(P_DATA * pParamData);

//
// ������� ���������� � ���
//
P_DATA * pd_hash_insert_new_var(P_DATA * pParamData, const char * szKey, const char * szValue);

//
// ������� ������� � ���
//
P_DATA * pd_hash_insert_new_array(P_DATA * pParamData, const char * szKey);

//
// ������� ���������� � ������
//
P_DATA * pd_array_insert_new_var(P_DATA * pParamData, const char * szKey);

//
// ������� ���� � ������
//
P_DATA * pd_array_insert_new_hash(P_DATA * pParamData);

//
// �������� ���������� �� ����
//
int pd_hash_erase_var(P_DATA * pParamData, const char * szKey);

//
// ���������� ������
//
void pd_destroy(P_DATA * pParamData);

#if defined(__cplusplus)
}
#endif

#endif // _CTPP_C_H__
// End.
