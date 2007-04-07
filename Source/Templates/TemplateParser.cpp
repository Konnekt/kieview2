#include "stdafx.h"

#include "TemplateParser.h"
#include "TemplateTokens.h"
#include "iBlockToken.h"
#include "TemplateParam.h"
#include "TemplateValue.h"
#include "Template.h"
#include "iTemplateVar.h"

iTemplateToken* TemplateParser::getToken(int type, iBlockToken* token) {
  if (type == TextToken::T_TEXT) {
    return new TextToken(this, token);
  } else if (type == UnLessToken::T_UNLESS) {
    return new UnLessToken(this, token);
  } else if (type == IFToken::T_IF) {
    return new IFToken(this, token);
  } else if (type == ArgumentToken::T_ARGUMENT) {
    return new ArgumentToken(this, token);
  } else if (type == IncludeToken::T_INCLUDE) {
    return new IncludeToken(this, token);
  } else if (type == SetToken::T_SET) {
    return new SetToken(this, token);
  }
  return NULL;
}

int TemplateParser::getType(string& text) {
  if (text == "if") {
    return IFToken::T_IF;
  } else if (text == "set") {
    return SetToken::T_SET;
  } else if (text == "unless") {
    return UnLessToken::T_UNLESS;
  } else if (text == "include") {
    return IncludeToken::T_INCLUDE;
  } else if (text[0] == '$' || text[0] == '-' || text[0] == 'f' || text[0] == 't' || /*(text[0] >= 'a' && text[0] <= 'z') ||*/ text[0] == '!'
    || /*(text[0] >= 'A' && text[0] <= 'Z') || */(text[0] >= '0' && text[0] <= '9') || text[0] == '(' || text[0] == ')'
    || text[0] == '\"' || text[0] == '\'')
  {
    return ArgumentToken::T_ARGUMENT;
  }
  return 0;
}

void TemplateParser::parse(oTemplate& tpl) {
  tpl->clear();
  tpl->_parser = this;
  tpl->_token = new iBlockToken(this, NULL);
  string::iterator it;
  try {
    parse(tpl->_token, tpl->_data.begin(), tpl->_data.end(), "", it, true);
  } catch (const TemplateException& ex) {
    throw;
  }
  if (it != tpl->_data.end()) {
    throw TemplateException("Syntax error. Bad template declaration.");
  }
}

TemplateParser::enParseRes TemplateParser::parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
  bool inToken = false;
  string::iterator itTokenPos = itCurrPos;
  bool allow = false;

  while(itCurrPos != itEnd) {
    if (*itCurrPos == '{' && (*(itCurrPos + 1) != ' ' || *(itCurrPos + 1) != '\t') && !inToken) {
      inToken = true;

      if (itTokenPos != itCurrPos && allowCreateTokens && block) {
        iTemplateToken* pToken = getToken(TextToken::T_TEXT, block);
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
      if (!token.size()) {
        throw TemplateException("Syntax error. Token does not have name.");
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

      int type = getType(token);
      if (block && allowCreateTokens && type != 0) {
        iTemplateToken* pToken = getToken(type, block);
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
    iTemplateToken* pToken = getToken(TextToken::T_TEXT, block);
    pToken->parse(block, itTokenPos, itCurrPos, "", itPos, true);
    block->add(pToken);
  }
  itPos = itCurrPos;
  return tplParseOK;
}

void TemplateParser::parseText(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  bool backSlash = false;
  string text;
  bool apostrophe = (*itCurrPos++) == '\'';

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
      } else if (*itCurrPos == '}') {
        text += '}';
      } else if (*itCurrPos == '\\') {
        text += '\\';
      } else if (*itCurrPos == '\'') {
        text += '\'';
      } else if (*itCurrPos == '\"') {
        text += '\"';
      } else {
        //text += *itCurrPos;
        throw TemplateException(stringf("Syntax error. Invalid special character: \\%c", *itCurrPos));
      }
      backSlash = false;
    } else if (*itCurrPos == '\\') {
      backSlash = true;
    } else if ((*itCurrPos == '\"' && !apostrophe) || (*itCurrPos == '\'' && apostrophe)) {
      break;
    } else {
      text += *itCurrPos;
    }
    itCurrPos++;
  }
  if (itCurrPos == itEnd || (*itCurrPos != '\"' && !apostrophe) || (*itCurrPos != '\'' && apostrophe) || backSlash) {
    throw TemplateException("Syntax error. The text does not have end sign.");
  }
  param->add(TemplateValue(text), oper, not);
  itPos = itCurrPos + 1;
}

void TemplateParser::parseRegExp(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  bool backSlash = false;
  string text;
  text += '/';

  //expr
  while (itCurrPos != itEnd) {
    if (backSlash) {
      if (*itCurrPos == '/') {
        text += *itCurrPos;
      }
      backSlash = false;
    }
    if (*itCurrPos == '/') {
      text += *itCurrPos;
      itCurrPos++;
      break;
    } else if (*itCurrPos == '\\') {
      backSlash = true;
      text += *itCurrPos;
    } else {
      text += *itCurrPos;
    }
    itCurrPos++;
  }

  //flags
  while (itCurrPos != itEnd) {
    if (*itCurrPos >= 'a' && *itCurrPos <= 'z') {
      text += *itCurrPos;
    } else {
      break;
    }
    itCurrPos++;
  }
  if (itCurrPos == itEnd) {
    throw TemplateException("Syntax error. ....");
  }
  RegEx reg;
  reg.setPattern(text);
  param->add(TemplateValue(text, reg), oper, not);
  itPos = itCurrPos + 1;
}

void TemplateParser::parseConst(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  string text;

  while (itCurrPos != itEnd) {
    if ((*itCurrPos >= 'a' && *itCurrPos <= 'z') || (*itCurrPos >= 'A' && *itCurrPos <= 'Z') || *itCurrPos == '_' || (*itCurrPos >= '0' && *itCurrPos <= '9')) {
      text += *itCurrPos;
    } else {
      break;
    }
    itCurrPos++;
  }
  if (text == "false") {
    param->add(TemplateValue(false), oper, not);
  } else if (text == "true") {
    param->add(TemplateValue(true), oper, not);
  } else {
    throw TemplateException("Syntax error. Const value does not true/false type.");
  }
  itPos = itCurrPos;
}

void TemplateParser::parseInt(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  string intstr;
  int multipler;
  if (itCurrPos != itEnd && *itCurrPos == '-') {
    multipler = -1;
    itCurrPos++;
  } else {
    multipler = 1;
  }
  while (itCurrPos != itEnd) {
    if (*itCurrPos >= '0' && *itCurrPos <= '9') {
      intstr += *itCurrPos;
    } else {
      break;
    }
    itCurrPos++;
  }
  if (!intstr.size()) {
    throw TemplateException("Syntax error. This value does not integer type.");
  }
  param->add(TemplateValue(multipler * atoi(intstr.c_str())), oper, not);
  itPos = itCurrPos;
}

void TemplateParser::parseVar(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
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
    param->add(TemplateValue(func), oper, not);

    TemplateParam* newParam;
    bool lastComma = false;

    do {
      newParam = new TemplateParam(this, param->getBlock());
      parseParam(newParam, itCurrPos + 1, itEnd, itCurrPos);
      if ((lastComma ||(itCurrPos != itEnd && *itCurrPos == ','))&& !newParam->count()) {
        throw TemplateException("Syntax error. Too many comma signs in function: " + name);
      }
      if (newParam->count()) {
        func->addParam(newParam);
        lastComma = true;
      } else {
        delete newParam;
      }
    } while (itCurrPos != itEnd && *itCurrPos == ',');

    if (itCurrPos == itEnd || *itCurrPos != ')') {
      throw TemplateException("Syntax error. Right bracked not found in function: " + name);
    }
    itPos = itCurrPos + 1;
    return;
  } else if (!isFunc){
    param->add(TemplateValue(new TemplateVariable(name)), oper, not);
  } else {
    throw TemplateException("Syntax error. Wrong var/funcion declaration.");
  }
  itPos = itCurrPos;
}

bool TemplateParser::parseArgument(TemplateParam* param, enOperators oper, bool not, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  if (*itCurrPos == '\"' || *itCurrPos == '\'') {
    parseText(param, oper, not, itCurrPos, itEnd, itPos);
  } else if (*itCurrPos == '(') {
    TemplateParam* newParam = new TemplateParam(this, param->getBlock());
    TemplateParser::parseParam(newParam, itCurrPos + 1, itEnd, itPos);
    if (itPos == itEnd && *itPos != ')') {
      delete newParam;
      throw TemplateException("Syntax error. Right bracked in block not found.");
    }
    itPos++;
    param->add(TemplateValue(newParam), oper, not);
  } else if (*itCurrPos >= '0' && *itCurrPos <= '9' || *itCurrPos == '-') {
    parseInt(param, oper, not, itCurrPos, itEnd, itPos);
  } else if ((*itCurrPos >= 'a' && *itCurrPos <= 'z') || (*itCurrPos >= 'A' && *itCurrPos <= 'Z') || *itCurrPos == '_') {
    parseConst(param, oper, not, itCurrPos, itEnd, itPos);
  } else if (*itCurrPos == '$') {
    parseVar(param, oper, not, itCurrPos + 1, itEnd, itPos);
  } else if (*itCurrPos == '/') {
    parseRegExp(param, oper, not, itCurrPos + 1, itEnd, itPos);
  } else {
    return false;
  }
  return true;
}

enOperators TemplateParser::parseOperator(string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  string oper;

  while (itCurrPos != itEnd) {
    if (*itCurrPos == '&' || *itCurrPos == '!' || *itCurrPos == '+' || *itCurrPos == '-' || *itCurrPos == '~' || *itCurrPos == '=' || *itCurrPos == '|') {
      oper += *itCurrPos;
    } else {
      break;
    }
    itCurrPos++;
  }

  itPos = itCurrPos;
  if (oper == "&&") {
    return opAnd;
  } else if (oper == "||") {
    return opOr;
  } else if (oper == "==") {
    return opComp;
  } else if (oper == "!=") {
    return opDiff;
  } else if (oper == "=~") {
    return opRegExComp;
  } else if (oper == "!~") {
    return opRegExDiff;
  } else if (oper == "+") {
    return opPlus;
  } else if (oper == "-") {
    return opMinus;
  } else if (oper == "!") {
    return opNot;
  } else if (!oper.size()){
    return opNone;
  } else {
    throw TemplateException("Syntax error. Invalid operator: " + oper);
  }
}

TemplateParser::enParseParamRes TemplateParser::parseParam(TemplateParam* param, string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos) {
  enOperators lastOperator = opNone;
  enOperators notOperator;
  bool not = false;
  bool isArgument = true;

  while (itCurrPos != itEnd) {
    if (*itCurrPos == ' ' || *itCurrPos == '\t' || *itCurrPos == '\n') {
      itCurrPos++;
    } else {
      if (isArgument) {
        notOperator = parseOperator(itCurrPos, itEnd, itCurrPos);
        if (notOperator == opNone || notOperator == opNot) {
          not = notOperator == opNot;
        } else if (notOperator == opMinus) {
          itCurrPos = itCurrPos - 1;
        } else {
          throw TemplateException("Syntax error. First parameter is operator.");
        }
        if(!parseArgument(param, lastOperator, not, itCurrPos, itEnd, itCurrPos)) {
          itPos = itCurrPos;
          return paramError;
        }
        isArgument = false;
      } else {
        lastOperator = parseOperator(itCurrPos, itEnd, itCurrPos);
        if (lastOperator == opNot) {
          throw TemplateException("Syntax error. Invalid operator.");
        }
        isArgument = true;
      }
    }
  }
  if (isArgument && lastOperator != opNone) {
    throw TemplateException("Syntax error. The operator ending a code.");
  }
  itPos = itCurrPos;
  return paramParseOK;
}

int f() {
  TemplateParser*p =new TemplateParser;
  oTemplate tpl = new FileTemplate("c:\\psiak.tpl");
  try {
  p->parse(tpl);
  OutputDebugStringA(tpl->output().c_str());
  } catch(const TemplateException& ex) {
    OutputDebugStringA(ex.getReason().a_str());
  }
  return 0;
}

int d = f();