#pragma once

#ifndef __TEMPLATEEXCEPTION_H__
#define __TEMPLATEEXCEPTION_H__

#include <Stamina/Exception.h>

class TemplateException: public ExceptionString {
public:
  TemplateException(const StringRef& reason): ExceptionString(reason) { }
};

#endif // __TEMPLATEEXCEPTION_H__