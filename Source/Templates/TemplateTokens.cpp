#include "stdafx.h"

#include "TemplateTokens.h"
#include "TemplateParser.h"
#include "TemplateParam.h"
#include "Template.h"

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
IFToken::IFToken(TemplateParser* parser, iBlockToken* parent): iBlockToken(parser, parent), _ifBlock(NULL), _elseBlock(NULL) { }

IFToken::~IFToken() {
  if (_ifBlock) {
    delete _ifBlock;
  }
  if (_elseBlock) {
    delete _elseBlock;
  }
}

void IFToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  _ifBlock = new iBlockToken(getParser(), getParent());

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
  int pr = getParser()->parse(_ifBlock, itData, itEnd, "else", itPos, allowCreateTokens);
  if (pr == TemplateParser::tplEndTokenFound) {
    return;

  } else if (pr == TemplateParser::tplStopTokenFound) { //znaleziono stopowy token czyli else(if)
    _elseBlock = new iBlockToken(getParser(), getParent());

    int nt = getParser()->parse(_elseBlock, ++itPos, itEnd, "", itPos, allowCreateTokens); //parsowanie kolejnych elementow
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
    arg |= (*it)->param->output().getBool();
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

  int pr = getParser()->parse(this, ++itData, itEnd, "/unless", itPos, allowCreateTokens);

  if (pr == TemplateParser::tplEndTokenFound) {
  } else if (pr == TemplateParser::tplStopTokenFound){
  } else if (pr == TemplateParser::tplParseOK) {
  }
}

string UnLessToken::output() {
  string out;

  bool arg = false;

  for (iSectionToken::tSectionArgs::iterator it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    arg |= (*it)->param->output().getBool();
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

ArgumentToken::ArgumentToken(TemplateParser* parser, iBlockToken* parent): iTemplateToken(parser, parent) {
  _param = new TemplateParam(getParser(), getParent());
}

ArgumentToken::~ArgumentToken() {
  delete _param;
}

void ArgumentToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  itCurrPos += 1;
  getParser()->parseParam(_param, itCurrPos, itEnd, itCurrPos);
  if (itCurrPos == itEnd || *itCurrPos != '}') {
    throw TemplateException("Syntax error. Nieoczekiwane zakoñczenie tokena.");
  }
  itPos = itCurrPos;
}

string ArgumentToken::output() {
  return _param->output().getString();
}

void ArgumentToken::clear() {
  _param->clear();
}

/*
 IncludeToken
*/

IncludeToken::IncludeToken(TemplateParser* parser, iBlockToken* parent): iSectionToken(parser, parent) {
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
  string path = (*it)->param->output().getString();
  for (it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    if (*(*it)->name == "inherit") {
      break;
    }
  }
  bool inherit = (it != _sectionArgs.end()) ? (*it)->param->output().getBool() : true;
  if (inherit) {
    _includeTpl = new FileTemplate(path, getParent());
  } else {
    _includeTpl = new FileTemplate(path);
  }
  for (it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    _includeTpl->addVariable(*(*it)->name, (*it)->param->output(), false);
  }
  getParser()->parse(_includeTpl);
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
  for (tSectionArgs::iterator it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    if (!getParent()->find(*(*it)->name)) {
      getParent()->setVariable(*(*it)->name, TemplateValue(), true);
    }
  }
  itPos = itData;
}

string SetToken::output() {
  iVariableManager* vm = NULL;
  for (tSectionArgs::iterator it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    if (vm = getParent()->find(*(*it)->name)) {
      vm->setVariable(*(*it)->name, (*it)->param->output());
    } else {
      getParent()->setVariable(*(*it)->name, (*it)->param->output());
    }
  }
  return "";
}

void SetToken::clear() {
}