/**
  *  Helpers
  *
  *  Licensed under The GNU Lesser General Public License
  *  Redistributions of files must retain the above copyright notice.
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kaway2/ kAway2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  *  @license      http://creativecommons.org/licenses/LGPL/2.1/
  */

#pragma once

#ifndef __HELPERS_H__
#define __HELPERS_H__

/*
 * Int64 to string conversion
 */

std::string i64tostr(__int64 value, int radix = 10);

/*
 *  Bool -> Human readable string
 */

char * btoa(bool value);
String btos(bool value);

/*
 *  NULL -> Human readable string
 */

char * nullChk(const StringRef& value);
char * nullChk(char * value);

/*
 *  Logging helpers, just a little short cut ;>
 */

void log(enDebugLevel level, const char * format, va_list ap);
void log(const char * format, ...);
void logDebug(const char * format, ...);

/*
 *  Various helpers
 */

namespace Helpers {
  void xor1_encrypt(const unsigned char* key, unsigned char* data, unsigned int size);
  void xor1_decrypt(const unsigned char* key, unsigned char* data, unsigned int size);

  String icon16(int ico);
  String icon32(int ico);

  String trunc(StringRef txt, int limit, const StringRef& suffix = "...");

  string rtrim(string txt, const string& chars = " ");
  string ltrim(string txt, const string& chars = " ");
  string trim(string txt, const string& chars = " ");

  int getPluginsGroup();
  int pluginExists(int net, int type = IMT_ALL);
  const char * getPlugName(int plugID);

  void UIActionCall(int group, int act, int cntID = 0);
  void touchConfigWnd();

  #ifdef SHARED_TABLETKA_H
  bool isMsgWndOpen(int cntID);
  #endif

  #ifdef __STAMINA_TIME64__
  bool isToday(Date64 date);
  #endif

  void showKNotify(char * text, int ico);
  int findParentAction(int group, int id);

  void addItemToHistory(cMessage* msg, int cnt, const char * dir, const StringRef& name, int session = 0);

  void chgBtn(int group, int id, int cnt, const char * name = 0, int ico = 0, int flags = -1);
  void chgBtn(int group, int id, const char * name, int ico = 0, int flags = 0);
}

#endif // __HELPERS_H__