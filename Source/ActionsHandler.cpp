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

#include "stdafx.h"
#include "ActionsHandler.h"
#include "Controller.h"

namespace kIEview2 {
  void ActionsHandler::AnchorClicked(const char* url, IECtrl* ctrl) {
    ShellExecute(GetDesktopWindow(), "open", url, 0, 0, SW_SHOWNORMAL);
  }

  void ActionsHandler::FileDropped(const char *url, IECtrl* ctrl) {
    Ctrl->IMessage(im::fileDrop, NET_BROADCAST, -1, (int)url);
  }

  int ActionsHandler::ShowMessage(HWND hWnd, const char* lpText, DWORD dwType) { 
    return MessageBox(hWnd, lpText, "[kIEview2] Wiadomoœæ od skryptu", dwType); 
  }

  IECtrl::PopupMenuListener::MakeAction ActionsHandler::PopupMenu(IECtrl::PopupMenuListener::MenuType type, POINT pt, IECtrl* ctrl) {
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::openUrl), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copyUrl), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::urlSep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::saveImage), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::imageSep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copySelection), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::print), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::selectAll), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::showSource), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::history), ctrl->getCntId() ? 0 : -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clearSep), ctrl->getCntId() ? 0 : -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clear), ctrl->getCntId() ? 0 : -1, ACTS_HIDDEN);

    switch (type) {
      case IECtrl::PopupMenuListener::MenuType::Anchor: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::openUrl), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copyUrl), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::urlSep), 0, ACTS_HIDDEN);
        break;
      }
      case IECtrl::PopupMenuListener::MenuType::Image: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::saveImage), 0, ACTS_HIDDEN);
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::imageSep), 0, ACTS_HIDDEN);
        break;
      }
      case IECtrl::PopupMenuListener::MenuType::Selection: {
        UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copySelection), 0, ACTS_HIDDEN);
        break;
      }
    }

    IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, act::popup::popup, ctrl->getCntId()), TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
      pt.x, pt.y, 0, UIActionHandleDirect(sUIAction(0, IMIG_MSGWND, ctrl->getCntId()))));

    switch (this->selectedMenuItem) {
      case act::popup::openUrl: {
        return MakeAction::OpenLink;
      }
      case act::popup::copyUrl: {
        return MakeAction::CopyLink;
      }
      case act::popup::saveImage: {
        return MakeAction::SaveImage;
      }
      case act::popup::copySelection: {
        return MakeAction::CopySelection;
      }
      case act::popup::print: {
        return MakeAction::Print;
      }
      case act::popup::selectAll: {
        return MakeAction::SelectAll;
      }
      case act::popup::showSource: {
        return MakeAction::ShowSource;
      }
      case act::popup::history: {
        if (ctrl->getCntId()) {
          // TODO: Przywracamy ostatni¹ rozmowê.
        }
        break;
      }
      case act::popup::clear: {
        if (ctrl->getCntId()) {
          Controller::getInstance()->clearWnd(ctrl);
        }
        break;
      }
    }
    return MakeAction::None;
  }
}