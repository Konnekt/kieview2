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
#include "Controller.h"

void xor1_encrypt(const unsigned char * key , unsigned char * data , unsigned int size) {
	unsigned int ki=0;
	if (!size) size = strlen((char *)data);
	unsigned int ksize = strlen((char *)key);
	int j = 0;
	for (unsigned int p=0;p<size;p++) {
		*data = (*data ^ key[ki]) + (unsigned char)((j) &  0xFF);// | (j*2);
		//    *data = *data;
		data++;
		ki++;
		if (ki>=ksize) ki=0;
		j++;
	}
}

void xor1_decrypt(const unsigned char * key , unsigned char * data , unsigned int size) {
	unsigned int ki=0;
	unsigned int ksize = strlen((char *)key);

	int j = 0;
	for (unsigned int p=0;p<size;p++) {
		*data = (*data - (unsigned char)((j) & 0xFF))  ^ key[ki];// | (j*2);
		data++;
		ki++;
		if (ki>=ksize) ki=0;
		j++;
	}
}

namespace kIEview2 {
  void ActionHandler::AnchorClicked(const char* url, IECtrl* ctrl) {
    ShellExecute(GetDesktopWindow(), "open", url, 0, 0, SW_SHOWNORMAL);
  }

  void ActionHandler::FileDropped(const char *url, IECtrl* ctrl) {
    Ctrl->IMessage(im::fileDrop, NET_BROADCAST, -1, cntId, (int)url);
  }

  int ActionHandler::ShowMessage(HWND hWnd, const char* lpText, DWORD dwType) { 
    return MessageBox(hWnd, lpText, "[kIEview2] Wiadomoœæ od skryptu", dwType); 
  }

  IECtrl::PopupMenuListener::MakeAction ActionHandler::PopupMenu(IECtrl::PopupMenuListener::MenuType type, POINT pt, IECtrl* ctrl) {
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::openUrl), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copyUrl), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::urlSep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::saveImage), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::imageSep), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::copySelection), -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::print), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::selectAll), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::showSource), 0, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::history), cntId ? 0 : -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clearSep), cntId ? 0 : -1, ACTS_HIDDEN);
    UIActionSetStatus(sUIAction(act::popup::popup, act::popup::clear), cntId ? 0 : -1, ACTS_HIDDEN);

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

    if (cntId) {
      IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, act::popup::popup, cntId), TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
        pt.x, pt.y, 0, UIActionHandleDirect(sUIAction(0, IMIG_MSGWND, cntId))));
    } else {
      IMessage(&sIMessage_UIMakePopup(sUIAction(IMIG_MSGWND, act::popup::popup, -1), TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
        pt.x, pt.y, 0, UIActionHandleDirect(sUIAction(0, IMIG_HISTORYWND, -1))));
    }

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
        if (cntId) {
          Tables::oTable& table = Controller::getInstance()->historyTable;
          string dir = (LPCTSTR)ICMessage(IMC_PROFILEDIR);
          dir += "history\\messages\\";
          string file = "u";
          file += GETCNTC(this->cntId, CNT_UID);
          file += '.';
          file += inttostr(GETCNTI(this->cntId, CNT_NET));
          file += ".dtb";
          table->setDirectory(dir.c_str());
          table->setFilename(file.c_str());
          table->load(true);

          unsigned count = table->getRowCount();
          for(unsigned i = 0; i < count; i++)
          {
            cMessage msg;
            msg.action = NOACTION;
            msg.notify = 0;
            msg.id = table->getInt(table->getRowId(i), table->getColIdByPos(0));
            msg.net = table->getInt(table->getRowId(i), table->getColIdByPos(1));
            msg.type = table->getInt(table->getRowId(i), table->getColIdByPos(2));
            msg.fromUid = strdup(table->getStr(table->getRowId(i), table->getColIdByPos(3)));
            xor1_encrypt((unsigned char*)"\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84", (unsigned char*)msg.fromUid, strlen(msg.fromUid));
            xor1_decrypt((unsigned char*)"\x40\x13\xf8\xb2\x84\x23\x04\xae\x6f\x3d", (unsigned char*)msg.fromUid, strlen(msg.fromUid));
            msg.toUid = strdup(table->getStr(table->getRowId(i), table->getColIdByPos(4)));
            xor1_encrypt((unsigned char*)"\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84", (unsigned char*)msg.toUid, strlen(msg.toUid));
            xor1_decrypt((unsigned char*)"\x40\x13\xf8\xb2\x84\x23\x04\xae\x6f\x3d", (unsigned char*)msg.toUid, strlen(msg.toUid));
            msg.body = strdup(table->getStr(table->getRowId(i), table->getColIdByPos(5)));
            xor1_encrypt((unsigned char*)"\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84", (unsigned char*)msg.body, strlen(msg.body));
            xor1_decrypt((unsigned char*)"\x40\x13\xf8\xb2\x84\x23\x04\xae\x6f\x3d", (unsigned char*)msg.body, strlen(msg.body));
            msg.ext = strdup(table->getStr(table->getRowId(i), table->getColIdByPos(6)));
            xor1_encrypt((unsigned char*)"\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84", (unsigned char*)msg.ext, strlen(msg.ext));
            xor1_decrypt((unsigned char*)"\x40\x13\xf8\xb2\x84\x23\x04\xae\x6f\x3d", (unsigned char*)msg.ext, strlen(msg.ext));
            msg.flag = table->getInt(table->getRowId(i), table->getColIdByPos(7));
            msg.time = table->getInt64(table->getRowId(i), table->getColIdByPos(8));
            UI::Notify::_insertMsg iMsg(&msg, strdup(table->getStr(table->getRowId(i), table->getColIdByPos(9))), false);
            xor1_encrypt((unsigned char*)"\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84", (unsigned char*)iMsg._display, strlen(iMsg._display));
            xor1_decrypt((unsigned char*)"\x40\x13\xf8\xb2\x84\x23\x04\xae\x6f\x3d", (unsigned char*)iMsg._display, strlen(iMsg._display));
            iMsg.act = sUIAction(IMIG_MSGWND, Konnekt::UI::ACT::msg_ctrlview, cntId);
            Controller::getInstance()->process((sIMessage_base*)&iMsg);
            delete [] msg.fromUid;
            delete [] msg.toUid;
            delete [] msg.body;
            delete [] msg.ext;
            delete [] iMsg._display;
          }
        }
        break;
      }
      case act::popup::clear: {
        if (cntId) {
          Controller::getInstance()->clearWnd(ctrl);
        }
        break;
      }
    }
    return MakeAction::None;
  }
}