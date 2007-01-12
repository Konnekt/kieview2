/**
  *  Message class
  *
  *  Licensed under The GNU Lesser General Public License
  *  Redistributions of files must retain the above copyright notice.
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kaway2/ kAway2 plugin SVN Repo
  *  @version      $Revision: 58 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-11-12 19:22:00 +0100 (N, 12 lis 2006) $
  *  @license      http://creativecommons.org/licenses/LGPL/2.1/
  */

#pragma once

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <konnekt/sms.h>
#include "Helpers.h"

class Message : public iObject {
public:
  /*
   * Class version
   */
	STAMINA_OBJECT_CLASS_VERSION(Message, iObject, Version(0,1,0,0));

  static cMessage prepare(const StringRef& to, const StringRef& from, int net, 
    const StringRef& body, int type, const StringRef& ext, int flag);
  static void reply(cMessage *msg, const StringRef& body, const StringRef& ext = "", 
    bool html = false);

  static void send(cMessage *msg);

  inline static void send(tCntId cnt, const StringRef& body, int type = MT_MESSAGE, 
    const StringRef& ext = "", bool html = false, bool inject = false) 
  {
    Message::send(cnt, "", body, type, ext, html, inject);
  }
  static void send(int cnt, const StringRef& from, const StringRef& body, int type = MT_MESSAGE, 
    const StringRef& ext = "", bool html = false, bool inject = false);

  inline static void send(const StringRef& to, int net, const StringRef& body, int type = MT_MESSAGE, 
    const StringRef& ext = "", bool html = false) 
  {
    Message::send(to, "", net, body, type, ext, html);
  }
  static void send(const StringRef& to, const StringRef& from, int net, const StringRef& body, 
    int type = MT_MESSAGE, const StringRef& ext = "", bool html = false);

  static void sms(const StringRef& to, const StringRef& body, const StringRef& gate, 
    const StringRef& from = "", StringRef ext = "");
  static void inject(cMessage *msg, int cntID, const char * display = 0, 
    bool scroll = true);
};

#endif // __MESSAGE_H__