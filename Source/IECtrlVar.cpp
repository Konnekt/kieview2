/**
  *  IECtrl::Var
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  */

#include "stdafx.h"
#include "IECtrl.h"

IECtrl::Var::Var() {
  m_eType = Type::Unknown;
  m_nLength = 0;
}

IECtrl::Var::Var(int value) {
  setValue(value);
}

IECtrl::Var::Var(bool value) {
  setValue(value);
}

IECtrl::Var::Var(double value) {
  setValue(value);
}

IECtrl::Var::Var(const char * value) {
  setValue(value);
}

IECtrl::Var::Var(IECtrl::Var & copy) {
  m_eType = Type::Unknown;
  this->copy(copy);
}

IECtrl::Var::Var(VARIANT &v) {
  setValue(v);
}

IECtrl::Var::~Var() {
  clear();
}

IECtrl::Var & IECtrl::Var::operator=(IECtrl::Var &copy) {
  if (this == &copy) return *this;
  this->copy(copy);
  return *this;
}

IECtrl::Var & IECtrl::Var::operator=(int value) {
  setValue(value);
  return *this;
}

IECtrl::Var & IECtrl::Var::operator=(double value) {
  setValue(value);
  return *this;
}

IECtrl::Var & IECtrl::Var::operator=(const char * value) {
  setValue(value);
  return *this;
}

IECtrl::Var & IECtrl::Var::operator=(VARIANT &value) {
  setValue(value);
  return *this;
}

int IECtrl::Var::getInteger() {
  if (m_eType == Type::Integer || m_eType == Type::Boolean)
    return m_iValue;
  else if (m_eType == Type::Real)
    return (int) m_dValue;
  else if (m_eType == Type::String)
    return atoi(m_szValue);

  return 0;
}

bool IECtrl::Var::getBool() {
  if (m_eType == Type::Integer)
    return (bool) m_iValue;
  else if (m_eType == Type::Boolean)
    return m_iValue;
  else if (m_eType == Type::Real)
    return (bool) m_dValue;
  else if (m_eType == Type::String)
    return (bool) getInteger();

  return false;
}

double IECtrl::Var::getReal() {
  if (m_eType == Type::Integer || m_eType == Type::Boolean)
    return (double)m_iValue;
  else if (m_eType == Type::Real)
    return m_dValue;
  else if (m_eType == Type::String)
    return atof(m_szValue);

  return 0.0;
}

const char * IECtrl::Var::getString() {
  char buff[_CVTBUFSIZE] = "";

  if (m_eType == Type::Integer)
    return itoa(m_iValue, buff, 10);
  else if (m_eType == Type::Boolean)
    return m_iValue ? "true" : "false";
  else if (m_eType == Type::Real)
    return _gcvt(m_dValue, 12, buff);
  else if (m_eType == Type::String)
    return m_szValue;
  else if (m_eType == Type::Array)
    return "Array";

  return "";
}

VARIANT * IECtrl::Var::getVariant(VARIANT *v) {
  if (v == NULL) {
    v = new VARIANT;
    VariantInit(v);
  }

  v->vt = VT_EMPTY;
  if (m_eType == Type::Integer) {
    v->vt = VT_INT;
    v->intVal = m_iValue;
  } else if (m_eType == Type::Boolean) {
    v->vt = VT_BOOL;
    v->boolVal = (bool) m_iValue;
  } else if (m_eType == Type::Real) {
    v->vt = VT_R8;
    v->dblVal = m_dValue;
  } else if (m_eType == Type::String) {
    v->vt = VT_BSTR;
    v->bstrVal = _com_util::ConvertStringToBSTR(m_szValue);
  }

  return v;
}

int IECtrl::Var::length() {
  if (m_eType == Type::Array) {
    return m_nLength;
  }
  return 0;
}

IECtrl::Var & IECtrl::Var::operator[](int i) {
  return getElement(i);
}

void IECtrl::Var::operator+=(IECtrl::Var & var) {
  switch (m_eType) {
    case Type::Integer:
      m_iValue += var.getInteger();
      break;

    case Type::Real:
      m_dValue += var.getReal();
      break;

    case Type::Array:
    case Type::Unknown:
      clear();
      setValue("");
      break;

    case Type::String: {
      const char * temp = strdup(var.getString());
      int len1 = 0;
      if (m_szValue != NULL) {
        len1 =  strlen(m_szValue);
      }
      int len2 = strlen(temp);
      char * pValue = new char[len1+len2+1];
      if (m_szValue != NULL) {
        CopyMemory(pValue, m_szValue, len1);
      }
      CopyMemory(pValue + len1, temp, len2);
      pValue[len1+len2] = '\0';
      if (m_szValue != NULL) {
        delete [] m_szValue;
      }
      delete [] temp;
      m_szValue = pValue;
      break;
    }
  }
}

void IECtrl::Var::operator+=(int var) {
  *this += IECtrl::Var(var);
}

void IECtrl::Var::operator+=(double var) {
  *this += IECtrl::Var(var);
}

void IECtrl::Var::operator+=(const char* var) {
  *this += IECtrl::Var(var);
}

// private
void IECtrl::Var::clear() {
  if (m_eType == Type::String && m_szValue != NULL) {
    delete [] m_szValue;
  } else if (m_eType == Type::Array && m_aValue != NULL) {
    for (int i = 0; i < m_nLength; i++)
      if (m_aValue[i] != NULL) {
        delete m_aValue[i];
      }
    delete [] m_aValue;
  }
  m_eType = Type::Unknown;
  m_nLength = 0;
}

void IECtrl::Var::copy(IECtrl::Var& copy) {
  clear();
  switch (copy.m_eType) {
    case Type::Integer:
      setValue(copy.m_iValue);
      break;
    case Type::Boolean:
      setValue((bool)copy.m_iValue);
      break;
    case Type::Real:
      setValue(copy.m_dValue);
      break;
    case Type::String:
      setValue(copy.m_szValue);
      break;
    case Type::Array:
      setValue(copy.m_aValue, copy.m_nLength);
      break;
    default:
      m_eType = Type::Unknown;
      m_nLength = 0;
  }
}

void IECtrl::Var::setValue(int value) {
  clear();
  m_eType = Type::Integer;
  m_iValue = value;
}

void IECtrl::Var::setValue(bool value) {
  clear();
  m_eType = Type::Boolean;
  m_iValue = value;
}

void IECtrl::Var::setValue(double value) {
  clear();
  m_eType = Type::Real;
  m_dValue = value;
}

void IECtrl::Var::setValue(const char * value) {
  clear();
  m_eType = Type::String;
  m_szValue = strdup(value);
}

void IECtrl::Var::setValue(IECtrl::Var* value[], unsigned int count) {
  clear();
  m_eType = Type::Array;
  m_nLength = count;
  m_aValue = new IECtrl::Var*[m_nLength];

  for (int i = 0; i < m_nLength; i++) {
    m_aValue[i] = new IECtrl::Var(*(value[i]));
  }
}

void IECtrl::Var::setValue(VARIANT &v) {
  switch (v.vt) {
    case VT_INT: setValue((int)v.intVal); break;
    case VT_BOOL: setValue((bool)v.boolVal); break;
    case VT_I1: setValue((int)v.cVal); break;
    case VT_I2: setValue((int)v.iVal); break;
    case VT_I4: setValue((int)v.lVal); break;
    case VT_UINT: setValue((int)v.uintVal); break;
    case VT_UI1: setValue((int)v.bVal); break;
    case VT_UI2: setValue((int)v.uiVal); break;
    case VT_UI4: setValue((int)v.ulVal); break;
    case VT_R4: setValue((double)v.fltVal); break;
    case VT_R8: setValue((double)v.dblVal); break;
    case VT_BSTR: {
      const char * temp = _com_util::ConvertBSTRToString(v.bstrVal);
      setValue(temp);
      delete [] temp;
      break;
    }
    /*
    case VT_DISPATCH: {
      IDispatch * pDispatch = v.pdispVal;
      IDispatchEx * pDispEx = NULL;
      if (SUCCEEDED(pDispatch->QueryInterface(IID_IDispatchEx, (void**)&pDispEx))) {
        HRESULT hr;
        BSTR bstrName;
        DISPID dispid;

        hr = pDispEx->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
        while (hr == NOERROR) {
          hr = pDispEx->GetMemberName(dispid, &bstrName);
          if (FAILED(hr)) continue;
          VARIANT vRet;
          VariantInit(&vRet);
          DISPPARAMS dispparams = {NULL, NULL, 0, 0};
          EXCEPINFO excepInfo; memset(&excepInfo, 0, sizeof(excepInfo));
          UINT nArgErr = (UINT)-1;  // initialize to invalid arg
          hr = pDispEx->InvokeEx(dispid, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dispparams, &vRet, &excepInfo, NULL);
          if (FAILED(hr)) continue;
          Var ret(vRet);
          VariantClear(&vRet);
        
          const char * szName = _com_util::ConvertBSTRToString(bstrName);
          SysFreeString(bstrName);
          MessageBox(0, ret.getString(), szName, 0); // @@@
          delete [] szName;
          hr = pDispEx->GetNextDispID(fdexEnumAll, dispid, &dispid);
        }
        pDispEx->Release();
      }
      break;
    }
    */
  }
}

IECtrl::Var & IECtrl::Var::getElement(int i) {
  if (i < 0) i = m_nLength;

  if (m_eType == Type::Array && i < m_nLength)
  {
  } else if (m_eType == Type::Array) {
    IECtrl::Var** pValue = new IECtrl::Var*[i+1];
    for (int j = 0; j < i + 1; j++) {
      if (j < m_nLength) {
        pValue[j] = m_aValue[j];
      } else {
        pValue[j] = new IECtrl::Var;
      }
    }
    if (m_aValue != NULL) {
      delete [] m_aValue;
    }
    m_eType = Type::Array;
    m_nLength = i+1;
    m_aValue = pValue;
  } else {
    clear();
    m_eType = Type::Array;
    m_nLength = i+1;
    m_aValue = new IECtrl::Var*[m_nLength];

    for (int j = 0; j < m_nLength; j++) {
      m_aValue[j] = new IECtrl::Var;
    }
  }
  return *(m_aValue[i]);
}

IECtrl::Var IECtrl::Var::operator+(IECtrl::Var & var) {
  IECtrl::Var ret(*this);
  ret += var;
  return ret;
}

IECtrl::Var IECtrl::Var::operator+(int var) {
  IECtrl::Var ret(*this);
  ret += IECtrl::Var(var);
  return ret;
}

IECtrl::Var IECtrl::Var::operator+(double var) {
  IECtrl::Var ret(*this);
  ret += IECtrl::Var(var);
  return ret;
}

IECtrl::Var IECtrl::Var::operator+(const char *var) {
  IECtrl::Var ret(*this);
  ret += IECtrl::Var(var);
  return ret;
}