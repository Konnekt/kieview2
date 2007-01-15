/**
  *  kIEview2 Control class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @copyright    Copyright (c) 2006-2007 Micha³ "Dulek" Dulko
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  */

#pragma once

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "kIEview2.h"
#include "PlugController.h"
#include "Helpers.h"
#include "IECtrl.h"
#include "ActionHandler.h"
#include "TplHandler.h"
#include "TplUdf.h"
#include "RtfHtml.h"

using namespace kIEview2;

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

namespace kIEview2 {
  class Controller : public PlugController<Controller> {
  public:
    friend class PlugController<Controller>;

  public:
    typedef function<IECtrl::Var(const IECtrl::Var& args)> fExternalCallback;
    typedef signal<IECtrl::Var(const IECtrl::Var& args)> ExternalCallbackSig;

    struct sExternalCallback {
      ExternalCallbackSig signal;
      static long id;
      string name;

      sExternalCallback(const StringRef& _name, fExternalCallback f): name(_name) {
        if (!f.empty()) signal.connect(f);
        id++;
      }
    };

    typedef std::vector<sExternalCallback*> tExternalCallbacks;

  public:
    typedef function<void(param_data&, UI::Notify::_insertMsg*)> fMessageHandler;
    typedef signal<void(param_data&, UI::Notify::_insertMsg*)> MessageHandlerSig;

    struct sMessageHandler {
      tConnections connections;
      MessageHandlerSig signal;
      String label;
    };

  public:
    typedef std::map<IECtrl*, ActionHandler*> tActionHandlers;
    typedef std::map<int, sMessageHandler*> tMsgHandlers;

    struct sMsg {
      Date64 time;
      tCntId cnt;
    };

    enum enMsgFields {
      fieldId, fieldNet, fieldType, fieldFromUid, fieldToUid, fieldBody, 
      fieldExt, fieldFlag, fieldTime, fieldSession, fieldDisplay
    };

  public:
    /* Class version */
    STAMINA_OBJECT_CLASS_VERSION(Controller, PlugController, Version(0,1,0,0));

  protected:
    Controller();

  public:
    ~Controller();

    bool hasMsgHandler(int type);
    bool registerMsgHandler(int type, fMessageHandler f, StringRef label, int priority = 0, 
      signals::connect_position pos = signals::at_back, bool overwrite = true);

    int readMsgs(tCntId cnt, int howMany, int sessionOffset = 0);
    int readLastMsgSession(tCntId cnt, int sessionOffset = 0);

    string getMsgTypeLabel(int type);
    String getStatusLabel(int status);

    string bytesToString(double bytes);
    string timeToString(int time);

    String htmlEscape(StringRef& txt);
    String linkify(StringRef& txt);
    String nl2br(StringRef& txt);

    inline sExternalCallback* getExternalCallback(const char* name) {
      // locking
      LockerCS lock(_locker);

      for (tExternalCallbacks::iterator it = externalCallbacks.begin(); it != externalCallbacks.end(); it++) {
        if ((*it)->name == name) return (*it);
      }
      return 0;
    }
    inline sExternalCallback* getExternalCallback(long id) {
      // locking
      LockerCS lock(_locker);

      for (tExternalCallbacks::iterator it = externalCallbacks.begin(); it != externalCallbacks.end(); it++) {
        if ((*it)->id == id) return (*it);
      }
      return 0;
    }
    inline sExternalCallback* registerExternalCallback(const char* name, fExternalCallback f) {
      // locking
      LockerCS lock(_locker);

      if (!getExternalCallback(name)) {
        externalCallbacks.push_back(new sExternalCallback(name, f));
        return externalCallbacks[externalCallbacks.size() - 1];
      }
      return 0;
    }

  protected:
    void _onPrepare();
    void _onAction();

    void _msgCtrlView();
    void _msgCtrlSend();

    tCntId getCntFromMsg(cMessage* msg);
    String getDisplayFromMsg(UI::Notify::_insertMsg* an);
    bool isMsgFromHistory(sUIActionNotify_base* an);
    void handleTextFlag(int flag, int mask);
    bool loadMsgTable(tCntId cnt);

    inline char* getStringCol(Tables::oTable& table, tRowId row, int pos) {
      const char encryptKey[] = "\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84";
      const char decryptKey[] = "\x40\x13\xf8\xb2\x84\x23\x04\xae\x6f\x3d";

      char* resultChar = table->getStr(row, table->getColIdByPos(pos));

      if (table->getColType(table->getColIdByPos(pos)) & cflagXor) {
        xor1_encrypt((unsigned char*)encryptKey, (unsigned char*)resultChar, strlen(resultChar));
        xor1_decrypt((unsigned char*)decryptKey, (unsigned char*)resultChar, strlen(resultChar));

        IMLOG("colPos = %i, value = %s", pos, resultChar);
      }
      return resultChar;
    }

    String _parseStatusTpl(UI::Notify::_insertStatus* an);
    String _parseMsgTpl(UI::Notify::_insertMsg* an);

    /*
     * Message types specific methods
     */
    void _handleQuickEventTpl(param_data& data, UI::Notify::_insertMsg* an);
    void _handleStdMsgTpl(param_data& data, UI::Notify::_insertMsg* an);
    void _handleSmsTpl(param_data& data, UI::Notify::_insertMsg* an);
    void _handleFileTpl(param_data& data, UI::Notify::_insertMsg* an);

  public:
    void clearWnd(IECtrl* ctrl);
    static DWORD CALLBACK streamOut(DWORD, LPBYTE, LONG, LONG*);
    static LRESULT CALLBACK msgWndProc(HWND, UINT, WPARAM, LPARAM);
    
  public:
    WNDPROC oldMsgWndProc;

  protected:
    tExternalCallbacks externalCallbacks;
    tActionHandlers actionHandlers;
    tMsgHandlers msgHandlers;
    CriticalSection _locker;
    Tables::oTable historyTable;
    TplHandler* tplHandler;
    RtfHtmlTag* rtfHtml;
  };
}

#endif // __CONTROLLER_H__