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

IECtrl::Var::Var(bool value) {
  setValue(value);
}

IECtrl::Var::Var(int value) {
  setValue(value);
}

IECtrl::Var::Var(UINT value) {
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

IECtrl::Var::Var(Stamina::String &v) {
  setValue(v);
}

IECtrl::Var::Var(string &v) {
  setValue(v);
}

IECtrl::Var::Var(Stamina::ByteBuffer &v) {
  setValue(v);
}

IECtrl::Var::Var(IECtrl::Object &v) {
  setValue(v);
}

IECtrl::Var::Var(IDispatch *v) {
  setValue(v);
}

IECtrl::Var::Var(IDispatch **v) {
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

IECtrl::Var & IECtrl::Var::operator=(bool value) {
  setValue(value);
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

IECtrl::Var & IECtrl::Var::operator=(Stamina::String &value) {
  setValue(value);
  return *this;
}

IECtrl::Var & IECtrl::Var::operator=(string &value) {
  setValue(value);
  return *this;
}

IECtrl::Var & IECtrl::Var::operator=(Stamina::ByteBuffer &value) {
  setValue(value);
  return *this;
}

IECtrl::Var & IECtrl::Var::operator=(IECtrl::Object &value) {
  setValue(value);
  return *this;
}

IECtrl::Var & IECtrl::Var::operator=(IDispatch* value) {
  setValue(value);
  return *this;
}

IECtrl::Var & IECtrl::Var::operator=(IDispatch** value) {
  setValue(value);
  return *this;
}
int IECtrl::Var::getInteger() {
  if (m_eType == Type::Integer || m_eType == Type::Boolean)
    return m_iValue;
  else if (m_eType == Type::Uint)
    return m_uValue;
  else if (m_eType == Type::Real)
    return (int) m_dValue;
  else if (m_eType == Type::String)
    return atoi(m_szValue);

  return 0;
}

UINT IECtrl::Var::getUint() {
  if (m_eType == Type::Uint)
    return m_uValue;
  else if (m_eType == Type::Integer || m_eType == Type::Boolean)
    return m_iValue;
  else if (m_eType == Type::Real)
    return (UINT) m_dValue;
  else if (m_eType == Type::String)
    return atol(m_szValue);

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
  else if (m_eType == Type::Uint)
    return (double)m_uValue;
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
  if (m_eType == Type::Uint)
    return ltoa(m_uValue, buff, 10);
  else if (m_eType == Type::Boolean)
    return m_iValue ? "true" : "false";
  else if (m_eType == Type::Real)
    return _gcvt(m_dValue, 12, buff);
  else if (m_eType == Type::StaminaString)
    return m_ssValue->a_str();
  else if (m_eType == Type::StdString)
    return m_sValue->c_str();
  else if (m_eType == Type::String)
    return m_szValue;
  else if (m_eType == Type::ByteBuffer)
    return "ByteBuffer";
  else if (m_eType == Type::Array)
    return "Array";
  else if (m_eType == Type::Object)
    return "Object";
  else if (m_eType == Type::Dispatch)
    return "Dispatch";
  else if (m_eType == Type::DispatchRef)
    return "Dispatch Reference";
  else if (m_eType == Type::Date) {
    if (m_dtValue != NULL) {
      return m_dtValue->strftime("%Y-%m-%d %H:%M:%S").c_str();
    } else {
      return Date64().strftime("%Y-%m-%d %H:%M:%S").c_str();
    }
  }
  return "";
}

String IECtrl::Var::getStaminaString() {
  return getString();
}

string IECtrl::Var::getStdString() {
  return getString();
}

Stamina::ByteBuffer IECtrl::Var::getByteBuffer() {
  if (isByteBuffer()) {
    return *m_bbValue;
  } else {
    return Stamina::ByteBuffer((unsigned char *)getString(), strlen(getString()));
  }
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
  } else if (m_eType == Type::Uint) {
    v->vt = VT_UINT;
    v->uintVal = m_uValue;
  } else if (m_eType == Type::Boolean) {
    v->vt = VT_BOOL;
    v->boolVal = (bool) m_iValue;
  } else if (m_eType == Type::Real) {
    v->vt = VT_R8;
    v->dblVal = m_dValue;
  } else if (isString()) {
    v->vt = VT_BSTR;
    v->bstrVal = _com_util::ConvertStringToBSTR(getString());
  } else if (isByteBuffer()) {
    BLOB b;
    b.cbSize = m_bbValue->getLength();
    b.pBlobData = m_bbValue->getBuffer();
    v->vt = VT_BLOB;
    memcpy(&v->llVal, &b, sizeof(b)); //nie ma gdzie wsadzic struktury blob
  } else if (m_eType == Type::Date) {
    v->vt = VT_DATE;
    TIME_ZONE_INFORMATION time_zone;
    GetTimeZoneInformation(&time_zone);

    if (m_dtValue != NULL) {
      __int64 date = m_dtValue->getTime64();
      v->date = date - (time_zone.Bias * 60);
      v->date /= (24 * 60 * 60);
      v->date += 25569;
    } else {
      v->date = 0;
    }
  } else if (m_eType == Type::Object) {
    m_objValue->getVariant(v);
  } else if (m_eType == Type::Dispatch) {
    v->vt = VT_DISPATCH;
    v->pdispVal = m_dispValue;
  } else if (m_eType == Type::DispatchRef) {
    v->vt = VT_DISPATCH | VT_BYREF;
    v->ppdispVal = m_pdispValue;
  }else if (m_eType == Type::Array) {
    /*
    if (v->parray && (v->vt & VT_ARRAY)) {
      if (v->parray->rgsabound[0].cElements < m_nLength && m_aValue != NULL) {
        SafeArrayDestroyData(v->parray);
      }
    }
    SAFEARRAY* psa = SafeArrayCreateVector(VT_VARIANT, 0, m_nLength);

    for (LONG i = 0; i < m_nLength; i++) {
       SafeArrayPutElement(psa, &i, m_aValue[i]->getVariant());
    }
    v->parray = psa;
    v->vt = VT_ARRAY | VT_VARIANT;
    */
  }
  return v;
}

Date64 IECtrl::Var::getDate() {
  if (m_eType == Type::Date && m_dtValue != NULL) {
    return (*m_dtValue);
  }
  return Date64(true);
}

IECtrl::Object IECtrl::Var::getObject(IECtrl* ctrl) {
  if (m_eType == Type::Object && m_objValue) {
    return (*m_objValue);
  } else if (m_eType == Type::Dispatch && m_dispValue) {
    return IECtrl::Object(ctrl, m_dispValue);
  } else if (m_eType == Type::Array) {
    return IECtrl::Object(ctrl, false, "Array", *this);
  }

  Var args;
  args[-1] = Var(*this);

  if (m_eType == Type::Integer || m_eType == Type::Real || m_eType == Type::Uint) {
    return IECtrl::Object(ctrl, false, "Number", args);
  } else if (m_eType == Type::Date) {
    return IECtrl::Object(ctrl, false, "Date", args);
  } else if (isString()) {
    return IECtrl::Object(ctrl, false, "String", args);
  } else if (m_eType == Type::Boolean) {
    return IECtrl::Object(ctrl, false, "Boolean", args);
  }
  return IECtrl::Object(ctrl, false);
}

bool IECtrl::Var::isUnknown() {
  return m_eType == Type::Unknown;
}
bool IECtrl::Var::isInteger() {
  return m_eType == Type::Integer;
}
bool IECtrl::Var::isUint() {
  return m_eType == Type::Integer;
}
bool IECtrl::Var::isBool() {
  return m_eType == Type::Boolean;
}
bool IECtrl::Var::isReal() {
  return m_eType == Type::Real;
}
bool IECtrl::Var::isString() {
  return m_eType == Type::String || m_eType == Type::StdString || m_eType == Type::StaminaString;
}
bool IECtrl::Var::isByteBuffer() {
  return m_eType == Type::ByteBuffer;
}
bool IECtrl::Var::isDate() {
  return m_eType == Type::Date;
}
bool IECtrl::Var::isObject() {
  return m_eType == Type::Object;
}
bool IECtrl::Var::isDispatch() {
  return m_eType == Type::Dispatch;
}
bool IECtrl::Var::isDispatchRef() {
  return m_eType == Type::DispatchRef;
}

IDispatch* IECtrl::Var::getDispatch() {
  if (m_eType == Type::Dispatch) {
    return m_dispValue;
  }
  return NULL;
}

IDispatch** IECtrl::Var::getDispatchRef() {
  if (m_eType == Type::DispatchRef) {
    return m_pdispValue;
  }
  return NULL;
}

bool IECtrl::Var::empty() {
  switch (m_eType) {
    case Type::Array:
      if (!length()) return true;
      break;
    case Type::Unknown:
      return true;
    case Type::Integer:
    case Type::Boolean:
    case Type::Real:
      if (!getInteger()) return true;
      break;
    case Type::Uint:
      if (!getUint()) return true;
      break;
    case Type::StaminaString:
    case Type::StdString:
    case Type::String:
      if (!strlen(getString())) return true;
      break;
    case Type::ByteBuffer:
      if (!m_bbValue->getLength()) return true;
      break;
  }
  return false;
}

int IECtrl::Var::length() {
  if (m_eType == Type::Array) {
    return m_nLength;
  } else if (isByteBuffer()) {
    return m_bbValue->getLength();
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

    case Type::Uint:
      m_uValue += var.getUint();
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

    case Type::StaminaString: {
      *m_ssValue += var.getString();
      break;
    }

    case Type::StdString: {
      *m_sValue += var.getString();
      break;
    }

    case Type::ByteBuffer: {
      Stamina::ByteBuffer& bb = var.getByteBuffer();
      m_bbValue->append(bb.getBuffer(), bb.getLength());
    }

    case Type::Date: {
      if (m_dtValue) {
        (*m_dtValue) = m_dtValue->getInt64() + var.getDate().getInt64();
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

void IECtrl::Var::operator+=(UINT var) {
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

void IECtrl::Var::operator+=(Stamina::String& var) {
  *this += IECtrl::Var(var);
}

void IECtrl::Var::operator+=(string& var) {
  *this += IECtrl::Var(var);
}

void IECtrl::Var::operator+=(Stamina::ByteBuffer& var) {
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
  } else if (m_eType == Type::Object) {
    delete m_objValue;
  } else if (m_eType == Type::StaminaString) {
    delete m_ssValue;
  } else if (m_eType == Type::StdString) {
    delete m_sValue;
  } else if (isByteBuffer()) {
    delete m_bbValue;
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
    case Type::Uint:
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
    case Type::StaminaString:
      setValue(copy.m_ssValue);
      break;
    case Type::StdString:
      setValue(copy.m_sValue);
      break;
    case Type::ByteBuffer:
      setValue(*copy.m_bbValue);
      break;
    case Type::Array:
      setValue(copy.m_aValue, copy.m_nLength);
      break;
    case Type::Date:
      setValue(copy.getDate());
      break;
    case Type::Object:
      setValue(copy.getObject());
      break;
    case Type::Dispatch:
      setValue(copy.getDispatch());
      break;
    case Type::DispatchRef:
      setValue(copy.getDispatchRef());
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

void IECtrl::Var::setValue(UINT value) {
  clear();
  m_eType = Type::Uint;
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
    case VT_UINT: setValue(v.uintVal); break;
    case VT_BOOL: setValue((bool)v.boolVal); break;
    case VT_I1: setValue((int)v.cVal); break;
    case VT_I2: setValue((int)v.iVal); break;
    case VT_I4: setValue((int)v.lVal); break;
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
    case VT_BLOB: {
      BLOB b;
      memcpy(&b, &v.llVal, sizeof(b));
      setValue(Stamina::ByteBuffer(b.pBlobData, b.cbSize));
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
        //
      }
      SafeArrayUnlock(v.parray);
      break;
    }
    case VT_DISPATCH: {
      /*
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
      */
      setValue(v.pdispVal);
      break;
    }
    case VT_DISPATCH | VT_BYREF: setValue(v.ppdispVal); break;
  }
}

void IECtrl::Var::setValue(Date64 &v) {
  clear();
  m_dtValue = new Date64(v.getTime64());
  m_eType = Type::Date;
}

void IECtrl::Var::setValue(Stamina::String &v) {
  clear();
  m_ssValue = new Stamina::String(v);
  m_eType = Type::StaminaString;
}

void IECtrl::Var::setValue(string &v) {
  clear();
  m_sValue = new string(v);
  m_eType = Type::StdString;
}

void IECtrl::Var::setValue(Stamina::ByteBuffer &v) {
  clear();
  m_bbValue = new Stamina::ByteBuffer(v);
  m_eType = Type::ByteBuffer;
}

void IECtrl::Var::setValue(IECtrl::Object &v) {
  clear();
  m_objValue = new IECtrl::Object(v);
  m_eType = Type::Object;
}

void IECtrl::Var::setValue(IDispatch* dispatch) {
  clear();
  m_dispValue = dispatch;
  m_eType = Type::Dispatch;
}

void IECtrl::Var::setValue(IDispatch** dispatch) {
  clear();
  m_pdispValue = dispatch;
  m_eType = Type::DispatchRef;
}

IECtrl::Var & IECtrl::Var::getElement(int i) {
  if (i < 0) i = m_nLength;

  if (m_eType == Type::Array && i < m_nLength) {
    //
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

IECtrl::Var IECtrl::Var::operator+(UINT var) {
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

IECtrl::Var IECtrl::Var::operator+(Stamina::String &var) {
  IECtrl::Var ret(*this);
  ret += IECtrl::Var(var);
  return ret;
}

IECtrl::Var IECtrl::Var::operator+(string &var) {
  IECtrl::Var ret(*this);
  ret += IECtrl::Var(var);
  return ret;
}

IECtrl::Var IECtrl::Var::operator+(Stamina::ByteBuffer &var) {
  IECtrl::Var ret(*this);
  ret += IECtrl::Var(var);
  return ret;
}

IECtrl::Object::Object(IECtrl* ctrl, bool defaultDestruct, const char* name, Var args): _ownObject(true), _defDestruct(defaultDestruct) {
  _refs = new UINT(1);

  IDispatch* pdScript = ctrl->getDispatch();
  pdScript->QueryInterface(IID_IDispatchEx, (void **)&_pdexScript);
  DISPID dispID = getDispID(_pdexScript, name);

  VARIANT vRet;
  DISPPARAMS dispparams = {0, 0, NULL, NULL};
  dispparams.rgvarg = new VARIANT[args.length()];

  for (UINT i = 0, l = args.length(); i < l; i++) {
    args[l - i - 1].getVariant(&dispparams.rgvarg[i]);
  }

  dispparams.cArgs = args.length();
  dispparams.cNamedArgs = 0;
  _pdexScript->InvokeEx(dispID, LOCALE_USER_DEFAULT, DISPATCH_CONSTRUCT, &dispparams, &vRet, NULL, NULL);
  delete [] dispparams.rgvarg;

  _pdispObj = vRet.pdispVal;
  _pdispObj->QueryInterface(IID_IDispatchEx, (void **)&_pdexObj);
}

IECtrl::Object::Object(IECtrl* ctrl, IDispatch* dispatch): _ownObject(false) {
  _refs = new UINT(1);
  ASSERT(dispatch != NULL);

  IDispatch* pdScript = ctrl->getDispatch();

  pdScript->QueryInterface(IID_IDispatchEx, (void **)&_pdexScript);
  dispatch->QueryInterface(IID_IDispatchEx, (void **)&_pdexObj);

  ASSERT(_pdexObj != NULL);
}
IECtrl::Object::Object(const Object &object) {
  CopyMemory(this, &object, sizeof(Object));
  (*_refs)++;
}

void IECtrl::Object::bindMethod(const char* name, const char* func) {
  DISPID fnDispID = getDispID(_pdexScript, func, fdexNameEnsure);
  DISPPARAMS dispparams, dispparamsNoArgs = {NULL, NULL, 0, 0};
  VARIANT vRet;
  VariantInit(&vRet);

  _pdexScript->InvokeEx(fnDispID, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dispparamsNoArgs, &vRet, NULL, NULL);

  DISPID dispID = getDispID(_pdexObj, name, fdexNameEnsure);
  DISPID putID = DISPID_PROPERTYPUT;

  dispparams.rgvarg = &vRet;
  dispparams.rgdispidNamedArgs = &putID;
  dispparams.cArgs = 1;
  dispparams.cNamedArgs = 1;

  _pdexObj->InvokeEx(dispID, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF, &dispparams, NULL, NULL, NULL);
}

void IECtrl::Object::addProperty(const char *name, IECtrl::Var var, bool ref) {
  getDispID(_pdexObj, name, fdexNameEnsure);
  setProperty(name, var, ref);
}

void IECtrl::Object::setProperty(const char *name, IECtrl::Var var, bool ref) {
  DISPPARAMS dispparams;
  VARIANT v;

  var.getVariant(&v);

  DISPID dispID = getDispID(_pdexObj, name);
  DISPID putID = DISPID_PROPERTYPUT;

  dispparams.rgvarg = &v;
  dispparams.rgdispidNamedArgs = &putID;
  dispparams.cArgs = 1;
  dispparams.cNamedArgs = 1;

  _pdexObj->InvokeEx(dispID, LOCALE_USER_DEFAULT, ref ? DISPATCH_PROPERTYPUTREF : DISPATCH_PROPERTYPUT, &dispparams, NULL, NULL, NULL);
}

IECtrl::Var IECtrl::Object::getProperty(const char *name) {
  DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
  DISPID dispID = getDispID(_pdexObj, name);
  VARIANT v;

  _pdexObj->InvokeEx(dispID, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dispparamsNoArgs, &v, NULL, NULL);
  return v;
}

VARIANT* IECtrl::Object::getVariant(VARIANT* v) {
  if (v == NULL) {
    v = new VARIANT;
    VariantInit(v);
  }
  v->vt = VT_DISPATCH;
  v->pdispVal = _pdexObj;

  return v;
}

DISPID IECtrl::Object::getDispID(IDispatchEx* dispatch, const char* name, DWORD flags) {
  BSTR bStrName = _com_util::ConvertStringToBSTR(name);
  DISPID dispID = 0;

  dispatch->GetDispID(bStrName, flags, &dispID);
  SysFreeString(bStrName);

  return dispID;
}

IECtrl::Object::~Object() {
  if(!--(*_refs)) {
    delete _refs;
    _pdexObj->Release();
    if (this->ownObject() && this->_defDestruct) {
      _pdispObj->Release();
    }
    _pdexScript->Release();
  }
}