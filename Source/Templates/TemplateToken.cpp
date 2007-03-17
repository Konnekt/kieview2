#include "stdafx.h"
#include "TemplateToken.h"
#include "TemplateParser.h"

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

void IFToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  iBlockToken* ifToken = new iBlockToken;
  string::iterator itData = itCurrPos;
  bool foundElse = false;

  while(*itData++ != '}');

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


void UnLessToken::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  string::iterator itData = itCurrPos;

  while(*itData++ != '}');
  int pr = TemplateParser::parse(this, itData, itEnd, "/unless", itPos, allowCreateTokens);
  if (pr == TemplateParser::tplEndTokenFound) {
  } else if (pr == TemplateParser::tplStopTokenFound){
  } else if (pr == TemplateParser::tplParseOK) {
  }
}

string UnLessToken::output() {
  string out;
  if (true) { //args
    tTokenList::iterator it = _token.begin();
    tTokenList::iterator itEnd = _token.end();

    while(it != itEnd) {
      out += (*it)->output();
      it++;
    }
  }
  return out;
}

