#include "stdafx.h"
#include "TemplateToken.h"
#include "TemplateParser.h"
#include "Template.h"

/* do skonczenia
void iSectionToken::parseArguments(string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  TemplateParam* newParam;
  bool lastComma = false;
  string name;
  UINT index = 0;

  do {
    if (getSectionType() == tNamed) {
      while(itCurrPos != itEnd) {
        if (*itCurrPos == '}') {
          throw TemplateException("Syntax error. Bad argument definition.");
        } else if (*itCurrPos == '=') {
          break;
        }
        itCurrPos++;
      }
    }
    newParam = new TemplateParam;
    TemplateParser::parseParam(newParam, itCurrPos, itEnd, itCurrPos);
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

  if (itCurrPos == itEnd || *itCurrPos != ')') {
    throw TemplateException("Syntax error. Right bracked not found in xxx");
  }
  itPos = itCurrPos + 1;
}
*/


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

void iBlockToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  TemplateParser::parse(this, itCurrPos, itEnd, stopToken, itPos, allowCreateTokens);
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
IFToken::IFToken() {
  _param = new TemplateParam;
}
IFToken::~IFToken() {
  delete _param;
}

void IFToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  iBlockToken* ifToken = new iBlockToken;
  string::iterator itData = itCurrPos;
  bool foundElse = false;

  itData += 3;
  TemplateParser::parseParam(_param, itData, itEnd, itData);
  if (itData == itEnd || *itData != '}') {
    throw TemplateException("Syntax error. brakuje znaku }.");
  }
  if (!_param->count()) {
    TemplateException("Syntax error. The if token powinien miec przynajmniej jeden argument.");
  }
  int pr = TemplateParser::parse(ifToken, itData, itEnd, "else", itPos, allowCreateTokens);
  if (pr == TemplateParser::tplEndTokenFound) {
    _blocks.push_back(ifToken);
    return;

  } else if (pr == TemplateParser::tplStopTokenFound) { //znaleziono stopowy token czyli else(if)
    _blocks.push_back(ifToken);
    iBlockToken* blockElse = new iBlockToken;

    int nt = TemplateParser::parse(blockElse, itPos, itEnd, "", itPos, allowCreateTokens); //parsowanie kolejnych elementow
    if (nt == TemplateParser::tplEndTokenFound) {
      _blocks.push_back(blockElse);
      return;
    }
    delete blockElse;
  }
  delete ifToken;
  throw TemplateException("Syntax error. If does not have an end token");
}

void IFToken::add(iTemplateToken* token) {
  if (_active == -1 || _active < _blocks.size())
    return;

  _blocks[_active]->add(token);
}

iTemplateToken* IFToken::get(UINT id) {
  if (_active == -1 || _active < _blocks.size())
    return NULL;

  return _blocks[_active]->get(id);
}

bool IFToken::remove(UINT id){
  if (_active == -1 || _active < _blocks.size())
    return false;

  return _blocks[_active]->remove(id);
}

UINT IFToken::count() {
  if (_active == -1 || _active < _blocks.size())
    return 0;

  return _blocks[_active]->count();
}

void IFToken::clear() {
  if (_active == -1 || _active < _blocks.size())
    return;

  return _blocks[_active]->clear();
}

/*
 UnLessToken
*/

UnLessToken::UnLessToken() {
  _param = new TemplateParam;
}
UnLessToken::~UnLessToken() {
  delete _param;
}

void UnLessToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  string::iterator itData = itCurrPos;

  itData += 7;
  TemplateParser::parseParam(_param, itData, itEnd, itData);
  if (itData == itEnd || *itData != '}') {
    throw TemplateException("Syntax error. Invalid token end.");
  }
  if (!_param->count()) {
    TemplateException("Syntax error. The Unless token powinien miec przynajmniej jeden argument.");
  }

  int pr = TemplateParser::parse(this, itData, itEnd, "/unless", itPos, allowCreateTokens);

  if (pr == TemplateParser::tplEndTokenFound) {
  } else if (pr == TemplateParser::tplStopTokenFound){
  } else if (pr == TemplateParser::tplParseOK) {
  }
}

string UnLessToken::output() {
  string out;
  if (_param->output().getBool()) {
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

ArgumentToken::ArgumentToken() {
  _param = new TemplateParam;
}

ArgumentToken::~ArgumentToken() {
  delete _param;
}

void ArgumentToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  itCurrPos += 1;
  TemplateParser::parseParam(_param, itCurrPos, itEnd, itCurrPos);
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

IncludeToken::IncludeToken() {
  _tpl = NULL;
}

IncludeToken::~IncludeToken() {
  clear();
}

void IncludeToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  string::iterator itData = itCurrPos;

  itData += 8;
/*  TemplateParser::parseParam(_param, itData, itEnd, itData);
  if (itData == itEnd || *itData != '}') {
    throw TemplateException("Syntax error. Invalid token end.");
  }
  if (!_param->count()) {
    TemplateException("Syntax error. The include token powinien miec argument.");
  }

  _tpl = new Template(_param->output().getString());
  */
  itPos = itCurrPos;
}

string IncludeToken::output() {
  string out;
  if (_tpl && _tpl->loaded()) {
    return _tpl->output();
  } else {
    return "";
  }
}

void IncludeToken::clear() {
  if (_tpl) {
    delete _tpl;
  }
  _tpl = NULL;
}