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

#include "stdafx.h"
#include "Helpers.h"

/*
 *  Bool -> Human readable string
 */

char * btoa(bool value) {
  return value ? "yes" : "no";
}

String btos(bool value) {
  return btoa(value);
}

/*
 *  NULL -> Human readable string
 */

char * nullChk(const StringRef& value) {
  return value.length() ? value.a_str() : "(none)";
}

char * nullChk(char * value) {
  return strlen(value) ? value : "(none)";
}

/*
 *  Logging helpers, just a little shortcut ;>
 */

void log(enDebugLevel level, const char * format, va_list ap) {
  if (Ctrl && Ctrl->DebugLevel(level)) {
    Ctrl->IMLOG_(level, format, ap);
  }
}

void log(const char * format, ...) {
  va_list ap;
  va_start(ap, format);
	log(DBG_LOG, format, ap);
  va_end(ap);
}

void logDebug(const char * format, ...) {
  va_list ap;
  va_start(ap, format);
	log(DBG_DEBUG, format, ap);
  va_end(ap);
}

/*
 *  Various helpers
 */

namespace Helpers {
  String icon16(int ico) {
    return "reg://IML16/" + inttostr(ico) + ".ico";
  }

  String icon32(int ico) {
    return "reg://IML32/" + inttostr(ico) + ".ico";
  }

  String trunc(StringRef txt, int limit, const StringRef& suffix) {
    if (limit && (txt.length() > limit)) {
      txt.erase(limit);

      if (int suffixSize = suffix.length()) {
        txt.replace(txt.length() - suffixSize, suffix, suffixSize);
      }
    }
    return PassStringRef(txt);
  }

  String trim(const StringRef& txt) {
    CStdString buff(txt.a_str()); 
    buff = buff.Trim();

    return buff;
  }

  int getPluginsGroup() {
    return Ctrl->ICMessage(IMI_GETPLUGINSGROUP);
  }

  int pluginExists(int net, int type) {
    return Ctrl->ICMessage(IMC_FINDPLUG, net, type);
  }

  const char * getPlugName(int plugID) {
    return SAFECHAR((char*) Ctrl->IMessageDirect(IM_PLUG_NAME, plugID));
  }

  void UIActionCall(int group, int act, int cntID) {
    Ctrl->ICMessage(IMI_ACTION_CALL, 
      (int) &sUIActionNotify_2params(sUIAction(group, act, cntID), ACTN_ACTION, 0, 0), 0);
  }

#ifdef SHARED_TABLETKA_H
  bool isMsgWndOpen(int cnt) {
    return Tabs::GetWindowState(cnt) != 0;
  }
#endif

  void showKNotify(char * text, int ico) {
    Ctrl->IMessage(&KNotify::sIMessage_notify(text, ico));
  }

#ifdef __STAMINA_TIME64__
  bool isToday(Date64 date) {
    Date64 today(true);

    return ((date.day != today.day) || (date.month != today.month) || (date.year != today.year)) 
      ? false : true;
  }
#endif

  int findParentAction(int group, int id) {
    return Ctrl->ICMessage(IMI_ACTION_FINDPARENT, (int) &sUIAction(group, id));
  }

  void addItemToHistory(cMessage* msg, int cnt, const char * dir, const StringRef& name, int session) {
    sHISTORYADD item;

    item.cnt = cnt;
    item.m = msg;
    item.dir = dir;
    item.name = name.a_str();
    item.session = session;

    Ctrl->ICMessage(IMI_HISTORY_ADD, (int) &item);
  }

  void chgBtn(int group, int id, int cnt, const char * name, int ico, int flags) {
    sUIActionInfo ai;

    if (name) ai.txt = (char*) name;
    if (ico) ai.p1 = ico;
    if (flags >= 0) {
      ai.status = flags;
      ai.statusMask = -1;
    }

    ai.act = sUIAction(group, id, cnt);
    ai.mask = (name ? UIAIM_TXT : 0) | (ico ? UIAIM_P1 : 0) | (flags >= 0 ? UIAIM_STATUS : 0);

    UIActionSet(ai);
  }

  void chgBtn(int group, int id, const char * name, int ico, int flags) {
    UIActionSet(sUIActionInfo(group, id, 0, flags, (char*) name, ico));
  }
}