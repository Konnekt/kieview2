/**
  *  kIEview2 Emot parsers
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @copyright    Copyright (c) 2006-2007 Micha³ "Dulek" Dulko
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 43 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-31 16:47:02 +0100 (N, 31 gru 2006) $
  */
  
#pragma once

#ifndef __EMOTSPARSERS_H__
#define __EMOTSPARSERS_H__

class EmotParser {
public:
  class WrongFormat: public ExceptionString {
  public:
    WrongFormat(const StringRef& reason): ExceptionString(reason) { }
  };
  
public:
  virtual EmotSet parse(string str) = 0;
};

class JispParser: public EmotParser {
public:
  class XMLParserException: public ExceptionString {
  public:
    XMLParserException(const StringRef& reason): ExceptionString(reason) { }
  };

public:
  JispParser() {
    this->parser.set_substitute_entities();
  }

public:
  EmotSet parse(string str);

public:
  xmlpp::DomParser parser;
};

class GGEmotParser: public EmotParser {
public:
  EmotSet parse(string str);
  
public:
  RegEx parser;
};

#endif // __EMOTSPARSERS_H__