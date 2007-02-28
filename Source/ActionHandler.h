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
#include "WndController.h"

namespace kIEview2 {
  class ActionHandler: public IECtrl::AnchorClickListener,
                       public IECtrl::PopupMenuListener,
                       public IECtrl::DropListener,
                       public IECtrl::KeyDownListener,
                       public IECtrl::ScriptMessageListener {
  public:
    typedef IECtrl::PopupMenuListener::MakeAction tMenuAction;
    typedef IECtrl::PopupMenuListener::MenuType tMenuType;

  public:
    ActionHandler(oWndController wndCtrl): selectedMenuItem(0), wndCtrl(wndCtrl) {
      IECtrl* pCtrl = wndCtrl->getIECtrl();

      pCtrl->setPopupMenuListener(this);
      pCtrl->setAnchorClickListener(this);
      pCtrl->setDropListener(this);
      pCtrl->setScriptMessageListener(this);
      pCtrl->setKeyDownListener(this);
    }

  public:
    void anchorClicked(const char* url, IECtrl* pCtrl);
    void fileDropped(const char *url, IECtrl* pCtrl);

    MakeAction popupMenu(MenuType type, POINT pt, IECtrl* pCtrl);
    int showMessage(HWND hWnd, const char* lpText, DWORD dwType);

    // @todo implement
    inline bool keyDown(UINT uCode, DWORD dwFlags) { return true; }

  public:
    int selectedMenuItem;

  protected:
    oWndController wndCtrl;
  };
}

#endif // __ACTIONHANDLER_H__