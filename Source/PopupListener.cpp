/**
  *  kIEview2 PopupListener class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  */

#include "stdafx.h"
#include "Controller.h"
#include "IECtrl.h"
#include "PopupListener.h"

namespace kIEview2 {
  IECtrl::PopupMenuListener::MakeAction PopupListener::PopupMenu(IECtrl::PopupMenuListener::MenuType type, POINT pt, IECtrl* ctrl) {
    switch (type) {
      case IECtrl::PopupMenuListener::MenuType::Unknown: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::openUrl), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copyUrl), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::urlSep), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::saveImage), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::imageSep), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copySelection), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::selectAll), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::history), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clearSep), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clear), 0, ACTS_HIDDEN);
        IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, act::popup::popup, ctrl->getCntId()), TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, 0, UIActionHandleDirect(sUIAction(0, IMIG_MSGWND, ctrl->getCntId()))));
        break;
      }
      case IECtrl::PopupMenuListener::MenuType::Anchor: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::openUrl), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copyUrl), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::urlSep), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::saveImage), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::imageSep), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copySelection), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::selectAll), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::history), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clearSep), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clear), 0, ACTS_HIDDEN);
        IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, act::popup::popup, ctrl->getCntId()), TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, 0, UIActionHandleDirect(sUIAction(0, IMIG_MSGWND, ctrl->getCntId()))));
        break;
      }
      case IECtrl::PopupMenuListener::MenuType::Image: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::openUrl), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copyUrl), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::urlSep), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::saveImage), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::imageSep), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copySelection), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::selectAll), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::history), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clearSep), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clear), 0, ACTS_HIDDEN);
        IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, act::popup::popup, ctrl->getCntId()), TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, 0, UIActionHandleDirect(sUIAction(0, IMIG_MSGWND, ctrl->getCntId()))));
        break;
      }
      case IECtrl::PopupMenuListener::MenuType::Selection: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::openUrl), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copyUrl), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::urlSep), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::saveImage), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::imageSep), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copySelection), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::selectAll), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::history), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clearSep), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clear), 0, ACTS_HIDDEN);
        IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, act::popup::popup, ctrl->getCntId()), TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, 0, UIActionHandleDirect(sUIAction(0, IMIG_MSGWND, ctrl->getCntId()))));
        break;
      }
    }
    switch (Controller::getInstance()->selectedMenuItem) {
      case act::popup::openUrl: {
        return IECtrl::PopupMenuListener::MakeAction::OpenLink;
      }
      case act::popup::copyUrl: {
        return IECtrl::PopupMenuListener::MakeAction::CopyLink;
      }
      case act::popup::saveImage: {
        return IECtrl::PopupMenuListener::MakeAction::SaveImage;
      }
      case act::popup::copySelection: {
        return IECtrl::PopupMenuListener::MakeAction::CopySelection;
      }
      case act::popup::selectAll: {
        return IECtrl::PopupMenuListener::MakeAction::SelectAll;
      }
      case act::popup::history: {
        return IECtrl::PopupMenuListener::MakeAction::None;
      }
      case act::popup::clear: {
        ctrl->clear();
        return IECtrl::PopupMenuListener::MakeAction::None;
      }
    }
  }
}