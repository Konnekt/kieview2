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

#include "IECtrl.h"
#include "kIEView2.h"

namespace kIEview2 {
  class ActionHandler: public IECtrl::AnchorClickListener,
                        public IECtrl::PopupMenuListener,
                        public IECtrl::DropListener,
                        public IECtrl::ExternalListener,
                        public IECtrl::KeyDownListener,
                        public IECtrl::ScriptMessageListener {
  public:
    ActionHandler(tCntId _cntId = 0): selectedMenuItem(0), cntId(_cntId) { }

  public:
    void AnchorClicked(const char* url, IECtrl* ctrl);
    void FileDropped(const char *url, IECtrl* ctrl);
    MakeAction PopupMenu(MenuType type, POINT pt, IECtrl* ctrl);
    int ShowMessage(HWND hWnd, const char* lpText, DWORD dwType);

    // @todo implement
    inline bool KeyDown(UINT uCode, DWORD dwFlags) { return true; }
    inline long GetMemberID(const char *name) { return 0; }
    inline IECtrl::Var Trigger(long id, IECtrl::Var &args) { return IECtrl::Var(); }

  public:
    int selectedMenuItem;
    tCntId cntId;
  };
}

#endif // __ACTIONHANDLER_H__