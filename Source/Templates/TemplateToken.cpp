#include "stdafx.h"

#include "TemplateToken.h"
#include "TemplateParser.h"
#include "Template.h"

/*
 iSection Interface
*/

/*TemplateValue iTemplateToken::getVariable(const std::string &name) {
  if (getParent() && getParent()->hasVariable(name)) {
    return getParent()->getVariable(name);
  }
  return GlobalsManager::get()->getVariable(name);
}

bool Template::setVariable(const string& name, const oTemplateValue& value, bool create) {
  if (getParent() && getParent()->hasVariable(name)) {
    return getParent()->setVariable(name, value);
  } else if (GlobalsManager::get()->hasVariable(name)) {
    return GlobalsManager::get()->setVariable(name, value);
  } else if (getParent()) {
    return getParent()->setVariable(name, value, true);
  }
  return false;
}*/

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

/*
 BlockToken
*/

void iBlockToken::add(iTemplateToken* token) {
  _token.push_back(token);
}

iTemplateToken* iBlockToken::get(UINT id) {
  if (id > count()) return NULL;

  tTokenList::iterator it = _token.begin();
  int i = 0;
  while(i < id) {
    i++;
    it++;
  }
  return *it;
}

iSectionToken::enSectionType iBlockToken::getSectionType() {
  return tUnnamed;
}

void iBlockToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  getParser()->parse(this, itCurrPos, itEnd, stopToken, itPos, allowCreateTokens);
}

bool iBlockToken::remove(UINT id) {
  if (id > count()) return false;

  tTokenList::iterator it = _token.begin();
  int i = 0;
  while(i < id) {
    i++;
    it++;
  }
  _token.erase(it);
  return true;
}

UINT iBlockToken::count() {
  return _token.size();
}

string iBlockToken::output() {
  string out;
  tTokenList::iterator it = _token.begin();
  tTokenList::iterator itEnd = _token.end();

  while(it != itEnd) {
    out += (*it)->output();
    it++;
  }
  return out;
}

void iBlockToken::clear() {
  tTokenList::iterator it = _token.begin();
  tTokenList::iterator itEnd = _token.end();

  while (it != itEnd) {
    delete *it;
  }
  _token.clear();
}