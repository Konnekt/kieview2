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
#include "EmotHandler.h"
#include "RtfHtml.h"
#include "EmotUI.h"

using namespace kIEview2;

namespace kIEview2 {
  namespace JS {
    class WndController;
    class Controller;
  }

  class Controller : public PlugController<Controller> {
  public:
    friend class PlugController<Controller>;

  public:
    typedef function<void(param_data&, Konnekt::UI::Notify::_insertMsg*)> fMessageHandler;
    typedef signal<void(param_data&, Konnekt::UI::Notify::_insertMsg*)> MessageHandlerSig;

    struct sMessageHandler {
      tConnections connections;
      MessageHandlerSig signal;
      string label;
    };

    struct sWndObjCollection {
      JS::WndController* jsWndController;
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
    bool bindMsgHandler(int type, fMessageHandler f, const string& label, int priority = 0, 
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
    void _emotLV();

    tCntId getCntFromMsg(cMessage* msg);
    String getDisplayFromMsg(Konnekt::UI::Notify::_insertMsg* an);

    bool isMsgFromHistory(sUIActionNotify_base* an);
    bool loadMsgTable(tCntId cnt);

    void handleTextFlag(int flag, int mask);
    char* getStringCol(Tables::oTable& table, tRowId row, int pos); // do zmiany

    String _parseStatusTpl(Konnekt::UI::Notify::_insertStatus* an);
    String _parseMsgTpl(Konnekt::UI::Notify::_insertMsg* an);

    /*
     * Message types specific methods
     */
    void _handleQuickEventTpl(param_data& data, Konnekt::UI::Notify::_insertMsg* an);
    void _handleStdMsgTpl(param_data& data, Konnekt::UI::Notify::_insertMsg* an);
    void _handleSmsTpl(param_data& data, Konnekt::UI::Notify::_insertMsg* an);
    void _handleFileTpl(param_data& data, Konnekt::UI::Notify::_insertMsg* an);

  public:
    bool autoScroll(sUIActionNotify_base* an, IECtrl* ctrl);
    void clearWnd(IECtrl* ctrl);

    static DWORD CALLBACK streamOut(DWORD, LPBYTE, LONG, LONG*);
    static LRESULT CALLBACK msgWndProc(HWND, UINT, WPARAM, LPARAM);
    
  public:
    WNDPROC oldMsgWndProc;
    UINT ieVersion;
    string kPath;

    EmotHandler emotHandler;

  protected:
    tWndObjCollection wndObjCollection;
    tMsgHandlers msgHandlers;
    CriticalSection _locker;
    Tables::oTable historyTable;
    JS::Controller* jsController;
    // EmotHandler emotHandler;
    TplHandler* tplHandler;
    RtfHtmlTag* rtfHtml;
  };

  namespace JS {
    class Controller : public IECtrl::iObject {
    public:
      Controller(IECtrl::Var& args): iObject(NULL, true), pCtrl(::Controller::getInstance()) {
        bindMethod("getPluginVersion", bind(&Controller::getPluginVersion, this, _1, _2));
        bindMethod("getPluginName", bind(&Controller::getPluginName, this, _1, _2));
        bindMethod("getEmot", bind(&Controller::getEmot, this, _1, _2));

        setProperty("ieVersion", (int) pCtrl->ieVersion);
        setProperty("name", "oController");
      }

    public:
      IECtrl::Var getEmot(IECtrl::Var& args, IECtrl::iObject* obj) {
        if (args.empty() || !args[0].isInteger()) return false;

        eM* emot;
        try {
          emot = pCtrl->emotHandler.getEmot(args[0].getInteger());
        } catch (const Exception& e) {
          throw IECtrl::JSException(e.getReason());
        }
        if (emot->isVirtual()) {
          return (char*) emot->getRawData().getString();
        }
        // wczytujemy plik z img_path i zwracamy
        return "ssaj";
      }

      IECtrl::Var getPluginName(IECtrl::Var& args, IECtrl::iObject* obj) {
        if (args.empty() || !args[0].isInteger()) return false;

        if (int plugID = Ctrl->ICMessage(IMC_FINDPLUG, args[0].getInteger(), IMT_ALL)) {
          return SAFECHAR((char*) Ctrl->IMessageDirect(IM_PLUG_NAME, plugID));
        }
        throw IECtrl::JSException("Plugin not found");
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
        throw IECtrl::JSException("Plugin not found");
      }

    protected:
      ::Controller* pCtrl;
    };

    class WndController : public IECtrl::iObject {
    public:
      WndController(IECtrl* ieCtrl, IECtrl::Var& args): iObject(ieCtrl, true), pCtrl(::Controller::getInstance()) {
        bindMethod("minimized", bind(resolve_cast0(&WndController::minimized), this));
        bindMethod("visible", bind(resolve_cast0(&WndController::visible), this));

        bindMethod("minimize", bind(resolve_cast0(&WndController::minimize), this));
        bindMethod("restore", bind(resolve_cast0(&WndController::restore), this));
        bindMethod("close", bind(resolve_cast0(&WndController::close), this));

        setProperty("name", "oWindow");
      }

    public:
      IECtrl::Var minimized() {
        return !IsIconic(m_pCtrl->getHWND());
      }
      IECtrl::Var visible() {
        return IsWindowVisible(m_pCtrl->getHWND());
      }

      IECtrl::Var minimize() {
        if (!minimized().getBool()) {
          return ShowWindow(m_pCtrl->getHWND(), SW_MINIMIZE);
        }
        return false;
      }
      IECtrl::Var restore() {
        if (minimized().getBool()) {
          ShowWindow(m_pCtrl->getHWND(), SW_RESTORE);
        }
        if (visible().getBool()) {
          SetWindowPos(m_pCtrl->getHWND(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        return true;
      }
      IECtrl::Var close() {
        return "@implement";
      }

    protected:
      ::Controller* pCtrl;
    };
  }
}

#endif // __CONTROLLER_H__