#include "stdafx.h"
#include "iSectionToken.h"

void iSectionToken::parseArguments(string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  TemplateParam* newParam;
  bool lastComma = false;
  string name;
  UINT index = 0;

  while (itCurrPos != itEnd && (*itCurrPos == ' ' || *itCurrPos == '\t')) {
    itCurrPos++;
  }
  do {
    if (*itCurrPos == ',') itCurrPos++;
    while (itCurrPos != itEnd && (*itCurrPos == ' ' || *itCurrPos == '\t')) {
    itCurrPos++;
    }
    if (getSectionType() == tNamed) {
      string::iterator it = itCurrPos;
      while(itCurrPos != itEnd) {
        //if (*itCurrPos == '}') {
       //   throw TemplateException("Syntax error. Bad argument definition.");
       // } else 
        if (*itCurrPos == '=') {
          itCurrPos++;
          break;
        } else if ((*itCurrPos >= 'a' && *itCurrPos <= 'z') || (*itCurrPos >= 'A' && *itCurrPos <= 'Z') || *itCurrPos == '_' ||
          (*itCurrPos >= '0' && *itCurrPos <= '9')) 
        {
          name += *itCurrPos;
        }
        else {
          throw TemplateException("Syntax error. Bad argument definition.");
        }
        itCurrPos++;
      }
      name = string(it, itCurrPos - 1);
      if (name.empty()) {
        throw TemplateException("Syntax error. Argument is unnamed.");
      }
    }
    newParam = new TemplateParam(getParser(), getParent());
    getParser()->parseParam(newParam, itCurrPos, itEnd, itCurrPos);
    if ((lastComma ||(itCurrPos != itEnd && *itCurrPos == ',')) && !newParam->count()) {
      throw TemplateException("Syntax error. Too many comma signs in xxx");
    }
    if (newParam->count()) {
      if (!name.empty()) {
        _sectionArgs.push_back(new sSectionArg(name, newParam));
      } else {
        _sectionArgs.push_back(new sSectionArg(index++, newParam));
      }
      lastComma = true;
    } else {
      delete newParam;
    }
  } while (itCurrPos != itEnd && *itCurrPos == ',');

  if (itCurrPos == itEnd) {
    throw TemplateException("Syntax error. Right bracked not found in xxx");
  }
  itPos = itCurrPos;
}

iSectionToken::enSectionType iSectionToken::getSectionType() {
  return tUnnamed;
}

iSectionToken::~iSectionToken() {
  for (tSectionArgs::iterator it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    if (getSectionType() == tNamed) {
      delete (*it)->name;
    }
    delete (*it)->param;
    delete *it;
  }
}