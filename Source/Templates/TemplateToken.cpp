#include "stdafx.h"
#include "TemplateToken.h"
#include "TemplateParser.h"
#include "Template.h"

/*
 iSection Interface
*/
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
    newParam = new TemplateParam;
    getTemplate()->getParser()->parseParam(newParam, itCurrPos, itEnd, itCurrPos);
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
  getTemplate()->getParser()->parse(this, itCurrPos, itEnd, stopToken, itPos, allowCreateTokens);
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

/*
 TextToken
*/

void TextToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  itPos = itEnd;
  _text = string(itCurrPos, itEnd);
}

string TextToken::output() {
  return _text;
}

void TextToken::clear() {
  _text.empty();
}

/*
 IFToken
*/
IFToken::IFToken(oTemplate& tpl): iBlockToken(tpl), _ifBlock(NULL), _elseBlock(NULL) { }

IFToken::~IFToken() {
  if (_ifBlock) {
    delete _ifBlock;
  }
  if (_elseBlock) {
    delete _elseBlock;
  }
}

void IFToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  _ifBlock = new iBlockToken(getTemplate());

  string::iterator itData = itCurrPos;
  bool foundElse = false;

  itData += 3;
  parseArguments(itData, itEnd, itData);

  if (!_sectionArgs.size()) {
    throw TemplateException("Syntax error. The if token should have got least one argument.");
  }
  if (itData == itEnd || *itData != '}') {
    throw TemplateException("Syntax error. The if token bad token end.");
  }
  itData++;
  int pr = getTemplate()->getParser()->parse(_ifBlock, itData, itEnd, "else", itPos, allowCreateTokens);
  if (pr == TemplateParser::tplEndTokenFound) {
    return;

  } else if (pr == TemplateParser::tplStopTokenFound) { //znaleziono stopowy token czyli else(if)
    _elseBlock = new iBlockToken(getTemplate());

    int nt = getTemplate()->getParser()->parse(_elseBlock, ++itPos, itEnd, "", itPos, allowCreateTokens); //parsowanie kolejnych elementow
    if (nt == TemplateParser::tplEndTokenFound) {
      return;
    }
    delete _elseBlock;
    _elseBlock = NULL;
    return;
  }

  delete _ifBlock;
  _ifBlock = NULL;
  throw TemplateException("Syntax error. If does not have an end token");
}

void IFToken::add(iTemplateToken* token) {
  return;
}

iTemplateToken* IFToken::get(UINT id) {
  return NULL;
}

bool IFToken::remove(UINT id){
  return true;
}

UINT IFToken::count() {
  return 0;
}

void IFToken::clear() {
  if (_ifBlock) {
    _ifBlock->clear();
  }
  if (_elseBlock) {
    _elseBlock->clear();
  }
}

string IFToken::output() {
  bool arg = false;

  for (iSectionToken::tSectionArgs::iterator it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    arg |= (*it)->param->output()->getBool();
  }
  if (arg) {
    if (_ifBlock) {
      return _ifBlock->output();
    }
  } else {
    if (_elseBlock) {
      return _elseBlock->output();
    }
  }
  return "";
}

/*
 UnLessToken
*/

iSectionToken::enSectionType UnLessToken::getSectionType() {
  return tUnnamed;
}

void UnLessToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  string::iterator itData = itCurrPos;

  itData += 7;
  parseArguments(itData, itEnd, itData);

  if (!_sectionArgs.size()) {
    throw TemplateException("Syntax error. The Unless token powinien miec przynajmniej jeden argument.");
  }
  if (itData == itEnd || *itData != '}') {
    throw TemplateException("Syntax error. The Unless token bledne zakonczenie tokena.");
  }

  int pr = getTemplate()->getParser()->parse(this, ++itData, itEnd, "/unless", itPos, allowCreateTokens);

  if (pr == TemplateParser::tplEndTokenFound) {
  } else if (pr == TemplateParser::tplStopTokenFound){
  } else if (pr == TemplateParser::tplParseOK) {
  }
}

string UnLessToken::output() {
  string out;

  bool arg = false;

  for (iSectionToken::tSectionArgs::iterator it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    arg |= (*it)->param->output()->getBool();
  }
  if (!arg) {
    tTokenList::iterator it = _token.begin();
    tTokenList::iterator itEnd = _token.end();

    while(it != itEnd) {
      out += (*it)->output();
      it++;
    }
  }
  return out;
}

/*
 ArgumentToken
*/

ArgumentToken::ArgumentToken(oTemplate& tpl): iTemplateToken(tpl) {
  _param = new TemplateParam;
}

ArgumentToken::~ArgumentToken() {
  delete _param;
}

void ArgumentToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  itCurrPos += 1;
  getTemplate()->getParser()->parseParam(_param, itCurrPos, itEnd, itCurrPos);
  if (itCurrPos == itEnd || *itCurrPos != '}') {
    throw TemplateException("Syntax error. Nieoczekiwane zakoñczenie tokena.");
  }
  itPos = itCurrPos;
}

string ArgumentToken::output() {
  return _param->output()->getString();
}

void ArgumentToken::clear() {
  _param->clear();
}

/*
 IncludeToken
*/

IncludeToken::IncludeToken(oTemplate& tpl): iSectionToken(tpl) {
  _includeTpl = NULL;
}

IncludeToken::~IncludeToken() {
  clear();
}

iSectionToken::enSectionType IncludeToken::getSectionType() {
  return tNamed;
}

void IncludeToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  string::iterator itData = itCurrPos;

  itData += 8;

  parseArguments(itData, itEnd, itData);

  if (!_sectionArgs.size()) {
    throw TemplateException("Syntax error. The Unless token powinien miec przynajmniej jeden argument.");
  }
  if (itData == itEnd || *itData != '}') {
    throw TemplateException("Syntax error. The Unless token bledne zakonczenie tokena.");
  }
  iSectionToken::tSectionArgs::iterator it;
  for (it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    if (*(*it)->name == "file") {
      break;
    }
  }
  if (it == _sectionArgs.end()) {
    throw TemplateException("Syntax error. File directive does not found.");
  }

  itPos = itData;
}

string IncludeToken::output() {
  clear();
  iSectionToken::tSectionArgs::iterator it;
  for (it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    if (*(*it)->name == "file") {
      break;
    }
  }

  _includeTpl = oTemplate(new Template((*it)->param->output()->getString()));
  getTemplate()->getParser()->parse(_includeTpl);
  if (_includeTpl && _includeTpl->loaded()) {
    return _includeTpl->output();
  } else {
    return "";
  }
}

void IncludeToken::clear() {
  _includeTpl = NULL;
}

iSectionToken::enSectionType SetToken::getSectionType() {
  return tNamed;
}

void SetToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  string::iterator itData = itCurrPos;

  itData += 5;

  parseArguments(itData, itEnd, itData);

  if (!_sectionArgs.size()) {
    throw TemplateException("Syntax error. The Set token should have at least one token.");
  }
  if (itData == itEnd || *itData != '}') {
    throw TemplateException("Syntax error. The Set token bad end of token.");
  }

  itPos = itData;
}

string SetToken::output() {
  for (tSectionArgs::iterator it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    if (!GlobalsManager::get()->hasVariable(*(*it)->name)) {
      throw TemplateException("variable not found.");
    } else if (!GlobalsManager::get()->isWritableVariable(*(*it)->name)){
      throw TemplateException("read only var.");
    } else {
      GlobalsManager::get()->setVariable(*(*it)->name, (*it)->param->output());
    }
  }
  return "";
}

void SetToken::clear() {
}