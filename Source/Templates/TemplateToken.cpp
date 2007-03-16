#include "stdafx.h"
#include "TemplateToken.h"

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
  parse(this, itCurrPos, itEnd, stopToken, itPos, allowCreateTokens);
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
