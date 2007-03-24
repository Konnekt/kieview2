#include "stdafx.h"
#include "TemplateParser.h"
#include "TemplateValue.h"

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
      if (*(itCurrPos - 1) == '\\' && *(itCurrPos - 2) != '\\') {
        itCurrPos++;
        continue;
      }
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




void parseText(TemplateParam* param, TemplateParam::enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  bool backSlash = false;
  string text;

  while (itCurrPos != itEnd) {
    if (backSlash) {
      if (*itCurrPos == 'n') {
        text += '\n';
      } else if (*itCurrPos == 'r') {
        text += '\r';
      } else if (*itCurrPos == 't') {
        text += '\t';
      } else if (*itCurrPos == 'v') {
        text += '\v';
      } else {
        text += *itCurrPos;
      }
      backSlash = false;
    } else if (*itCurrPos == '\\') {
      backSlash = true;
    } else if (*itCurrPos == '\"') {
      break;
    } else {
      text += *itCurrPos;
    }
    itCurrPos++;
  }

  if (*itCurrPos != '\"' || backSlash) {
    //err
  }
  param->add(new TemplateValue(text), oper, not);
  itPos = itCurrPos + 1;
}

void parseBool(TemplateParam* param, TemplateParam::enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  string text;

  while (itCurrPos != itEnd) {
    if ((*itCurrPos >= 'a' && *itCurrPos <= 'z') || (*itCurrPos >= 'A' && *itCurrPos <= 'Z')) {
      text += *itCurrPos;
    } else {
      break;
    }
    itCurrPos++;
  }
  if (text == "false") {
    param->add(new TemplateValue(false), oper, not);
  } else if (text == "true") {
    param->add(new TemplateValue(true), oper, not);
  } else {
    //err
  }
  itPos = itCurrPos;
}

void parseInt(TemplateParam* param, TemplateParam::enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  string intstr;

  while (itCurrPos != itEnd) {
    if (*itCurrPos >= '0' && *itCurrPos <= '9') {
      intstr += *itCurrPos;
    } else {
      break;
    }
    itCurrPos++;
  }

  param->add(new TemplateValue(atoi(intstr.c_str())), oper, not);
  itPos = itCurrPos;
}

void parseVar(TemplateParam* param, TemplateParam::enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  bool isFunc = false;
  string name;

  while (itCurrPos != itEnd) {
    if ((*itCurrPos >= '0' && *itCurrPos <= '9') || (*itCurrPos >= 'a' && *itCurrPos <= 'z') || (*itCurrPos >= 'A' && *itCurrPos <= 'Z') || *itCurrPos == '_') {
      name += *itCurrPos;
    } else if (*itCurrPos == '(') {
      isFunc = true;
      break;
    } else {
      break;
    }
    itCurrPos++;
  }

  if (isFunc) {
    TemplateFunction* func = new TemplateFunction(name);
    param->add(new TemplateValue(func), oper, not);
    TemplateParam* newParam;
    do {
      newParam = new TemplateParam;
      TemplateParser::parseParam(newParam, itCurrPos + 1, itEnd, itCurrPos);
      if (newParam->count()) {
        func->addParam(newParam);
      } else {
        delete newParam;
      }
    } while (itCurrPos != itEnd && *itCurrPos == ',');
    if (*itCurrPos != ')') {
      //err
    }
    itPos = itCurrPos + 1;
    return;
  } else if (!isFunc){
    param->add(new TemplateValue(new TemplateVariable(name)), oper, not);
  } else {
    //err
  }
  itPos = itCurrPos;
}
bool parseArgument(TemplateParam* param, TemplateParam::enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  if (*itCurrPos == '\"') {
    parseText(param, oper, not, itCurrPos + 1, itEnd, itPos);
  } else if (*itCurrPos == '(') {
    TemplateParam* newParam = new TemplateParam;
    TemplateParser::parseParam(newParam, itCurrPos + 1, itEnd, itPos);
    if (itPos == itEnd && *itPos != ')') {
      delete newParam;
      //err
    }
    itPos++;
    param->add(new TemplateValue(newParam), oper, not);
  } else if (*itCurrPos >= '0' && *itCurrPos <= '9') {
    parseInt(param, oper, not, itCurrPos, itEnd, itPos);
  } else if (*itCurrPos == 't' || *itCurrPos == 'f') {
    parseBool(param, oper, not, itCurrPos, itEnd, itPos);
  } else if (*itCurrPos == '$') {
    parseVar(param, oper, not, itCurrPos + 1, itEnd, itPos);
  } else {
    return false;
  }
  return true;
}

TemplateParam::enOperators parseOperator(string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  string oper;

  while (itCurrPos != itEnd) {
    if (*itCurrPos == '&' || *itCurrPos == '!' || *itCurrPos == '+' || *itCurrPos == '-' || *itCurrPos == '=' || *itCurrPos == '|') {
      oper += *itCurrPos;
    } else {
      break;
    }
    itCurrPos++;
  }

  itPos = itCurrPos;
  if (oper == "&&") {
    return TemplateParam::opAnd;
  } else if (oper == "||") {
    return TemplateParam::opOr;
  } else if (oper == "==") {
    return TemplateParam::opComp;
  } else if (oper == "!=") {
    return TemplateParam::opDiff;
  } else if (oper == "+") {
    return TemplateParam::opPlus;
  } else if (oper == "-") {
    return TemplateParam::opMinus;
  } else if (oper == "!") {
    return TemplateParam::opNot;
  } else {
    return TemplateParam::opNone;
  }
}


TemplateParser::enParseParamRes TemplateParser::parseParam(TemplateParam* param, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  TemplateParam::enOperators lastOperator = TemplateParam::opNone;
  TemplateParam::enOperators notOperator;
  bool not = false;
  bool da = true;

  while (itCurrPos != itEnd) {
    if (*itCurrPos == ' ' || *itCurrPos == '\t') {
      itCurrPos++;
    } else {
      if (da) {
        notOperator = parseOperator(itCurrPos, itEnd, itCurrPos);
        if (notOperator == TemplateParam::opNone || notOperator == TemplateParam::opNot) {
          not = notOperator == TemplateParam::opNot;
        }
        if(!parseArgument(param, lastOperator, not, itCurrPos, itEnd, itCurrPos)) {
          itPos = itCurrPos;
          return paramError;
        }
        da = false;
      } else {
        lastOperator = parseOperator(itCurrPos, itEnd, itCurrPos);
        if (lastOperator == TemplateParam::opNot) {
          //err
        }
        da = true;
      }
    }
  }
  if (da && lastOperator != TemplateParam::opNone) {
    //err
  }
  itPos = itCurrPos;
  return paramParseOK;
}
