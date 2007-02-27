/**
  *  kIEview2 Controller class
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
#include "WndController.h"
#include "Helpers.h"

#include "IECtrl.h"
#include "RtfHtml.h"

#include "ActionHandler.h"
#include "TplHandler.h"
#include "EmotHandler.h"

#include "StyleUI.h"
#include "EmotUI.h"

using namespace kIEview2;
using namespace Helpers;

namespace kIEview2 {
  namespace JS {
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

  public:
    typedef std::map<int, sMessageHandler*> tMsgHandlers;
    typedef vector<oWndController> tWndControllers;

    typedef vector<sEmailInsertion> tEmailInsertions;
    typedef vector<sLinkInsertion> tLinkInsertions;

    enum enMsgFields {
      fieldId, fieldNet, fieldType, fieldFromUid, fieldToUid, fieldBody, 
      fieldExt, fieldFlag, fieldTime, fieldSession, fieldDisplay
    };

  public:
    /* Class version */
    STAMINA_OBJECT_CLASS_VERSION(Controller, PlugController, Version(0,1,0,0));

  protected: Controller();
  public: ~Controller();

    bool hasMsgHandler(int type);
    bool bindMsgHandler(int type, fMessageHandler f, const string& label, int priority = 0, 
      signals::connect_position pos = signals::at_back, bool overwrite = true);

    IECtrl::Var getJSWndController(IECtrl::Var& args, IECtrl::iObject* obj);
    IECtrl::Var getJSController();

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
      if (emots) emotHandler >> txt;
      if (linkify) txt = postLinkify(txt);

      return normalizeSpaces(txt);
    }

    String preLinkify(StringRef& txt);
    String postLinkify(StringRef& txt);

    String htmlEscape(StringRef& txt);
    String normalizeSpaces(StringRef& txt);
    String nl2br(StringRef& txt);

    int getIEVersion();

    oWndController getWndController(sUIActionNotify_base* an);
    oWndController getWndController(IECtrl* pCtrl);
    oWndController getWndController(tCntId cntID);

  protected:
    void _onPluginsLoaded();
    void _onPrepare();
    void _onAction();
    void _onCfgChanged();

    void _msgCtrlView();
    void _msgCtrlSend();

    void _styleLV();
    void _emotLV();

    tCntId getCntFromMsg(cMessage* msg);
    String getDisplayFromMsg(Konnekt::UI::Notify::_insertMsg* an);

    bool isMsgFromHistory(sUIActionNotify_base* an);
    bool loadMsgTable(tCntId cnt);

    void waitForIECtrlReady(IECtrl* pCtrl, UINT sleepTime = 100) {
      Ctrl->WMProcess();
      if (!pCtrl->isReady()) {
        do {
          Ctrl->WMProcess();
          Ctrl->Sleep(sleepTime);
        } while (!pCtrl->isReady());
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
    bool autoScroll(sUIActionNotify_base* an, IECtrl* pCtrl);

    EmotHandler* getEmotHandler() {
      return &emotHandler;
    }

    TplHandler* getTplHandler() {
      return &tplHandler;
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
    tWndControllers wndControllers;
    tMsgHandlers msgHandlers;
    CriticalSection _locker;
    Tables::oTable historyTable;
    JS::Controller* jsController;
    EmotHandler emotHandler;
    TplHandler tplHandler;
    WNDPROC oldMsgWndProc;
    StyleLV* styleLV;
    EmotLV* emotLV;
    RtfHtmlTag rtfHtml;

    tEmailInsertions eMailInsertions;
    tLinkInsertions linkInsertions;
  };

  namespace JS {
    class Controller : public IECtrl::iObject {
    public:
      Controller(): iObject(NULL, true), pCtrl(::Controller::getInstance()) {
        bindMethod("getPluginVersion", bind(&Controller::getPluginVersion, this, _1, _2));
        bindMethod("getPluginName", bind(&Controller::getPluginName, this, _1, _2));

        setProperty("ieVersion", (int) pCtrl->ieVersion);
        setProperty("name", "oController");
      }

    public:
      IECtrl::Var getPluginName(IECtrl::Var& args, IECtrl::iObject* obj) {
        if (args.empty() || !args[0].isInteger()) return false;

        if (int plugID = pluginExists(args[0].getInteger())) {
          return getPlugName(plugID);
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
  }
}

#endif // __CONTROLLER_H__