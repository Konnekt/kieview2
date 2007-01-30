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
#include "RtfHtml.h"

using namespace kIEview2;

namespace kIEview2 {
  class JSWndController;
  class JSController;

  class Controller : public PlugController<Controller> {
  public:
    friend class PlugController<Controller>;

  public:
    typedef function<void(param_data&, UI::Notify::_insertMsg*)> fMessageHandler;
    typedef signal<void(param_data&, UI::Notify::_insertMsg*)> MessageHandlerSig;

    struct sMessageHandler {
      tConnections connections;
      MessageHandlerSig signal;
      String label;
    };

    struct sWndObjCollection {
      JSWndController* jsWndController;
      ActionHandler* actionHandler;

      sWndObjCollection(): jsWndController(0), actionHandler(0) { }
      ~sWndObjCollection() {
        if (jsWndController) delete jsWndController;
        if (actionHandler) delete actionHandler;
      }
    };

  public:
    typedef std::map<IECtrl*, sWndObjCollection> tWndObjCollection;
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

    IECtrl::Var getJSWndController(IECtrl::Var& args, IECtrl::iObject* obj);
    IECtrl::Var getJSController(IECtrl::Var& args, IECtrl::iObject* obj);

    int readMsgs(tCntId cnt, int howMany, int sessionOffset = 0);
    int readLastMsgSession(tCntId cnt, int sessionOffset = 0);

    String getSettingStr(const string& name, tTable table, tRowId row = 0);
    string getMsgTypeLabel(int type);
    String getStatusLabel(int status);

    string bytesToString(double bytes);
    string timeToString(int time);

    static string __stdcall linkifyDo(RegEx* reg);
    static string __stdcall mailifyDo(RegEx* reg);

    String htmlEscape(StringRef& txt);
    String linkify(StringRef& txt);
    String normalizeSpaces(StringRef& txt);
    String nl2br(StringRef& txt);

    int getIEVersion();

  protected:
    void _onPluginsLoaded();
    void _onPrepare();
    void _onAction();
    void _onCfgChanged();

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
    UINT ieVersion;

  protected:
    tWndObjCollection wndObjCollection;
    tMsgHandlers msgHandlers;
    CriticalSection _locker;
    Tables::oTable historyTable;
    JSController* jsController;
    TplHandler* tplHandler;
    RtfHtmlTag* rtfHtml;
  };

  class JSController : public IECtrl::iObject {
  public:
    JSController(IECtrl::Var& args): iObject(NULL, true), pCtrl(Controller::getInstance()) {
      bindMethod("getPluginVersion", bind(&JSController::getPluginVersion, this, _1, _2));
      bindMethod("getPluginName", bind(&JSController::getPluginName, this, _1, _2));

      setProperty("ieVersion", (int) pCtrl->ieVersion);
      setProperty("name", "oController");
    }

  public:
    IECtrl::Var getPluginName(IECtrl::Var& args, IECtrl::iObject* obj) {
      if (args.empty() || !args[0].isInteger()) return false;
      
      if (int plugID = Ctrl->ICMessage(IMC_FINDPLUG, args[0].getInteger(), IMT_ALL)) {
        return SAFECHAR((char*) Ctrl->IMessageDirect(IM_PLUG_NAME, plugID));
      }
      return false;
    }

    IECtrl::Var getPluginVersion(IECtrl::Var& args, IECtrl::iObject* obj) {
      if (args.empty()) return false;

      int plugID = 0;
      if (args[0].isString()) {
        plugID = Ctrl->ICMessage(IMC_FINDPLUG_BYNAME, (int) args[0].getString());
      } else if (args[0].isInteger()) {
        plugID = Ctrl->ICMessage(IMC_FINDPLUG, args[0].getInteger(), IMT_ALL);
      } else {
        return false;
      }

      if (plugID) {
        char ver[50] = {0};
        Ctrl->ICMessage(IMC_PLUG_VERSION, Ctrl->ICMessage(IMC_PLUGID_POS, plugID, 0), (int) ver);
        if (Ctrl->getError() != IMERROR_NORESULT) {
	        return ver;
        }
      }
      // throw JSException("Plugin not found");
      return false;
    }

  protected:
    Controller* pCtrl;
  };

  class JSWndController : public IECtrl::iObject {
  public:
    JSWndController(IECtrl *ieCtrl, IECtrl::Var& args): iObject(ieCtrl, true), pCtrl(Controller::getInstance()) {
      bindMethod("minimize", bind(resolve_cast0(&JSWndController::minimize), this));
      bindMethod("close", bind(resolve_cast0(&JSWndController::close), this));

      setProperty("name", "oWindow");
    }

  public:
    IECtrl::Var minimize() {
      return "@implement";
    }
    IECtrl::Var close() {
      return "@implement";
    }

  protected:
    Controller* pCtrl;
  };
}

#endif // __CONTROLLER_H__