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
#include "Message.h"

#include "WndController.h"
#include "Controller.h"
#include "ActionHandler.h"
#include "StyleHandler.h"

namespace kIEview2 {
  WndController::WndController(sUIActionNotify_createWindow* an): jsController(0), insertedMsgs(0) {
    historyTable = Tables::registerTable(Ctrl, tableNotFound, optPrivate);
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

    insertedMsgs = 0;
    setSession(false);
  }

  void WndController::initWnd() {
    LockerCS lock(_locker);

    clearWnd();

    if (getCntID()) {
      if (int showOnLoad = pCtrl->getConfig()->getInt(cfg::showOnLoad)) {
        if (showOnLoad == showLastSession) {
          readLastMsgSession();
        } else {
          readMsgs(pCtrl->getConfig()->getInt(cfg::lastMsgCount));
        }
      }
    }
  }

  void WndController::switchStyle(StyleSet* style) {
    styleSetID = style->getID();

    if (getCntID()) {
      bool isEmpty = empty();
      clearWnd();

      if (!isEmpty) {
        readLastMsgSession();
      }
    }
  }

  void WndController::clearGroupedMsgs() {
    LockerCS lock(_locker);

    lastMsg = sGroupedMsg();
    lastSt = sGroupedSt();
  }

  bool WndController::loadMsgTable() {
    if (historyTable->isLoaded()) return false;
    tCntId cnt = getCntID();

    string dir = (const char*) Ctrl->ICMessage(IMC_PROFILEDIR);
    dir += "history\\messages\\";

    string file = "u";
    file += urlEncode(pCtrl->config->getChar(CNT_UID, cnt), '#') + ".";
    file += inttostr(pCtrl->config->getInt(CNT_NET, cnt)) + ".dtb";

    historyTable->setDirectory(dir.c_str());
    historyTable->setFilename(file.c_str());
    historyTable->load(true);

    return true;
  }

  char* WndController::getStringCol(tRowId row, int pos) {
    const char encryptKey[] = "\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84";
    const char decryptKey[] = "\x40\x13\xf8\xb2\x84\x23\x04\xae\x6f\x3d";

    Value v(Tables::ctypeString);
    v.vCChar = (char*) historyTable->getStr(row, historyTable->getColIdByPos(pos));
    v.buffSize = strlen(v.vCChar);

    if (historyTable->getColType(historyTable->getColIdByPos(pos)) & cflagXor) {
      DT::xor1_encrypt((unsigned char*)encryptKey, (unsigned char*)v.vCChar, v.buffSize);
      DT::xor1_decrypt((unsigned char*)decryptKey, (unsigned char*)v.vCChar, v.buffSize);

      IMLOG("[WndController::getStringCol()]: colPos = %i, value = %s", pos, v.vCChar);
    }
    return (char*) v.vCChar;
  }

  int WndController::readMsgs(int howMany, int sessionOffset, bool setSession) {
    tCntId cnt = getCntID();
    if (!howMany) {
      Message::quickEvent(cnt, "Brak wiadomoœci do wczytania.", false, false, true);
      return 0;
    }

    // locking
    LockerCS lock(_locker);

    IMLOG("[WndController::readMsgs()]: cnt = %i, howMany = %i, sessionOffset = %i",
      cnt, howMany, sessionOffset);

    Tables::oTable table = historyTable;
    bool dataLoaded = loadMsgTable();

    vector<Konnekt::UI::Notify::_insertMsg> msgs;
    int m = 0;

    for (int i = table->getRowCount() - 1, s = 0; (i >= 0) && (m < howMany); i--) {
      if (sessionOffset) {
        if (!table->getInt(i, table->getColIdByPos(fieldSession))) {
          if (++s == sessionOffset) {
            continue;
          }
        }
        if (s < sessionOffset) {
          continue;
        }
      }
      IMLOG("[WndController::readMsgs()]: i = %i, m = %i, s = %i", i, m, s);

      cMessage* msg = new cMessage;
      msg->id = table->getInt(i, table->getColIdByPos(fieldId));
      msg->net = table->getInt(i, table->getColIdByPos(fieldNet));
      msg->type = table->getInt(i, table->getColIdByPos(fieldType));
      msg->fromUid = getStringCol(i, fieldFromUid);
      msg->toUid = getStringCol(i, fieldToUid);
      msg->body = getStringCol(i, fieldBody);
      msg->ext = getStringCol(i, fieldExt);
      msg->flag = table->getInt(i, table->getColIdByPos(fieldFlag));
      msg->time = table->getInt64(i, table->getColIdByPos(fieldTime));

      msgs.push_back(Konnekt::UI::Notify::_insertMsg(msg, getStringCol(i, fieldDisplay), false));
      m++;
    }

    if (m) {
      if (setSession) this->setSession(true);
      Message::quickEvent(cnt, "Wczytujê wiadomoœci z historii.");
    }

    for (vector<Konnekt::UI::Notify::_insertMsg>::reverse_iterator it = msgs.rbegin(); it != msgs.rend(); it++) {
      Message::inject(it->_message, cnt, it->_display);

      delete it->_message;
    }

    String msg;
    if (!m) {
      msg = "Nie wczytano ¿adnych wiadomoœci.";
    } else if (m == 1) {
      msg = "Wczytano <b>jedn¹</b> wiadomoœæ.";
    } else {
      msg = "Wczytano <b>" + inttostr(m) + "</b> ostatnich wiadomoœci.";
    }

    Message::quickEvent(cnt, msg, true);

    if (dataLoaded) {
      table->unloadData();
    }
    return m;
  }

  int WndController::readLastMsgSession(int sessionOffset, bool setSession) {
    // locking
    LockerCS lock(_locker);

    tCntId cnt = getCntID();
    Tables::oTable table = historyTable;
    loadMsgTable();

    int howMany = 0;

    for (int i = table->getRowCount() - 1, s = 0; i >= 0; i--) {
      if (sessionOffset) {
        if (!table->getInt(i, table->getColIdByPos(fieldSession))) {
          s++;
        }
        if (s == sessionOffset) {
          howMany++;
        }
        continue;
      }
      howMany++;
      if (!table->getInt(i, table->getColIdByPos(fieldSession))) {
        break;
      }
    }

    int msgCount = readMsgs(howMany, sessionOffset, setSession);
    table->unloadData();

    return msgCount;
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