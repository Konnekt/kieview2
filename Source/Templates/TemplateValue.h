#pragma once

#ifndef __TEMPLATE_VALUE_H__
#define __TEMPLATE_VALUE_H__

class iTemplateValue {
  public:
  iTemplateValue& operator + (iTemplateValue* value) {
    plus(value);
    return *this;
  }
  iTemplateValue& operator - (iTemplateValue* value) {
    minus(value);
    return *this;
  }
 // iTemplateValue& operator not (iTemplateValue* value) {
  //  not(value);
 //   return *this;
//  }
  iTemplateValue& operator != (iTemplateValue* value) {
    diff(value);
    return *this;
  }
  iTemplateValue& operator == (iTemplateValue* value) {
    comp(value);
    return *this;
  }
  iTemplateValue& operator && (iTemplateValue* value) {
    and(value);
    return *this;
  }
  iTemplateValue& operator || (iTemplateValue* value) {
    or(value);
    return *this;
  }

  private:
    virtual void plus(iTemplateValue* value);
    virtual void minus(iTemplateValue* value);
    virtual void not(iTemplateValue* value);
    virtual void comp(iTemplateValue* value);
    virtual void and(iTemplateValue* value);
    virtual void or(iTemplateValue* value);
    virtual void diff(iTemplateValue* value);
};



#endif // __TEMPLATE_VALUE_H__