/**
  *  kIEview2 Action handler class
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

#ifndef __ACTIONHANDLER_H__
#define __ACTIONHANDLER_H__

#include "kIEView2.h"
#include "IECtrl.h"

namespace kIEview2 {
  class ActionHandler: public IECtrl::AnchorClickListener,
                       public IECtrl::PopupMenuListener,
                       public IECtrl::DropListener,
                       public IECtrl::ExternalListener,
                       public IECtrl::KeyDownListener,
                       public IECtrl::ScriptMessageListener {
  public:
    typedef IECtrl::PopupMenuListener::MakeAction tMenuAction;
    typedef IECtrl::PopupMenuListener::MenuType tMenuType;

  public:
    ActionHandler(tCntId _cntId = 0): selectedMenuItem(0), cntId(_cntId) { }

  public:
    void anchorClicked(const char* url, IECtrl* ctrl);
    void fileDropped(const char *url, IECtrl* ctrl);
    MakeAction popupMenu(MenuType type, POINT pt, IECtrl* ctrl);
    int showMessage(HWND hWnd, const char* lpText, DWORD dwType);

    long getMemberID(const char *name);
    string getMemberName(long id);
    IECtrl::Var trigger(long id, IECtrl::Var& args, IECtrl* ctrl, bool construct = false);
    bool isObject(long id);

    // @todo implement
    inline bool keyDown(UINT uCode, DWORD dwFlags) { return true; }

  public:
    int selectedMenuItem;
    tCntId cntId;
  };
}

#endif // __ACTIONHANDLER_H__