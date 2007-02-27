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

#include "stdafx.h"

#include "ActionHandler.h"
#include "WndController.h"
#include "Controller.h"

namespace kIEview2 {
  void ActionHandler::anchorClicked(const char* url, IECtrl* pCtrl) {
    Ctrl->IMessage(im::anchorClick, NET_BROADCAST, -1, wndCtrl->getCntID(), (int)url);
    ShellExecute(GetDesktopWindow(), "open", url, 0, 0, SW_SHOWNORMAL);
  }

  void ActionHandler::fileDropped(const char *url, IECtrl* pCtrl) {
    Ctrl->IMessage(im::fileDrop, NET_BROADCAST, -1, wndCtrl->getCntID(), (int)url);
  }

  int ActionHandler::showMessage(HWND hWnd, const char* lpText, DWORD dwType) { 
    return MessageBox(hWnd, lpText, "[kIEview2] Wiadomoœæ od skryptu", dwType); 
  }

  ActionHandler::tMenuAction ActionHandler::popupMenu(tMenuType type, POINT pt, IECtrl* pCtrl) {
    tCntId cntID = wndCtrl->getCntID();

    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::openUrl), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copyUrl), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::urlSep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::saveImage), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::imageSep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copySelection), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::print), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::selectAll), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::showSource), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::historySep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::lastMsgs), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::lastSession), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clearSep), cntID ? 0 : -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clear), cntID ? 0 : -1, ACTS_HIDDEN);

    switch (type) {
      case tMenuType::Anchor: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::openUrl), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copyUrl), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::urlSep), 0, ACTS_HIDDEN);
        break;
      }
      case tMenuType::Image: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::saveImage), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::imageSep), 0, ACTS_HIDDEN);
        break;
      }
      case tMenuType::Selection: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copySelection), 0, ACTS_HIDDEN);
        break;
      }
      default: {
        if (!cntID) break;

        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::historySep), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::lastMsgs), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::lastSession), 0, ACTS_HIDDEN);
        break;
      }
    }

    if (cntID) {
      Ctrl->IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, act::popup::popup, cntID), TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
        pt.x, pt.y, 0, UIActionHandleDirect(sUIAction(0, IMIG_MSGWND, cntID))));
    } else {
      Ctrl->IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, act::popup::popup, -1), TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
        pt.x, pt.y, 0, UIActionHandleDirect(sUIAction(0, IMIG_HISTORYWND, -1))));
    }

    switch (this->selectedMenuItem) {
      case act::popup::openUrl: {
        return tMenuAction::OpenLink;
      }
      case act::popup::copyUrl: {
        return tMenuAction::CopyLink;
      }
      case act::popup::saveImage: {
        return tMenuAction::SaveImage;
      }
      case act::popup::copySelection: {
        return tMenuAction::CopySelection;
      }
      case act::popup::print: {
        return tMenuAction::Print;
      }
      case act::popup::selectAll: {
        return tMenuAction::SelectAll;
      }
      case act::popup::showSource: {
        return tMenuAction::ShowSource;
      }
      case act::popup::lastMsgs: {
        if (cntID) {
          Controller::getInstance()->readMsgs(cntID, Controller::getConfig()->getInt(cfg::lastMsgCount), 
            GetProp(GetParent(pCtrl->getHWND()), "MsgSession") ? 1 : 0);
        }
        break;
      }
      case act::popup::lastSession: {
        if (cntID) {
          Controller::getInstance()->readLastMsgSession(cntID, GetProp(GetParent(pCtrl->getHWND()), "MsgSession") ? 1 : 0);
        }
        break;
      }
      case act::popup::clear: {
        if (cntID) {
          wndCtrl->clearWnd();
        }
        break;
      }
    }
    return tMenuAction::None;
  }
}