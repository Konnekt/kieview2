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
#include "StyleHandler.h"
#include "EmotHandler.h"

#include "StyleUI.h"
#include "EmotUI.h"

using namespace kIEview2;
using namespace Helpers;

namespace kIEview2 {
  class Controller;

  namespace JS {
    class UdfBridge : public IECtrl::iObject {
    public:
      UdfBridge() {
        setProperty("name", "JavaScript front-end to UDF (User Defined Functions) functions (used mostly in templates)");
      }
      bool __call(const string& name, IECtrl::Var& args, IECtrl::Var& ret);
    };

    class Controller : public IECtrl::iObject {
    public:
      Controller();

    protected:
      ::Controller* pCtrl;
    };
  }

  class Controller : public PlugController<Controller> {
  public:
    friend class PlugController<Controller>;
    friend class WndController;

  public:
    typedef function<void(oTemplate&, Konnekt::UI::Notify::_insertMsg*)> fMessageHandler;
    typedef signal<void(oTemplate&, Konnekt::UI::Notify::_insertMsg*)> MessageHandlerSig;

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
    typedef map<tMsgType, sMessageHandler*> tMsgHandlers;
    typedef vector<oWndController> tWndControllers;

    typedef vector<sEmailInsertion> tEmailInsertions;
    typedef vector<sLinkInsertion> tLinkInsertions;

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

    String getSettingStr(const string& name, tTable table, tRowId row = 0);
    string getMsgTypeLabel(tMsgType type);
    String getStatusLabel(int status);

    string bytesToString(double bytes);
    string timeToString(int time);

    String parseBody(StringRef& txt, bool escape, bool _nl2br, bool linkify, bool emots);

    String preLinkify(StringRef& txt);
    String postLinkify(StringRef& txt);

    String htmlEscape(StringRef& txt);
    String normalizeSpaces(StringRef& txt);
    String nl2br(StringRef& txt);

    int getIEVersion();

    void onStylesReload() {
      StyleSet* currentStyle = styleHandler.getCurrentStyle();
      for (tWndControllers::iterator it = wndControllers.begin(); it != wndControllers.end(); it++) {
        if (!styleHandler.hasStyle((*it)->getStyleSetID())) (*it)->switchStyle(currentStyle);
      }
    }

    void onSwitchStyle(StyleSet* oldStyle, StyleSet* newStyle) {
      for (tWndControllers::iterator it = wndControllers.begin(); it != wndControllers.end(); it++) {
        if ((*it)->getStyleSet()->getID() == oldStyle->getID()) (*it)->switchStyle(newStyle);
      }
    }

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

    void setActionsStatus();
    static void handleTextFlag(int flag, int mask, HWND hwnd);

    String _parseStatusTpl(Konnekt::UI::Notify::_insertStatus* an);
    String _parseMsgTpl(Konnekt::UI::Notify::_insertMsg* an);

    /*
     * Message types specific methods
     */
    void _handleQuickEventTpl(oTemplate& data, Konnekt::UI::Notify::_insertMsg* an);
    void _handleStdMsgTpl(oTemplate& data, Konnekt::UI::Notify::_insertMsg* an);
    void _handleSmsTpl(oTemplate& data, Konnekt::UI::Notify::_insertMsg* an);
    void _handleFileTpl(oTemplate& data, Konnekt::UI::Notify::_insertMsg* an);

  public:
    bool autoScroll(sUIActionNotify_base* an, IECtrl* pCtrl);

    EmotHandler* getEmotHandler() {
      return &emotHandler;
    }
    StyleHandler* getStyleHandler() {
      return &styleHandler;
    }

    static DWORD CALLBACK streamOut(DWORD, LPBYTE, LONG, LONG*);
    static LRESULT CALLBACK msgWndProc(HWND, UINT, WPARAM, LPARAM);
    static LRESULT CALLBACK msgREWndProc(HWND, UINT, WPARAM, LPARAM);

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
    JS::Controller* jsController;
    JS::UdfBridge udfBridge;
    EmotHandler emotHandler;
    StyleHandler styleHandler;
    WNDPROC oldMsgWndProc;
    WNDPROC oldREWndProc;
    StyleLV* styleLV;
    EmotLV* emotLV;
    RtfHtmlTag rtfHtml;

    tEmailInsertions eMailInsertions;
    tLinkInsertions linkInsertions;
  };
}

#endif // __CONTROLLER_H__