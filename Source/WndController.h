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

using namespace Konnekt::Tables;
using namespace kIEview2;
using namespace Helpers;

namespace kIEview2 {
  struct sGroupedMsg {
    String display;
    Date64 time;
    tCntId cnt;
    tMsgType type;
    bool empty;

    sGroupedMsg(tCntId _cnt, tMsgType _type, const Date64& _time, const StringRef& _display): 
      cnt(_cnt), type(_type), time(_time), display(_display), empty(false) { }
    sGroupedMsg(): cnt(0), type(0), empty(true) { }
  };

  struct sGroupedSt {
    Date64 time;
    String info;
    int status;
    bool empty;

    sGroupedSt(int _status, const Date64& _time, const StringRef& _info): 
      status(_status), time(_time), info(_info), empty(false) { }
    sGroupedSt(): status(0), empty(true) { }
  };

  namespace JS {
    class WndController;
  }
  class ActionHandler;
  class Controller;
  class StyleSet;

  class WndController : public SharedObject<iSharedObject> {
  public:
    enum enMsgFields {
      fieldId, fieldNet, fieldType, fieldFromUid, fieldToUid, fieldBody, 
      fieldExt, fieldFlag, fieldTime, fieldSession, fieldDisplay
    };

  public:
    WndController(sUIActionNotify_createWindow* an);
    ~WndController();

  public:
    bool loadMsgTable();
    char* getStringCol(tRowId row, int pos); // do zmiany

    int readMsgs(int howMany, int sessionOffset = 0, bool setSession = false);
    int readLastMsgSession(int sessionOffset = 0, bool setSession = false);

    void switchStyle(StyleSet* style);
    void clearGroupedMsgs();

    bool empty() {
      return insertedMsgs == 0;
    }
    void clearWnd();
    void initWnd();

    bool getSession() const {
      return GetProp(GetParent(pIECtrl->getHWND()), "MsgSession");
    }
    void setSession(bool value) {
      SetProp(GetParent(pIECtrl->getHWND()), "MsgSession", (HANDLE) value);
    }

    string getStyleSetID() {
      return styleSetID;
    }
    StyleSet* getStyleSet();

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
    Tables::oTable historyTable;
    Controller* pCtrl;
    IECtrl* pIECtrl;

    UINT insertedMsgs;
    bool pasteSession;
    string styleSetID;
    tCntId cntID;

    sGroupedMsg lastMsg;
    sGroupedSt lastSt;

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
      IECtrl::Var clear();
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