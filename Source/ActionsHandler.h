/**
  *  kIEview2 Actions handler class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  */

#pragma once

#ifndef __ACTIONSHANDLER_H__
#define __ACTIONSHANDLER_H__

#include "IECtrl.h"
#include "kIEView2.h"

namespace kIEview2 {
  class ActionsHandler: public IECtrl::AnchorClickListener,
                        public IECtrl::PopupMenuListener,
                        public IECtrl::DropListener,
                        public IECtrl::ExternalListener,
                        public IECtrl::KeyDownListener,
                        public IECtrl::ScriptMessageListener {
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
  };
}

#endif // __ACTIONSHANDLER_H__