/**
  *  Template parsing class handler
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 24 $
  *  @modifiedby   $LastChangedBy: dulek000 $
  *  @lastmodified $Date: 2006-12-16 15:09:18 +0100 (So, 16 gru 2006) $
  */

#include "stdafx.h"
#include "TplHandler.h"
#include "Helpers.h"

TplHandler::TplHandler(const string& tplExt) {
  setTplExt(tplExt);
}

void TplHandler::addTplDir(const string& dir, bool asInclude) {
  if (!dir.size()) return;

  string tplDir = dir;
  tplDir = unifyPath(tplDir);
  tplDir = Helpers::ltrim(tplDir, ".\\");

  if (asInclude) {
    addIncludeDir(tplDir);
  }
  tplDirs.push_back(tplDir);
}

String TplHandler::runFunc(const string& name, udf_fn_param& params) {
  udf_fn* func = getUdfFactory()->get(name);
  func->param(params);
  func->handler();
  return func->result();
}

String TplHandler::runFunc(const string& name, const StringRef& param1) {
  udf_fn* func = getUdfFactory()->get(name);
  func->param(param1);
  func->handler();
  return func->result();
}

String TplHandler::runFunc(const string& name, const StringRef& param1, const StringRef& param2) {
  udf_fn* func = getUdfFactory()->get(name);
  func->param(param1, param2);
  func->handler();
  return func->result();
}

String TplHandler::runFunc(const string& name, const StringRef& param1, const StringRef& param2, const StringRef& param3) {
  udf_fn* func = getUdfFactory()->get(name);
  func->param(param1, param2, param3);
  func->handler();
  return func->result();
}

std::string TplHandler::getTplDir(const char* tplName) {
  if (!tplDirs.size()) return "";

  string fileName = tplName;
  fileName += "." + tplExt;

  for (tTplDirs::iterator it = tplDirs.begin(); it != tplDirs.end(); it++) {
    if (fileExists(((*it) + "/" + fileName).c_str())) return *it;
  }
  throw std::logic_error("Cannot find file '" + fileName + "'.");
}

std::string TplHandler::getTplPath(const char* tplName) {
  string fullPath = getTplDir(tplName) + "/";

  fullPath += tplName;
  fullPath += ".";
  fullPath += tplExt;

  return fullPath;
}

String TplHandler::getTpl(const char* tplName) {
  loader_base loader;
  loader.load_file(getTplPath(tplName));

  return loader.get_data();
}

void TplHandler::bindStdFunctions() {
  using namespace template_parser_std_fn_ns;

  bindUdf("numFormat", new num_format);
  bindUdf("urlEscape", new urlescape);
  bindUdf("htmlEscape", new htmlescape);
  bindUdf("hrefParam", new href_param);
  bindUdf("formParam", new form_param);

  bindUdf("inSet?", new value_in_set);
  bindUdf("isEmail?", new udf_is_email);
  bindUdf("isAlpha?", new udf_is_alpha);
  bindUdf("isAlphaNum?", new udf_is_alnum);
  bindUdf("isNum?", new udf_is_num);
  bindUdf("isInt?", new udf_is_int);
  bindUdf("isFloat?", new udf_is_float);
  bindUdf("isTrue?", new istrue);
}

String TplHandler::parseException(const exception &e) {
  String result, exceptionString;

  param_data data(param_data::HASH);
  data.hash_insert_new_var("reason", e.what());

  exceptionString = 
    "<div class=\"exception\">"
      "<b>Exception caught</b> (<TMPL_var htmlEscape(reason)>) !"
    "</div>";

  try {
    result = parseTpl(&data, "exception");
  } catch(...) { 
    try {
      result = parseString(&data, exceptionString);
    } catch(const exception &e2) {
      result = "It can't be worse - exception caught while parsing exceptionString (";
      result += e2.what();
      result += ")<br/>";
    }
  }
  return result;
}

String TplHandler::parseString(param_data* data, const StringRef& text) {
  template_text parser(getUdfFactory());

  // Set allowed list of catalogs to include
  parser.set_include_dir(includeDirs);
  // Parse the template
  parser.parse(text);
  // We impose parameters on a pattern
  parser.param(data);

  return parser.output();
}

String TplHandler::parseTpl(param_data* data, const char* tplName) {
  if (kPath.size()) {
    data->hash_insert_new_var("tplPath", kPath + getTplDir(tplName));
  }
  return parseString(data, getTpl(tplName));
}

template_ret_type tplParser::parse_block(string::const_iterator itmData, string::const_iterator itmDataEnd)
{
  template_ret_type sReturnType;
  unsigned int iPosition = 0;
  string::const_iterator itmRollBackPos = itmData;
  e_token_type eTokenType = TMPL_TEXT;
  string sTextSection;

  bool bInToken     = false;
  bool inCloseToken = false;
  bool bParseFlag   = true;
  bool bDoRollback  = false;
  bool bEndToken    = false;
  bool bPrevBracket = false;

  while (itmData != itmDataEnd && bParseFlag)
  {
    if (*itmData == '{' && *(itmData + 1) == '{')
    {
      bPrevBracket = true;
      iPosition = 1;
      if (bInToken || inCloseToken)
      {
        if (bPrevBracket) { sTextSection.append(itmRollBackPos, itmData);       }
        else              { sTextSection.append(itmRollBackPos, (itmData - 1)); }
      }
      else { bInToken = true; }
      itmRollBackPos = itmData;
    }
    else { bPrevBracket = false; }
    if (bInToken)
    {
      switch (iPosition)
      {
        case 1:
        case 2:
          if (*itmData != '{') {
            bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos);
          }
          iPosition++;
          break;
        case 3:
          if      (*itmData == '$') { eTokenType = TMPL_VAR;     iPosition++; }
          else if (*itmData == 'i') { eTokenType = TMPL_IF;      iPosition++; }
          else if (*itmData == 'e') { eTokenType = TMPL_ELSE;    iPosition++; }
          else if (*itmData == 'u') { eTokenType = TMPL_UNLESS;  iPosition++; }
          else if (*itmData == 'b') { eTokenType = TMPL_BREAK;   iPosition++; }
          else if (*itmData == 'c') { eTokenType = TMPL_COMMENT; iPosition++; }
          else if (*itmData == 'f') { eTokenType = TMPL_UDF;     iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
        case 4:
          if      (eTokenType == TMPL_VAR)      { iPosition++; }
          else if (*itmData == 'a' && eTokenType == TMPL_ELSE)      { eTokenType = TMPL_LOOP; iPosition++; }
          else if (*itmData == 'f' && eTokenType == TMPL_IF)        { iPosition++; }
          else if (*itmData == 'l' && eTokenType == TMPL_ELSE)      { iPosition++; }
          else if (*itmData == 'n' && eTokenType == TMPL_UNLESS)    { iPosition++; }
          else if (*itmData == 'u' && eTokenType == TMPL_UDF)       { iPosition++; }
          else if (*itmData == 'n' && eTokenType == TMPL_IF)        { eTokenType = TMPL_INCLUDE; iPosition++; }
          else if (*itmData == 'r' && eTokenType == TMPL_BREAK)     { iPosition++; }
          else if (*itmData == 'o' && eTokenType == TMPL_COMMENT)   { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
      }
      // TMPL_VAR
      if (eTokenType == TMPL_VAR && iPosition > 3)
      {
        insert_text_block(sTextSection);
        parse_param_string(iPosition, TMPL_VAR, itmData, itmDataEnd, itmRollBackPos);
        bEndToken = true; bInToken = false; iPosition = 0;
      }
      // TMPL_IF
      else if (eTokenType == TMPL_IF && iPosition > 4)
      {
        insert_text_block(sTextSection);
        parse_param_string(iPosition, TMPL_IF, itmData, itmDataEnd, itmRollBackPos);
        bEndToken = true; bInToken = false; iPosition = 0;
      }
      // TMPL_ELSE
      else if (eTokenType == TMPL_ELSE && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 's') { iPosition++; }
          else                 { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'e') { iPosition++; }
          else                 { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == '}') { iPosition++; }
          else                 { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == '}') { bInToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else                 { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
      // TMPL_UNLESS
      else if (eTokenType == TMPL_UNLESS && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'l')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'e')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 's')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == 's')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 8)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_UNLESS, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_LOOP
      else if (eTokenType == TMPL_LOOP && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'c')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'h')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 6)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_LOOP, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_UDF
      else if (eTokenType == TMPL_UDF && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'u')  { iPosition++; }
          else                  { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'c')  { iPosition++; }
          else                  { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 6)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_UDF, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
        else { iPosition++; }
      }
      // TMPL_INCLUDE
      else if (eTokenType == TMPL_INCLUDE && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'c')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'l')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'u')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == 'd')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9)
        {
          if (*itmData == 'e')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 9)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_INCLUDE, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_BREAK
      else if (eTokenType == TMPL_BREAK && iPosition > 4)
      {
        if (iPosition == 5)
        {
          if (*itmData == 'e')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 6)
        {
          if (*itmData == 'a')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'k')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 7)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_BREAK, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
      // TMPL_COMMENT
      else if (eTokenType == TMPL_BREAK && iPosition > 4)
      {
        if (iPosition == 5 || iPosition == 6)
        {
          if (*itmData == 'm')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'e')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == 'n')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9)
        {
          if (*itmData == 't')  { iPosition++; }
          else { bInToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition > 9)
        {
          insert_text_block(sTextSection);
          parse_param_string(iPosition, TMPL_COMMENT, itmData, itmDataEnd, itmRollBackPos);
          bEndToken = true; bInToken = false; iPosition = 0;
        }
      }
    }

    if (inCloseToken)
    {
      switch (iPosition)
      {
        case 1:
        case 2:
        case 3:
           iPosition++;
           break;
        case 4:
          if      (*itmData == 'e') { eTokenType = TMPL_LOOP;    iPosition++; }
          else if (*itmData == 'i') { eTokenType = TMPL_IF;      iPosition++; }
          else if (*itmData == 'u') { eTokenType = TMPL_UNLESS;  iPosition++; }
          else if (*itmData == 'c') { eTokenType = TMPL_COMMENT; iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
        case 5:
          if      ((*itmData == 'a') && eTokenType == TMPL_IF)        { iPosition++; }
          else if ((*itmData == 'f') && eTokenType == TMPL_IF)        { iPosition++; }
          else if ((*itmData == 'n') && eTokenType == TMPL_UNLESS)    { iPosition++; }
          else if ((*itmData == 'o') && eTokenType == TMPL_COMMENT)   { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
          break;
      }

      if (eTokenType == TMPL_IF && iPosition > 5)
      {
        if (iPosition == 6)
        {
          if (*itmData == '}') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == '}') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
      else if (eTokenType == TMPL_LOOP && iPosition > 5)
      {
        if (iPosition == 6)
        {
          if (*itmData == 'c') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'h') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == '}') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9)
        {
          if (*itmData == '}') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
      else if (eTokenType == TMPL_UNLESS && iPosition > 5)
      {
        if (iPosition == 6)
        {
          if (*itmData == 'l') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7)
        {
          if (*itmData == 'e') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 8)
        {
          if (*itmData == 's') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9)
        {
          if (*itmData == 's') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 10)
        {
          if (*itmData == '}') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 11)
        {
          if (*itmData == '}') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
      else if (eTokenType == TMPL_COMMENT && iPosition > 5)
      {
        if (iPosition == 6)
        {
          if (*itmData == 'o') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 7 || iPosition == 8)
        {
          if (*itmData == 'm') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 9)
        {
          if (*itmData == 'e') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 10)
        {
          if (*itmData == 'n') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 11)
        {
          if (*itmData == 't') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 12)
        {
          if (*itmData == '}') { iPosition++; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else if (iPosition == 13)
        {
          if (*itmData == '}') { inCloseToken = false; bParseFlag = false; iPosition = 0; bEndToken = true; }
          else { inCloseToken = false; bDoRollback = true; do_rollback_token(itmData, itmRollBackPos); }
        }
        else { iPosition++; }
      }
    }

    if(iPosition == C_TEMPLATE_MAX_TOKEN_LEN)
    {
      iPosition = 0; bInToken = false; inCloseToken = false; bDoRollback = true;
      do_rollback_token(itmData, itmRollBackPos);
    }

    if ((!bInToken || !inCloseToken) && bDoRollback)
    {
      sTextSection.append(itmRollBackPos, itmData);
      iPosition = 0;
      bDoRollback = false;
    }

    if (!bInToken && !inCloseToken && !bEndToken) { sTextSection += *itmData; }
    if (bEndToken) { bEndToken = false; }

    if      (*itmData == '\t') { iPos += iTabLength; }
    else if (*itmData == '\n') { iPos = 1; iLine++; }
    else                       { iPos++; }
    itmData++;
  }

  insert_text_block(sTextSection);

  sReturnType.token_type = eTokenType;
  sReturnType.parse_pos  = --itmData;
  sReturnType.line       = iLine;
  sReturnType.column     = iPos;

  return sReturnType;
}

void tplParser::parse_param_string(unsigned int &iPosition, const e_token_type &eFoundToken, string::const_iterator & itmData, string::const_iterator itmDataEnd, string::const_iterator itmRollBackPos)
{
  using std::string;

  stack_ref sStackRef;
  sStackRef.function = NULL;

  bool bIsEOP       = false;
  bool bInParam     = false;
  bool bIsVariable  = false;
  bool bIsQuoted    = false;
  bool bFunctionSet = false;

  bool bLeftBracketFound  = false;
  bool bRightBracketFound = false;

  char chQuote = '\0';

  string sParam;
  string sFunctionName;
  string::const_iterator itsParamBegin = itmData;
  string::const_iterator itsParamEnd   = itmData;

  while (itmData != itmDataEnd)
  {
    if (bInParam)
    {
      if (bIsVariable)
      {
        if (*itmData == ' ' || *itmData == '\t' || *itmData == '\r' || *itmData == '\n' || *itmData == ',')
        {
          bIsEOP = true;
          sParam.assign(itsParamBegin, itmData);
        }
        else if (*itmData == '}')
        {
          if (bLeftBracketFound && !bRightBracketFound) { throw std::logic_error("Unbalanced left bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }

          bIsEOP = true;
          sParam.assign(itsParamBegin, itmData);
        }
        else if (*itmData == '(')
        {
          if (!bLeftBracketFound) { bLeftBracketFound = true; bIsEOP = true; sParam.assign(itsParamBegin, itmData); }
          else { throw std::logic_error("Extra left bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
        }
        else if (*itmData == ')')
        {
          if (!bRightBracketFound && bLeftBracketFound) { bRightBracketFound = true; bIsEOP = true; sParam.assign(itsParamBegin, itmData); }
          else { throw std::logic_error("Unbalanced right bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
        }
        else if (*itmData == '\'' || *itmData == '"') { throw std::logic_error("Syntax error in parameter name at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
      }
      else
      {
        if (bIsQuoted)
        {
          itsParamEnd = itmData;
          itsParamEnd --;

          if (*itmData == chQuote) { sParam.append(itsParamBegin, itsParamEnd); itsParamBegin = itmData; }
          else
          {
            sParam.append(itsParamBegin, itsParamEnd);
            if      (*itmData == 'n') { sParam += '\n'; }
            else if (*itmData == 'r') { sParam += '\r'; }
            else if (*itmData == 't') { sParam += '\t'; }
            else if (*itmData == 'b') { sParam += '\b'; }
            else if (*itmData == 'a') { sParam += '\a'; }
            itsParamBegin = itmData;
            ++itsParamBegin;
          }
        }
        else if ((*itmData == '\'' || *itmData == '"') && (*itmData == chQuote)) { bIsEOP = true; sParam.append(itsParamBegin, itmData); }
      }
    }
    else
    {
      if (*itmData == '\'' || *itmData == '"') { chQuote = *itmData; bInParam = true; bIsVariable = false; itsParamBegin = itmData; itsParamBegin++; }
      else if (*itmData == '(')
      {
        if (!bLeftBracketFound) { bLeftBracketFound = true; }
        else { throw std::logic_error("Extra left bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
      }
      else if (*itmData == ')')
      {
        if (!bRightBracketFound && bLeftBracketFound) { bRightBracketFound = true; }
        else { throw std::logic_error("Unbalanced right bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
      }
      else if (*itmData != ' ' && *itmData != '\t' && *itmData != '\r' && *itmData != '\n' && *itmData != ',')
      {
        bInParam = true; bIsVariable = true; itsParamBegin = itmData;
      }
    }
    if (bIsEOP)
    {
      if (bLeftBracketFound)
      {
        if (!bFunctionSet)
        {
          sFunctionName.assign(sParam);
          sParam.erase();
          sStackRef.function = NULL;
          try { sStackRef.function = pFactory -> get(sFunctionName); }
          catch(std::exception &e) { throw std::logic_error("Line " + d2str<int>(iLine) + " column " + d2str<int>(iPos) + ": " + e.what()); }
          bFunctionSet = true;
        }
        else { sStackRef.function_parameters.push_back(function_param_data(sParam, bIsVariable)); sParam.erase(); }
      }
      else { sStackRef.function_parameters.push_back(function_param_data(sParam, bIsVariable)); sParam.erase(); }

      bIsEOP = false; bInParam = false; bIsVariable = false;
    }
    if (*itmData == '\\')
    {
      if (bIsVariable || !bInParam) { throw std::logic_error("Quoting allowed only in constants at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
      if (!bIsQuoted) { bIsQuoted = true; }
      else            { bIsQuoted = false; }
    }
    else { bIsQuoted = false; }

    if (iPosition == C_TEMPLATE_MAX_TOKEN_LEN)
    {
      iPosition = 0;
      do_rollback_token(itmData, itmRollBackPos);
    }

    if      (*itmData == '\t') { iPos += iTabLength; }
    else if (*itmData == '\n') { iPos = 1; iLine++; }
    else                       { iPos++; }

    if ((bIsVariable || !bInParam)  && *itmData == '>')
    {
      if (bLeftBracketFound && !bRightBracketFound) { throw std::logic_error("Unbalanced left bracket at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
      break;
    }

    if (bRightBracketFound && *itmData != ' ' && *itmData != '\t' && *itmData != ')' && *itmData != '}') { throw std::logic_error("Syntax error at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }

    iPosition ++;
    itmData ++;
  }

  if (sStackRef.function_parameters.size() == 0) { throw std::logic_error("Need at least one parameter at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }

  if (!bFunctionSet)
  {
    if (sStackRef.function_parameters.size() != 1 && eFoundToken != TMPL_LOOP && eFoundToken != TMPL_DECLARE) { throw std::logic_error("Only one parameter are allowed at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
    sFunctionName.assign(sStackRef.function_parameters[0].param);
  }
  sStackRef.elem_name.assign(sFunctionName);
  template_ret_type sReturnType;
  switch (eFoundToken)
  {
    case TMPL_VAR:
      if (sStackRef.function_parameters.size() != 1) { throw std::logic_error("Only one parameter are allowed for TMPL_var at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
      sStackRef.elem_name.assign(sStackRef.function_parameters[0].param);

      sStackRef.template_elem = new template_var(sStackRef.function);
      sStackRef.function      = NULL;
      sReturnType.line        = iLine;
      sReturnType.column      = iPos;
      sReturnType.parse_pos   = itmData;
      sReturnType.token_type  = TMPL_VAR;
      break;

    case TMPL_IF:
      sStackRef.template_elem = new template_if(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars, true, vIncludeDir, pLoaderBase);
      sReturnType = sStackRef.template_elem -> parse_block(++itmData, itmDataEnd);
      break;

    case TMPL_UNLESS:
      sStackRef.template_elem = new template_if(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars, false, vIncludeDir, pLoaderBase);
      sReturnType = sStackRef.template_elem -> parse_block(++itmData, itmDataEnd);
      break;

    case TMPL_LOOP:
      {
        if (bFunctionSet) { throw std::logic_error("Cannot use TMPL_loop with function at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
        bool bMLoopContextVars = bLoopContextVars;
        bool bMGlobalVars      = bGlobalVars;

        std::vector<function_param_data>::iterator itvFunctionParamData = sStackRef.function_parameters.begin();
        if (bLoopContextVars)
        {
          while (itvFunctionParamData != sStackRef.function_parameters.end())
          {
            if (itvFunctionParamData -> param == "__NO_CONTEXT_VARS__") { bMLoopContextVars = false; break; }
            itvFunctionParamData++;
          }
        }
        else
        {
          while (itvFunctionParamData != sStackRef.function_parameters.end())
          {
            if (itvFunctionParamData -> param == "__CONTEXT_VARS__") { bMLoopContextVars = true; break; }
            itvFunctionParamData++;
          }
        }

        itvFunctionParamData = sStackRef.function_parameters.begin();
        if (bGlobalVars)
        {
          while (itvFunctionParamData != sStackRef.function_parameters.end())
          {
            if (itvFunctionParamData -> param == "__NO_GLOBAL_VARS__") { bMGlobalVars = false; break; }
            itvFunctionParamData++;
          }
        }
        else
        {
          while (itvFunctionParamData != sStackRef.function_parameters.end())
          {
            if (itvFunctionParamData -> param == "__GLOBAL_VARS__") { bMGlobalVars = true; break; }
            itvFunctionParamData++;
          }
        }
        sStackRef.elem_name.assign(sStackRef.function_parameters[sStackRef.function_parameters.size() - 1].param);
        sStackRef.template_elem = new template_loop(pFactory, iLine, iPos, iTabLength, bDebug, bStrict, bMLoopContextVars, bMGlobalVars, vIncludeDir, pLoaderBase);
        sReturnType = sStackRef.template_elem -> parse_block(++itmData, itmDataEnd);
      }

      break;

    case TMPL_UDF:
      sStackRef.template_elem = new template_udf(sStackRef, bGlobalVars);
      sReturnType.line        = iLine;
      sReturnType.column      = iPos;
      sReturnType.parse_pos   = itmData;
      sReturnType.token_type  = TMPL_UDF;
      break;

    case TMPL_INCLUDE:
      {
        if (bFunctionSet) { throw std::logic_error("Cannot use TMPL_include with function at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
        string sIncludeFile;
        check_include_file(sStackRef.function_parameters[0].param, sIncludeFile);

        if (!pLoaderBase)
        {
          pLoaderBase = new loader_base();
          bSelfCreatedLoader = true;
        }
        pLoaderBase -> load_file(sIncludeFile);
        std::string &sTemplate = pLoaderBase -> get_data();

        sStackRef.template_elem = new template_include(pFactory, iTabLength, bDebug, bStrict, bLoopContextVars, bGlobalVars, vIncludeDir, pLoaderBase);
        sStackRef.template_elem -> parse_block(sTemplate.begin(), sTemplate.end());

        sReturnType.line        = iLine;
        sReturnType.column      = iPos;
        sReturnType.parse_pos   = itmData;
        sReturnType.token_type  = TMPL_INCLUDE;
      }
      break;

    case TMPL_DECLARE:
      if (bFunctionSet) { throw std::logic_error("Cannot use TMPL_declare with function at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos)); }
      sStackRef.template_elem = new template_declare(sStackRef);
      sReturnType.line        = iLine;
      sReturnType.column      = iPos;
      sReturnType.parse_pos   = itmData;
      sReturnType.token_type  = TMPL_DECLARE;
      break;

    case TMPL_BREAK:
      sStackRef.template_elem = new template_break(sStackRef, bGlobalVars);
      sReturnType.line        = iLine;
      sReturnType.column      = iPos;
      sReturnType.parse_pos   = itmData;
      sReturnType.token_type  = TMPL_BREAK;
      break;
    default:
      throw std::logic_error("Ouch! This should not happened at line " + d2str<int>(iLine) + " column " + d2str<int>(iPos));
  }

  iLine = sReturnType.line;
  iPos  = sReturnType.column;

  if (eFoundToken != sReturnType.token_type)
  {
    delete sStackRef.template_elem;
    fatal_parsing_error(eFoundToken, sReturnType.token_type);
  }

  vStack.push_back(sStackRef);
  itmData = sReturnType.parse_pos;
}