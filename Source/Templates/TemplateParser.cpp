#include "stdafx.h"
#include "TemplateParser.h"

iTemplateToken* TemplateParser::getToken(int type) {
  if (type == TextToken::T_TEXT) {
    return new TextToken;
  } else if (type == UnLessToken::T_UNLESS) {
    return new UnLessToken;
  } else if (type == IFToken::T_IF) {
    return new IFToken;
  }
  return NULL;
}

int TemplateParser::getType(string& text) {
  if (text == "unless") {
    return UnLessToken::T_UNLESS;
  } else if (text == "if") {
    return IFToken::T_IF;
  }
  return 0;
}

TemplateParser::enParseRes TemplateParser::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  bool inToken = false;
  string::iterator itTokenPos = itCurrPos;
  bool allow = false;

  while(itCurrPos != itEnd) {
    if (*itCurrPos == '{' && (*(itCurrPos + 1) != ' ' || *(itCurrPos + 1) != '\t') && !inToken) {
      inToken = true;

      if (itTokenPos != itCurrPos && allowCreateTokens && block) {
        iTemplateToken* pToken = getToken(TextToken::T_TEXT);
        pToken->parse(block, itTokenPos, itCurrPos, "", itPos, true);
        block->add(pToken);
      }
      itTokenPos = itCurrPos;
    }

    if (*itCurrPos == '}' && inToken) {
      inToken = false;
      string token(itTokenPos + 1, itCurrPos);
      int sPos = token.find(" ");
      if (sPos != string::npos) {
        token = token.substr(0, sPos);
      }
      if (token == stopToken) {
        itPos = itCurrPos;
        return tplStopTokenFound;
      }
      if (*(itTokenPos + 1) == '/') {
        itPos = itCurrPos;
        return tplEndTokenFound;
      }


      iTemplateToken* pToken = NULL;

      int type = getType(token);//token);
      if (block && allowCreateTokens && type != 0) {
        iTemplateToken* pToken = getToken(type);//, block);
        if (pToken) {
          pToken->parse(block, itTokenPos, itEnd, ("/" + token), itPos, true);
          itCurrPos = itPos;
          allow = true;
        }
        block->add(pToken);
      }
      itTokenPos = itCurrPos + 1;
      int pr = 0;
      if (!allow) {
        pr = parse((pToken && (pToken->getType() & iBlockToken::T_BLOCK)) ? (iBlockToken*)pToken : block, itCurrPos, itEnd, ("/" + token), itPos, false);
        allow = false;
      }

      if (pr == tplStopTokenFound) {
        itCurrPos = itPos;
        itTokenPos = itPos + 1;
      } else if (pr == tplEndTokenFound) {
        //ups, end token found;
        return tplEndTokenFound;
      }
    }
    itCurrPos++;
  }
  if (itTokenPos != itCurrPos && allowCreateTokens && block) {
    iTemplateToken* pToken = getToken(TextToken::T_TEXT);
    pToken->parse(block, itTokenPos, itCurrPos, "", itPos, true);
    block->add(pToken);
  }
  return tplParseOK;
}

/*
class TemplateParam {
public:
  typedef vector<iTemplateValue*> tValues;

public:
  void add(iTemplateValue* value);
  void remove(UINT id);
  UINT count():
  void clear();
  iTemplateValue* get(UINT id);
  iTemplateValue output();

protected:
  tValues _values;
};
*/