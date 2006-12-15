/**
  *  kIEview2 PopupListner class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 16 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-08 19:37:43 +0100 (Pt, 08 gru 2006) $
  */

#include "stdafx.h"
#include "IECtrl.h"
#include "PopupListner.h"
#include "kIEView2.h"

namespace kIEview2 {
  IECtrl::PopupMenuListener::MakeAction PopupListner::PopupMenu(IECtrl::PopupMenuListener::MenuType type, POINT pt, IECtrl* ctrl) {
    switch (type) {
      // TODO: Menu nie dzia³a. Nie wiem jak pobraæ CNT-a tutaj.
      case IECtrl::PopupMenuListener::MenuType::Unknown: {
        IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, kIEview2::act::popup::popup), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, ctrl->getHWND()));
      }
      case IECtrl::PopupMenuListener::MenuType::Anchor: {
        IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, kIEview2::act::popup::popup), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, ctrl->getHWND()));
      }
      case IECtrl::PopupMenuListener::MenuType::Image: {
        IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, kIEview2::act::popup::popup), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, ctrl->getHWND()));
      }
      case IECtrl::PopupMenuListener::MenuType::Selection: {
        IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, kIEview2::act::popup::popup), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, ctrl->getHWND()));
      }
    }
    return IECtrl::PopupMenuListener::MakeAction::None;
  }
}