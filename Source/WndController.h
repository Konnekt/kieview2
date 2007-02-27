/**
  *  kIEview2 Window Controller class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 246 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2007-02-24 03:57:02 +0100 (So, 24 lut 2007) $
  */

#pragma once

#ifndef __WNDCONTROLLER_H__
#define __WNDCONTROLLER_H__

#include "kIEview2.h"
#include "Helpers.h"

#include "IECtrl.h"
#include "TplHandler.h"

using namespace kIEview2;
using namespace Helpers;

namespace kIEview2 {
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

  namespace JS {
    class WndController;
  }
  class ActionHandler;
  class Controller;

  class WndController : public SharedObject<iSharedObject> {
  public:
    WndController(sUIActionNotify_createWindow* an);
    ~WndController();

  public:
    void clearGroupedMsgs();

    void clearWnd();
    void initWnd();

    tCntId getCntID() const {
      return cntID;
    }
    IECtrl* getIECtrl() {
      return pIECtrl;
    }
    IECtrl::Var getJSController();

  public:
    JS::WndController* jsController;
    ActionHandler* actionHandler;
    Controller* pCtrl;
    IECtrl* pIECtrl;

    TplSet* tplSet;
    tCntId cntID;

    tGroupedMsgs groupedMsgs;
    tGroupedSt groupedSt;

  protected:
    CriticalSection _locker;
  };
  typedef SharedPtr<WndController> oWndController;

  namespace JS {
    class WndController : public IECtrl::iObject {
    public:
      WndController(oWndController wndCtrl);

    public:
      IECtrl::Var minimized();
      IECtrl::Var visible();
      IECtrl::Var tabbed();

      IECtrl::Var minimize();
      IECtrl::Var restore();
      IECtrl::Var show();
      IECtrl::Var close();
      IECtrl::Var flash(IECtrl::Var& args, IECtrl::iObject* obj);

      IECtrl::Var breakGrouping();
      IECtrl::Var reloadParent();

    protected:
      oWndController oCtrl;

      bool haveTabs;
      HWND hWndWnd;
    };
  }
}

#endif // __WNDCONTROLLER_H__