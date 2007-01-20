/**
  *  kIEview2 Control class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @copyright    Copyright (c) 2006-2007 Micha� "Dulek" Dulko
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

namespace kIEview2 {
  class Controller : public PlugController<Controller> {
  public:
    friend class PlugController<Controller>;

  public:
    typedef function<IECtrl::Var(IECtrl::Var&)> fExternalCallback;
    typedef signal<IECtrl::Var(IECtrl::Var&)> ExternalCallbackSig;

    struct sExternalCallback {
      ExternalCallbackSig signal;
      string name;
      long id;

      sExternalCallback(const StringRef& _name, fExternalCallback f): name(_name), id(random()) {
        if (!f.empty()) signal.connect(f);
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

    sExternalCallback* getExternalCallback(const char* name);
    sExternalCallback* getExternalCallback(long id);
    sExternalCallback* registerExternalCallback(const char* name, fExternalCallback f);

    int readMsgs(tCntId cnt, int howMany, int sessionOffset = 0);
    int readLastMsgSession(tCntId cnt, int sessionOffset = 0);

    string getMsgTypeLabel(int type);
    String getStatusLabel(int status);

    string bytesToString(double bytes);
    string timeToString(int time);

    String htmlEscape(StringRef& txt);
    String linkify(StringRef& txt);
    String nl2br(StringRef& txt);

  protected:
    void _onPrepare();
    void _onAction();

    void _msgCtrlView();
    void _msgCtrlSend();

    tCntId getCntFromMsg(cMessage* msg);
    String getDisplayFromMsg(UI::Notify::_insertMsg* an);

    bool isMsgFromHistory(sUIActionNotify_base* an);
    bool loadMsgTable(tCntId cnt);

    void handleTextFlag(int flag, int mask);
    char* getStringCol(Tables::oTable& table, tRowId row, int pos); // do zmiany

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