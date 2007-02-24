/**
  *  kIEview2 Emot parser class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @copyright    Copyright (c) 2006-2007 Micha� "Dulek" Dulko
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 43 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-31 16:47:02 +0100 (N, 31 gru 2006) $
  */
  
#pragma once

#ifndef __EMOTPARSER_H__
#define __EMOTPARSER_H__

#include <fstream>

#include "iPackageHandler.h"
#include "Emots.h"
#include "oZip.h"

/*
 * Base emot definition parser
 */
class EmotParserException: public ExceptionString {
public:
  EmotParserException(const StringRef& reason): ExceptionString(reason) { }
};
class WrongFormat: public EmotParserException {
public:
  WrongFormat(const StringRef& reason): EmotParserException(reason) { }
};

/*
 * JISP emot definition parser
 */
class JispParser: public iPackageParser {
public:
  class XMLParserException: public EmotParserException {
  public:
    XMLParserException(const StringRef& reason): EmotParserException(reason) { }
  };

public:
  void setDefinitionFilter(FindFileFiltered& files) {
    files.setMask(files.found().getDirectory() + "*.jisp");
  }
  iPackage* parse(const FindFile::Found& defFile);
};

/*
 * GG emot definition parser
 */
class GGParser: public iPackageParser {
public:
  void setDefinitionFilter(FindFileFiltered& files) {
    files.setMask(files.found().getDirectory() + "emots.txt");
  }
  iPackage* parse(const FindFile::Found& defFile);
};

#endif // __EMOTPARSER_H__