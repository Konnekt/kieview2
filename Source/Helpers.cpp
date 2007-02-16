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
 * Int64 to string conversion
 */

std::string i64tostr(__int64 value, int radix) {
  char buf[50];
  _i64toa(value, (char*) buf, radix);

  return buf;
}

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
  void xor1_encrypt(const unsigned char* key, unsigned char* data, unsigned int size) {
    unsigned int ksize = strlen((char*)key);
    unsigned int ki = 0;

    if (!size) {
      size = strlen((char*)data);
    }

    int j = 0;
    for (unsigned int p = 0; p < size; p++) {
      *data = (*data ^ key[ki]) + (unsigned char)((j) & 0xFF); // | (j * 2);
      data++;
      ki++;
      if (ki >= ksize) ki = 0;
      j++;
    }
  }

  void xor1_decrypt(const unsigned char* key, unsigned char* data, unsigned int size) {
    unsigned int ksize = strlen((char*)key);
    unsigned int ki = 0;

    int j = 0;
    for (unsigned int p = 0; p < size; p++) {
      *data = (*data - (unsigned char)((j) & 0xFF)) ^ key[ki]; // | (j * 2);
      data++;
      ki++;
      if (ki >= ksize) ki = 0;
      j++;
    }
  }

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

  string rtrim(const char* txt, const char *chars) {
    if (!txt || !chars) return "";
    int nChars = strlen(chars);
    int nTxt = strlen(txt);

    if (!nTxt || !nChars || nChars >= nTxt) return "";
     
     int r = 0;
     while(!strncmp(&txt[nTxt - nChars - r], chars, nChars) && r < nTxt) {
       r+=nChars;
     }
     if (r) {
      string s;
      s.assign(txt, nTxt - nChars - r);
      return s;
    }
    return txt;
  }

  string trim(string txt, const string& chars) {
    txt = ltrim(txt.c_str(), chars.c_str());
    txt = rtrim(txt.c_str(), chars.c_str());
    return txt;
  }

/*
  string rtrim(string txt, const string& chars) {
    if (!txt.size() || !chars.size()) return "";

    for (string::const_reverse_iterator it = chars.rbegin(); it != chars.rend(); it++) {
      while (txt[txt.size() - 1] == (*it)) txt.erase(--txt.end());
    }
    return txt;
  }
*/

  string ltrim(const char* txt, const char *chars) {
    if (!txt || !chars) return "";
    int nChars = strlen(chars);
    int nTxt = strlen(txt);

    if (!nTxt || !nChars || nChars >= nTxt) return "";
    
     int r = 0;
     while(!strncmp(&txt[0 + r], chars, nChars) && r < nTxt) {
       r+=nChars;
     }
     if (r) {
      string s;
      s.assign(txt, nChars + r, nTxt - (nChars + r));
      return s;
    } 
    return txt;
  }

/*
  string ltrim(string txt, const string& chars) {
    if (!txt.size() || !chars.size()) return "";

    for (string::const_reverse_iterator it = chars.rbegin(); it != chars.rend(); it++) {
      while (txt[0] == (*it)) txt.erase(txt.begin());
    }
    return txt;
  }
*/

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

  void touchConfigWnd() {
    SendMessage((HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND)), WM_USER + 18091, 0, 0);
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

  Stamina::UI::oImage loadGif(const char* path, Size& size) {
    LPPICTURE pic;

    HANDLE hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    DWORD fSize = GetFileSize(hFile, NULL);
    if (fSize == -1) return NULL;

    LPVOID pvData = NULL;
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, fSize);
    if (!hGlobal ) return NULL;
    pvData = GlobalLock(hGlobal);
    DWORD dwBytesRead = 0;

    BOOL read = ReadFile(hFile, pvData, fSize, &dwBytesRead, NULL);
    if (!read) {
      GlobalUnlock(hGlobal);
      CloseHandle(hFile);
    }

    LPSTREAM pstm = NULL;
    HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);

    hr = OleLoadPicture(pstm, fSize, FALSE, IID_IPicture, (LPVOID *)&pic);
    pstm->Release();

    long px, py;
    pic->get_Width(&px);
    pic->get_Height(&py);

    HDC dc = GetDC(0);
    HDC tdc = CreateCompatibleDC(dc);
    HBITMAP hBitmap = CreateCompatibleBitmap(dc, size.cx, size.cy);
    SelectObject(tdc, hBitmap);
    RECT rc = {0, 0,  size.cx, size.cy};
    pic->Render(dc, 0, 0, size.cx, size.cy, 0, px, py, -px, &rc);
    Stamina::UI::oImage gif = Stamina::UI::Bitmap(hBitmap, false);
    pic->Release();
    ReleaseDC(0, dc);

    CloseHandle(hFile);
    GlobalUnlock(hGlobal);
    GlobalFree(hGlobal);

    return gif;
  }
}