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

IECtrl::Var::Var(const IECtrl::Var& copy) {
  m_eType = Type::Unknown;
  this->copy((IECtrl::Var&)copy);
}

IECtrl::Var::Var(IECtrl::Var & copy) {
  m_eType = Type::Unknown;
  this->copy(copy);
}

IECtrl::Var::Var(VARIANT &v) {
  setValue(v);
}

IECtrl::Var::Var(Date64 &v) {
  setValue(v);
}

IECtrl::Var::~Var() {
  clear();
}

const IECtrl::Var & IECtrl::Var::operator=(const IECtrl::Var &copy) {
  if (this == &copy) return *this;
  this->copy((IECtrl::Var&)copy);
  return *this;
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

IECtrl::Var & IECtrl::Var::operator=(Date64 &value) {
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
  else if (m_eType == Type::Date) {
    if (m_dtValue != NULL) {
      return m_dtValue->strftime("%Y-%m-%d %H:%M:%S").c_str();
    } else {
      return Date64().strftime("%Y-%m-%d %H:%M:%S").c_str();
    }
  }

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
  } else if (m_eType == Type::Date) {
    v->vt = VT_DATE;
    TIME_ZONE_INFORMATION time_zone;
    GetTimeZoneInformation(&time_zone);
    if (m_dtValue != NULL) {
      __int64 date = m_dtValue->getTime64();
      v->date = date - (time_zone.Bias * 60);
      v->date/= (24 * 60 * 60);
      v->date+= 25569;
    } else {
      v->date= 0;
    }
  } else if (m_eType == Type::Array) {
    if (v->parray && (v->vt & VT_ARRAY)) {
      if (v->parray->rgsabound[0].cElements < m_nLength && m_aValue != NULL) {
        SafeArrayDestroyData(v->parray);
      }
    }
    SAFEARRAYBOUND rgsabound;
    rgsabound.cElements = length();
    rgsabound.lLbound = 0;
    SAFEARRAY* psa = SafeArrayCreate(VT_VARIANT, 1, &rgsabound);

    for(LONG i = 0; i < psa->rgsabound[0].cElements; i++) {
       SafeArrayPutElement(psa, &i, m_aValue[i]->getVariant(NULL));
    }
    v->parray = psa;
    v->vt = VT_ARRAY | VT_VARIANT;
  }

  return v;
}

Date64 IECtrl::Var::getDate() {
  if (m_eType == Type::Date && m_dtValue != NULL) {
    return (*m_dtValue);
  }

  return Date64(true);
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

    case Type::Array: {
      if (var.m_nLength > 0) {
        Var** pValue = new IECtrl::Var*[m_nLength + var.m_nLength];
        CopyMemory(pValue, m_aValue, sizeof(IECtrl::Var*) * m_nLength);

        for (int i = m_nLength; i < m_nLength + var.m_nLength; i++) {
          pValue[i] = new IECtrl::Var(var.getElement(i - m_nLength));
        }
        delete [] m_aValue;
        m_aValue = pValue;
        m_nLength += var.m_nLength;
      }
      break;
    }
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

    case Type::Date: {
      if (m_dtValue) {
        (*m_dtValue) = (*m_dtValue).getInt64() + var.getDate().getInt64();
      } else {
        setValue(var.getDate());
      }
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

void IECtrl::Var::operator+=(Date64& var) {
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
  } else if (m_eType == Type::Date) {
    delete m_dtValue;
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
    case Type::Date:
      setValue(copy.getDate());
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
    case VT_DATE: {
      TIME_ZONE_INFORMATION time_zone;
      GetTimeZoneInformation(&time_zone);
      DATE date = v.date - 25569;
      __int64 date64 = date * (24 * 60 * 60) + (time_zone.Bias * 60);
      setValue(Date64(date64));
      break;
    }
    case VT_ARRAY: {
      if (v.parray->cDims != 1 || (SafeArrayLock(v.parray) < 0)) return;

      if (v.parray->fFeatures & FADF_VARIANT) {
        IECtrl::Var** var = new IECtrl::Var*[v.parray->rgsabound[0].cElements];
        VARIANT* vArray = (VARIANT *)v.parray->pvData;

        for (DWORD i = 0; i < v.parray->rgsabound[0].cElements; i++) {
          var[i] = new IECtrl::Var(vArray[i]);
        }
        setValue(var, v.parray->rgsabound[0].cElements);

        for (DWORD i = 0; i < v.parray->rgsabound[0].cElements; i++) {
          delete var[i];
        }
        delete [] var;
      }
      if (v.parray->fFeatures & FADF_BSTR) {
      }
      
      SafeArrayUnlock(v.parray);
    }
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
          hr = pDispEx->GetNextDispID(fdexEnumAll, dispid, &dispid);
        }
        pDispEx->Release();
      }
      break;
    }

  }
}
void IECtrl::Var::setValue(Date64 &v) {
  clear();
  m_dtValue = new Date64(v.getTime64());
  m_eType = Type::Date;
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

IECtrl::Var IECtrl::Var::operator+(Date64 &var) {
  IECtrl::Var ret(*this);
  ret += IECtrl::Var(var);
  return ret;
}

IECtrl::Object::Object(IDispatch* pdScript, const char* name, Var args) {
  DISPID dispid;

  pdScript->QueryInterface(IID_IDispatchEx, (void **)&_pdexScript);
  BSTR bstrName = _com_util::ConvertStringToBSTR(name);
  _pdexScript->GetDispID(bstrName, 0, &dispid);

  VARIANT vRet;
  DISPPARAMS dispparams = {0, 0, NULL, NULL};
  dispparams.rgvarg = new VARIANT[args.length()];

  for(int i= 0;i < args.length(); i++) {
    args[i].getVariant(&dispparams.rgvarg[i]);
  }

  dispparams.cArgs = args.length();
  dispparams.cNamedArgs = 0;
  _pdexScript->InvokeEx(dispid, LOCALE_USER_DEFAULT, DISPATCH_CONSTRUCT, &dispparams, &vRet, NULL, NULL);
  delete [] dispparams.rgvarg;

  _pdispObj = vRet.pdispVal;
  _pdispObj->QueryInterface(IID_IDispatchEx, (void **)&_pdexObj);
}

bool IECtrl::Object::bindMethod(const char* name, const char* func) {
  DISPID dispid;
  BSTR bstrName = _com_util::ConvertStringToBSTR(func);
  _pdexScript->GetDispID(bstrName, fdexNameEnsure, &dispid);

  VARIANT vRet;
  VariantInit(&vRet);
  DISPPARAMS dispparams, dispparamsNoArgs = {NULL, NULL, 0, 0};

  _pdexScript->InvokeEx(dispid, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dispparamsNoArgs, &vRet, NULL, NULL);

  addElement(name, dispid);

  DISPID putid = DISPID_PROPERTYPUT;
  dispparams.rgvarg = &vRet;
  dispparams.rgdispidNamedArgs = &putid;
  dispparams.cArgs = 1;
  dispparams.cNamedArgs = 1;
  _pdexObj->InvokeEx(dispid, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF, &dispparams, NULL, NULL, NULL);
  return true;
}

void IECtrl::Object::addElement(const char *name, DISPID& dispid) {
  BSTR bstrName = _com_util::ConvertStringToBSTR(name);
  _pdexObj->GetDispID(bstrName, fdexNameEnsure, &dispid);
}


void IECtrl::Object::setPropety(const char *name, VARIANT& v) {
  DISPID dispid;
  DISPPARAMS dispparams;

  BSTR bstrName = _com_util::ConvertStringToBSTR(name);
  _pdexObj->GetDispID(bstrName, 0, &dispid);

  DISPID putid = DISPID_PROPERTYPUT;
  dispparams.rgvarg = &v;
  dispparams.rgdispidNamedArgs = &putid;
  dispparams.cArgs = 1;
  dispparams.cNamedArgs = 1;
  _pdexObj->InvokeEx(dispid, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF, &dispparams, NULL, NULL, NULL);
}

void IECtrl::Object::getPropety(const char *name, VARIANT& v) {
  DISPID dispid;
  DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};

  BSTR bstrName = _com_util::ConvertStringToBSTR(name);
  _pdexObj->GetDispID(bstrName, 0, &dispid);
  delete [] bstrName;

  _pdexObj->InvokeEx(dispid, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dispparamsNoArgs, &v, NULL, NULL);
}

VARIANT* IECtrl::Object::getObject(VARIANT* v) {
  if (v == NULL) {
    v = new VARIANT;
    VariantInit(v);
  }

  v->vt = VT_DISPATCH;
  v->pdispVal = _pdexObj;
  return v;
}

IECtrl::Object::~Object() {
}