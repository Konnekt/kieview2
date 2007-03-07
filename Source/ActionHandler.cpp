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
    Ctrl->IMessage(im::anchorClick, NET_BROADCAST, -1, wndCtrl->getCntID(), (int) url);
    ShellExecute(GetDesktopWindow(), "open", url, 0, 0, SW_SHOWNORMAL);
  }

  void ActionHandler::fileDropped(const char *url, IECtrl* pCtrl) {
    Ctrl->IMessage(im::fileDrop, NET_BROADCAST, -1, wndCtrl->getCntID(), (int) url);
  }

  int ActionHandler::showMessage(HWND hWnd, const char* lpText, DWORD dwType) { 
    return MessageBox(hWnd, lpText, "[kIEview2] Wiadomoœæ od skryptu", dwType); 
  }

  bool ActionHandler::keyDown(UINT uCode, DWORD dwFlags) {
    if (HIBYTE(GetKeyState(VK_CONTROL)) && !HIBYTE(GetKeyState(VK_MENU))) {
      if (uCode == 'C' || uCode == 'c') {
        if (m_pCtrl) m_pCtrl->copySelection();
      } else if (uCode == 'A' || uCode == 'a') {
        if (m_pCtrl) m_pCtrl->selectAll();
      }
      return false;
    }
    return true;
  }

  ActionHandler::tMenuAction ActionHandler::popupMenu(tMenuType type, POINT pt, IECtrl* pCtrl) {
    tCntId cntID = wndCtrl->getCntID();

    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::url::open), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::url::copy), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::img::save), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::sel::copy), -1, ACTS_HIDDEN);

    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::sep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::print), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::selectAll), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::showSource), 0, ACTS_HIDDEN);

    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::pasteSessionSep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::pasteSession), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::historySep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::lastMsgs), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::lastSession), -1, ACTS_HIDDEN);

    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clearSep), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clear), 0, ACTS_HIDDEN);

    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::to), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::sep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::up), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::down), -1, ACTS_HIDDEN);

    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::up), 0, ACTS_DISABLED);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::down), 0, ACTS_DISABLED);

    switch (type) {
      case tMenuType::Anchor: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::url::open), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::url::copy), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::sep), 0, ACTS_HIDDEN);
        break;
      }
      case tMenuType::Image: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::img::save), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::sep), 0, ACTS_HIDDEN);
        break;
      }
      case tMenuType::Selection: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::sel::copy), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::sep), 0, ACTS_HIDDEN);
        break;
      }
      case tMenuType::Scroll: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::to), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::sep), 0, ACTS_HIDDEN);

        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::down), 0, ACTS_HIDDEN);
        if (pCtrl->isScrollOnBottom()) {
          UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::down), -1, ACTS_DISABLED);
        }
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::up), 0, ACTS_HIDDEN);
        if (pCtrl->isScrollOnTop()) {
          UIActionSetStatus(sUIAction(act::popup::popup, act::popup::scroll::up), -1, ACTS_DISABLED);
        }

        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::selectAll), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::print), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::showSource), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clearSep), -1, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clear), -1, ACTS_HIDDEN);
        break;
      }
      default: {
        if (cntID) {
          UIActionSetStatus(sUIAction(act::popup::popup, act::popup::pasteSessionSep), 0, ACTS_HIDDEN);
          UIActionSetStatus(sUIAction(act::popup::popup, act::popup::pasteSession), 0, ACTS_HIDDEN);
          UIActionSetStatus(sUIAction(act::popup::popup, act::popup::historySep), 0, ACTS_HIDDEN);
          UIActionSetStatus(sUIAction(act::popup::popup, act::popup::lastSession), 0, ACTS_HIDDEN);
          UIActionSetStatus(sUIAction(act::popup::popup, act::popup::lastMsgs), 0, ACTS_HIDDEN);
        }
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
      case act::popup::url::open: {
        return tMenuAction::OpenLink;
      }
      case act::popup::url::copy: {
        return tMenuAction::CopyLink;
      }
      case act::popup::img::save: {
        return tMenuAction::SaveImage;
      }
      case act::popup::sel::copy: {
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
            wndCtrl->getSession() && wndCtrl->pasteSession, wndCtrl->pasteSession);
        }
        break;
      }
      case act::popup::lastSession: {
        if (cntID) {
          Controller::getInstance()->readLastMsgSession(cntID, wndCtrl->getSession() && wndCtrl->pasteSession, wndCtrl->pasteSession);
        }
        break;
      }
      case act::popup::clear: {
        if (cntID) wndCtrl->clearWnd();
        break;
      }
      case act::popup::scroll::up: {
        pCtrl->scrollToTop();
        break;
      }
      case act::popup::scroll::down: {
        pCtrl->scrollToBottom();
        break;
      }
      case act::popup::scroll::to: {
        ScreenToClient(pCtrl->getHWND(), &pt);
        pCtrl->setScrollPosByMouse(-1, pt.y);
      }
    }
    return tMenuAction::None;
  }
}