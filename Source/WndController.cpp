/**
  *  kIEview2 Window Controller class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 261 $
  *  @modifiedby   $LastChangedBy: ursus6 $
  *  @lastmodified $Date: 2007-02-26 22:06:25 +0100 (Pn, 26 lut 2007) $
  */

#include "stdafx.h"

#include "ActionHandler.h"
#include "WndController.h"
#include "Controller.h"
#include "StyleHandler.h"

namespace kIEview2 {
  WndController::WndController(sUIActionNotify_createWindow* an): jsController(0), insertedMsgs(0) {
    pIECtrl = new IECtrl(an->hwndParent, an->x, an->y, an->w, an->h);
    actionHandler = new ActionHandler(this);

    pCtrl = Controller::getInstance();
    styleSetID = pCtrl->getStyleHandler()->getCurrentStyle()->getID();

    pasteSession = pCtrl->getConfig()->getInt(cfg::pasteSession);
    an->hwnd = pIECtrl->getHWND();
    cntID = an->act.cnt;

    pIECtrl->enableSandbox(false);
    // initWnd();
  }

  WndController::~WndController() {
    if (jsController) delete jsController;
    delete actionHandler;
    delete pIECtrl;
  }

  StyleSet* WndController::getStyleSet() {
    try {
      return pCtrl->getStyleHandler()->getByID(styleSetID);
    } catch(...) {
      return pCtrl->getStyleHandler()->getCurrentStyle();
    }
  }

  IECtrl::Var WndController::getJSController() {
    LockerCS lock(_locker);

    if (!jsController) {
      jsController = new JS::WndController(this);
    }
    return jsController;
  }

  void WndController::clearWnd() {
    LockerCS lock(_locker);

    // czyscimy wiadomosci grupowania
    clearGroupedMsgs();
    // ladujemy bootstrapa
    if (!getStyleSet()->empty()) {
      pIECtrl->navigate(("file:///" + unifyPath(getStyleSet()->getDir(), false, '/') + "/__bootstrap.html").c_str());
    } else {
      pIECtrl->navigate("about:blank");
    }
    // pIECtrl->clear();
    pIECtrl->waitTillLoaded();
    /*
    while (!pIECtrl->isReady()) {
#ifdef PLUGEXPORTH
      Ctrl->WMProcess();
      Ctrl->Sleep(250);
#else
      Sleep(250);
#endif
    }
    */

    insertedMsgs = 0;
    setSession(false);
  }

  void WndController::initWnd() {
    LockerCS lock(_locker);

    clearWnd();

    if (getCntID()) {
      if (int showOnLoad = pCtrl->getConfig()->getInt(cfg::showOnLoad)) {
        if (showOnLoad == showLastSession) {
          pCtrl->readLastMsgSession(cntID);
        } else {
          pCtrl->readMsgs(cntID, pCtrl->getConfig()->getInt(cfg::lastMsgCount));
        }
      }
    }
  }

  void WndController::switchStyle(StyleSet* style) {
    bool isEmpty = empty();
    styleSetID = style->getID();
    clearWnd();

    if (!isEmpty) {
      pCtrl->readLastMsgSession(cntID);
    }
  }

  void WndController::clearGroupedMsgs() {
    LockerCS lock(_locker);

    groupedMsgs.clear();
    groupedSt.clear();
  }

  namespace JS {
    WndController::WndController(oWndController wndCtrl): iObject(wndCtrl->getIECtrl(), true), oCtrl(wndCtrl) {
      bindMethod("minimized", bind(resolve_cast0(&WndController::minimized), this), true);
      bindMethod("visible", bind(resolve_cast0(&WndController::visible), this), true);
      bindMethod("tabbed", bind(resolve_cast0(&WndController::tabbed), this), true);

      bindMethod("minimize", bind(resolve_cast0(&WndController::minimize), this));
      bindMethod("restore", bind(resolve_cast0(&WndController::restore), this));
      bindMethod("show", bind(resolve_cast0(&WndController::show), this));
      bindMethod("close", bind(resolve_cast0(&WndController::close), this));
      bindMethod("clear", bind(resolve_cast0(&WndController::clear), this));
      bindMethod("flash", bind(&WndController::flash, this, _1, _2));

      bindMethod("breakGrouping", bind(resolve_cast0(&WndController::breakGrouping), this));
      bindMethod("reloadParent", bind(resolve_cast0(&WndController::reloadParent), this));

      setProperty("name", "oWindow");

      haveTabs = pluginExists(plugsNET::tabletka);
      reloadParent();
    }

    IECtrl::Var WndController::minimized() {
      return IsIconic(hWndWnd);
    }
    IECtrl::Var WndController::visible() {
      return IsWindowVisible(hWndWnd) && GetForegroundWindow() == hWndWnd;
    }
    IECtrl::Var WndController::tabbed() {
      return (bool) GetProp(GetParent(m_pCtrl->getHWND()), "TABBED");
    }

    IECtrl::Var WndController::minimize() {
      if (!minimized().getBool()) {
        return ShowWindow(hWndWnd, SW_MINIMIZE);
      }
      return false;
    }
    IECtrl::Var WndController::restore() {
      if (minimized().getBool()) {
        return ShowWindow(hWndWnd, SW_RESTORE) && show().getBool();
      }
      return false;
    }
    IECtrl::Var WndController::show() {
      if (minimized().getBool()) {
        return restore();
      }
      return SetForegroundWindow(hWndWnd);
    }
    IECtrl::Var WndController::close() {
      return CloseWindow(hWndWnd);
    }
    IECtrl::Var WndController::clear() {
      oCtrl->clearWnd();
      return true;
    }
    IECtrl::Var WndController::flash(IECtrl::Var& args, IECtrl::iObject* obj) {
      /*
      FLASHWINFO info;
      info.cbSize = sizeof(FLASHWINFO)
      info.dwFlags = FLASHW_ALL;
      info.hwnd = hWndWnd;
      info.dwTimeout = args[1].getInteger();
      info.uCount = args[0].getInteger();

      return FlashWindowEx(&info);
      */
      return false;
    }

    IECtrl::Var WndController::breakGrouping() {
      oCtrl->clearGroupedMsgs();
      return true;
    }
    IECtrl::Var WndController::reloadParent() {
      if (haveTabs && tabbed().getBool()) {
        hWndWnd = (HWND) Ctrl->IMessage(Tabs::IM::GetTabWindow, plugsNET::tabletka);
      } else {
        hWndWnd = GetParent(m_pCtrl->getHWND());
      }
      return true;
    }
  }
}