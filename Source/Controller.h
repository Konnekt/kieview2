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
#include "EmotHandler.h"
#include "RtfHtml.h"
#include "EmotUI.h"

using namespace kIEview2;
using namespace Helpers;

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

    struct sGroupedMsg {
      Date64 time;
      tCntId cnt;
      int type;

      sGroupedMsg(tCntId _cnt, int _type, const Date64& _time): cnt(_cnt), type(_type), time(_time) { }
      sGroupedMsg(): cnt(0), type(0) { }
    };
    typedef vector<sGroupedMsg> tGroupedMsgs;

    struct sGroupedSt {
      String info;
      Date64 time;
      int status;

      sGroupedSt(int _status, const Date64& _time, const StringRef& _info = ""): status(_status), time(_time), info(_info) { }
      sGroupedSt(): status(0) { }
    };
    typedef vector<sGroupedSt> tGroupedSt;

    struct sEmailInsertion {
      string email;
      UINT id;

      sEmailInsertion(UINT _id, const string& _email): id(_id), email(_email) { }
      sEmailInsertion(): id(0) { }
    };

    struct sLinkInsertion {
      string name;
      string url;
      UINT id;

      sLinkInsertion(UINT _id): id(_id) { }
      sLinkInsertion(): id(0) { }
    };

    struct sWndObjCollection {
      JS::WndController* jsWndController;
      ActionHandler* actionHandler;

      tGroupedMsgs groupedMsgs;
      tGroupedSt groupedSt;

      sWndObjCollection(): jsWndController(0), actionHandler(0) { }
      ~sWndObjCollection() {
        if (jsWndController) delete jsWndController;
        if (actionHandler) delete actionHandler;
      }
    };

  public:
    typedef std::map<IECtrl*, sWndObjCollection> tWndObjCollection;
    typedef std::map<int, sMessageHandler*> tMsgHandlers;

    typedef vector<sEmailInsertion> tEmailInsertions;
    typedef vector<sLinkInsertion> tLinkInsertions;

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

    String parseBody(StringRef& txt, bool escape, bool _nl2br, bool linkify, bool emots) {
      if (escape) txt = htmlEscape(txt);
      if (_nl2br) txt = nl2br(txt);
      if (linkify) txt = preLinkify(txt);
      if (emots) txt = emotHandler.parse(txt, /* config->getInt(CNT_NET, an->act.cnt) */ 0);
      if (linkify) txt = postLinkify(txt);

      return normalizeSpaces(txt);
    }

    String preLinkify(StringRef& txt);
    String postLinkify(StringRef& txt);

    String htmlEscape(StringRef& txt);
    String normalizeSpaces(StringRef& txt);
    String nl2br(StringRef& txt);

    int getIEVersion();

    sWndObjCollection* getWndObjects(sUIActionNotify_base* an) {
      IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));
      return ctrl ? &wndObjCollection[ctrl] : NULL;
    }
    sWndObjCollection* getWndObjects(IECtrl* ctrl) {
      return ctrl ? &wndObjCollection[ctrl] : NULL;
    }

    void clearGroupedMsgs(sUIActionNotify_base* an) {
      getWndObjects(an)->groupedMsgs.clear();
    }
    void clearGroupedMsgs(IECtrl* ctrl) {
      getWndObjects(ctrl)->groupedMsgs.clear();
    }

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

    void waitForIECtrlReady(IECtrl* ctrl, UINT sleepTime = 100) {
      Ctrl->WMProcess();
      if (!ctrl->isReady()) {
        do {
          Ctrl->WMProcess();
          Ctrl->Sleep(sleepTime);
        } while (!ctrl->isReady());
      }
    }

    void setActionsStatus() {
      bool showEmot = config->getInt(cfg::useEmots) && config->getInt(cfg::showEmotChooser);
      bool showAutoScroll = config->getInt(cfg::showAutoScroll);
      bool showFormat = config->getInt(cfg::showFormattingBtns);
      bool showColor = config->getInt(cfg::showColorChooser);

      UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::autoScroll), !showAutoScroll ? -1 : 0, ACTS_HIDDEN);
      UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::emots), !showEmot ? -1 : 0, ACTS_HIDDEN);
      UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::color), !showColor ? -1 : 0, ACTS_HIDDEN);

      UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::underline), !showFormat ? -1 : 0, ACTS_HIDDEN);
      UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::italic), !showFormat ? -1 : 0, ACTS_HIDDEN);
      UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::bold), !showFormat ? -1 : 0, ACTS_HIDDEN);

      UIActionSetStatus(sUIAction(IMIG_MSGBAR, act::formatTb::formatTb), 
        !(showEmot || showAutoScroll || showFormat || showColor) ? -1 : 0, ACTS_HIDDEN);
    }

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
    void initWnd(IECtrl* ctrl);

    EmotHandler* getEmotHandler() {
      return &emotHandler;
    }

    static DWORD CALLBACK streamOut(DWORD, LPBYTE, LONG, LONG*);
    static LRESULT CALLBACK msgWndProc(HWND, UINT, WPARAM, LPARAM);
    
  public:
    UINT ieVersion;
    string kPath;

  protected:
    static string __stdcall linkInsertion(RegEx* reg);
    static string __stdcall replaceLink(RegEx* reg);

    static string __stdcall eMailInsertion(RegEx* reg);
    static string __stdcall replaceEmail(RegEx* reg);

  protected:
    tWndObjCollection wndObjCollection;
    tMsgHandlers msgHandlers;
    CriticalSection _locker;
    Tables::oTable historyTable;
    JS::Controller* jsController;
    EmotHandler emotHandler;
    WNDPROC oldMsgWndProc;
    EmotLV* emotLV;
    TplHandler* tplHandler;
    RtfHtmlTag* rtfHtml;

    tEmailInsertions eMailInsertions;
    tLinkInsertions linkInsertions;
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
          emot = pCtrl->getEmotHandler()->getEmot(args[0].getInteger());
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
      WndController(IECtrl* ctrl, IECtrl::Var& args): iObject(ctrl, true), pCtrl(::Controller::getInstance()) {
        bindMethod("breakGrouping", bind(resolve_cast0(&WndController::breakGrouping), this));

        bindMethod("minimized", bind(resolve_cast0(&WndController::minimized), this));
        bindMethod("visible", bind(resolve_cast0(&WndController::visible), this));

        bindMethod("minimize", bind(resolve_cast0(&WndController::minimize), this));
        bindMethod("restore", bind(resolve_cast0(&WndController::restore), this));
        bindMethod("close", bind(resolve_cast0(&WndController::close), this));

        bindMethod("tabbedWindow", bind(resolve_cast0(&WndController::tabbedWindow), this));
        bindMethod("reloadParent", bind(resolve_cast0(&WndController::reloadParent), this));

        setProperty("name", "oWindow");

        if (ICMessage(IMC_FINDPLUG, Tabs::net, IMT_ALL) && GetProp(GetParent(ctrl->getHWND()), "TABBED")) {
          hWndWnd = (HWND)IMessage(Tabs::IM::GetTabWindow, Tabs::net);
        } else {
          hWndWnd = GetParent(ctrl->getHWND());
        }
      }

    public:
      IECtrl::Var breakGrouping() {
        pCtrl->clearGroupedMsgs(m_pCtrl);
        return true;
      }

      IECtrl::Var minimized() {
        return IsIconic(hWndWnd);
      }
      IECtrl::Var visible() {
        return IsWindowVisible(hWndWnd);
      }

      IECtrl::Var minimize() {
        if (!minimized().getBool()) {
          return ShowWindow(hWndWnd, SW_MINIMIZE);
        }
        return false;
      }
      IECtrl::Var restore() {
        if (minimized().getBool()) {
          ShowWindow(hWndWnd, SW_RESTORE);
        }
        if (visible().getBool()) {
          SetWindowPos(hWndWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        return true;
      }
      IECtrl::Var close() {
        return CloseWindow(hWndWnd);
      }

      IECtrl::Var tabbedWindow() {
        return GetProp(GetParent(ctrl->getHWND()), "TABBED");
      }

      IECtrl::Var reloadParent() {
        if (ICMessage(IMC_FINDPLUG, Tabs::net, IMT_ALL) && GetProp(GetParent(ctrl->getHWND()), "TABBED")) {
          hWndWnd = (HWND)IMessage(Tabs::IM::GetTabWindow, Tabs::net);
          return 1;
        } else {
          hWndWnd = GetParent(ctrl->getHWND());
          return 0;
        }
      }

    protected:
      ::Controller* pCtrl;
      HWND hWndWnd;
    };
  }
}

#endif // __CONTROLLER_H__