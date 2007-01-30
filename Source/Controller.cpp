/**
  *  kIEview2 Controller class
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
#include "Controller.h"
#include "Message.h"
#include "TplUdf.h"

void xor1_encrypt(const unsigned char* key, unsigned char* data, unsigned int size) {
  unsigned int ksize = strlen((char*)key);
  unsigned int ki = 0;

  if (!size) {
    size = strlen((char*)data);
  }

  int j = 0;
  for (unsigned int p = 0; p < size; p++) {
    *data = (*data ^ key[ki]) + (unsigned char)((j) & 0xFF); // | (j * 2);
    data++;
    ki++;
    if (ki >= ksize) ki = 0;
    j++;
  }
}

void xor1_decrypt(const unsigned char* key, unsigned char* data, unsigned int size) {
  unsigned int ksize = strlen((char*)key);
  unsigned int ki = 0;

  int j = 0;
  for (unsigned int p = 0; p < size; p++) {
    *data = (*data - (unsigned char)((j) & 0xFF)) ^ key[ki]; // | (j * 2);
    data++;
    ki++;
    if (ki >= ksize) ki = 0;
    j++;
  }
}

namespace kIEview2 {
  // initialization
  Controller::Controller() {
    ieVersion = getIEVersion();

    IECtrl::getExternal()->bindMethod("oController", bind(&Controller::getJSController, this, _1, _2), true);
    IECtrl::getExternal()->bindMethod("oWindow", bind(&Controller::getJSWndController, this, _1, _2), true);
    jsController = NULL;

    /* Static values like net, type or version */
    this->setStaticValue(IM_PLUG_TYPE, IMT_CONFIG | IMT_MSGUI | IMT_UI);
    this->setStaticValue(IM_PLUG_PRIORITY, PLUGP_HIGH + 1);
    this->setStaticValue(IM_PLUG_NAME, (int) "kIEview2");
    this->setStaticValue(IM_PLUG_SIG, (int) sig);
    this->setStaticValue(IM_PLUG_NET, net);

    /* Callbacks */
    this->registerObserver(IM_ALLPLUGINSINITIALIZED, bind(resolve_cast0(&Controller::_onPluginsLoaded), this));
    this->registerObserver(IM_UI_PREPARE, bind(resolve_cast0(&Controller::_onPrepare), this));
    this->registerObserver(IM_UIACTION, bind(resolve_cast0(&Controller::_onAction), this));
    this->registerObserver(IM_CFG_CHANGED, bind(resolve_cast0(&Controller::_onCfgChanged), this));
    this->registerActionObserver(UI::ACT::msg_ctrlview, bind(resolve_cast0(&Controller::_msgCtrlView), this));
    this->registerActionObserver(UI::ACT::msg_ctrlsend, bind(resolve_cast0(&Controller::_msgCtrlSend), this));

    /* Configuration columns */
    config->setColumn(DTCFG, cfg::showFormatTb, DT_CT_INT, 1, "kIEview2/showFormatTb");
    config->setColumn(DTCFG, cfg::linkify, DT_CT_INT, 1, "kIEview2/linkify/use");
    config->setColumn(DTCFG, cfg::useEmots, DT_CT_INT, 1, "kIEview2/emots/use");
    config->setColumn(DTCFG, cfg::emotsDir, DT_CT_STR, "emots", "kIEview2/emots/dir");
    config->setColumn(DTCFG, cfg::emotsPack, DT_CT_STR, "", "kIEview2/emots/pack");
    config->setColumn(DTCFG, cfg::autoScroll, DT_CT_INT, 1, "kIEview2/autoScroll");

    this->subclassAction(UI::ACT::msg_ctrlview, IMIG_MSGWND);
    this->subclassAction(UI::ACT::msg_ctrlview, IMIG_HISTORYWND);
    this->subclassAction(UI::ACT::msg_ctrlsend, IMIG_MSGWND);

    IECtrl::init();
    setlocale(LC_ALL, "polish");

    this->tplHandler = new TplHandler;
    this->rtfHtml = new RtfHtmlTag;

    tplHandler->bindStdFunctions();
    tplHandler->bindUdf("getCntSetting", new udf_get_cnt_setting);
    tplHandler->bindUdf("getSetting", new udf_get_cfg_setting);

    tplHandler->bindUdf("formatString", new udf_stringf);
    tplHandler->bindUdf("formatTime", new udf_strftime);

    tplHandler->bindUdf("getExtParam", new udf_get_ext_param);
    tplHandler->bindUdf("replace", new udf_replace);
    tplHandler->bindUdf("match?", new udf_match);

    registerMsgHandler(MT_QUICKEVENT, bind(&Controller::_handleQuickEventTpl, this, _1, _2), "quickevent");
    registerMsgHandler(MT_MESSAGE, bind(&Controller::_handleStdMsgTpl, this, _1, _2), "message");
    registerMsgHandler(MT_FILE, bind(&Controller::_handleFileTpl, this, _1, _2), "file");
    registerMsgHandler(MT_SMS, bind(&Controller::_handleSmsTpl, this, _1, _2), "sms");

  }

  Controller::~Controller() {
    wndObjCollection.clear();

    if (jsController) {
      delete jsController;
    }
    for (tMsgHandlers::iterator it = msgHandlers.begin(); it != msgHandlers.end(); it++) {
      delete it->second;
    }

    delete tplHandler;
    delete rtfHtml;

    IECtrl::deinit();
  }

  void Controller::_onPluginsLoaded() {
    if (getIEVersion() < 6) {
      Ctrl->IMessage(&sIMessage_plugOut(Ctrl->ID(), "kIEview2 potrzebuje do dzia³ania conajmniej IE6.",
        sIMessage_plugOut::erNo, sIMessage_plugOut::euNowAndOnNextStart));
      return setFailure();
    }
    setSuccess();
  }

  void Controller::_onPrepare() {
    historyTable = Tables::registerTable(Ctrl, tableNotFound, optPrivate);
    IECtrl::setAutoCopySel(config->getInt(CFG_UIMSGVIEW_COPY));

    // @debug replace with user selected tpl directory
    tplHandler->setKonnektPath((char*) Ctrl->ICMessage(IMC_KONNEKTDIR));
    tplHandler->addTplDir("/data/templates/core/");

    IconRegister(IML_16, ico::logo, Ctrl->hDll(), IDI_LOGO);
    IconRegister(IML_16, ico::link, Ctrl->hDll(), IDI_LINK);
    IconRegister(IML_16, ico::copy, Ctrl->hDll(), IDI_COPY);
    IconRegister(IML_16, ico::save, Ctrl->hDll(), IDI_SAVE);
    IconRegister(IML_16, ico::emots, Ctrl->hDll(), IDI_EMOTS);
    IconRegister(IML_16, ico::bold, Ctrl->hDll(), IDI_BOLD);
    IconRegister(IML_16, ico::italic, Ctrl->hDll(), IDI_ITALIC);
    IconRegister(IML_16, ico::underline, Ctrl->hDll(), IDI_UNDERLINE);
    IconRegister(IML_16, ico::color, Ctrl->hDll(), IDI_COLOR);
    IconRegister(IML_16, ico::print, Ctrl->hDll(), IDI_PRINT);
    IconRegister(IML_16, ico::source, Ctrl->hDll(), IDI_SOURCE);
    IconRegister(IML_16, ico::autoScroll, Ctrl->hDll(), IDI_AUTOSCROLL);

    // menu akcji pod prawym klawiszem myszy
    UIGroupAdd(IMIG_MSGWND, act::popup::popup, ACTR_INIT);
    UIActionAdd(act::popup::popup, act::popup::openUrl, ACTSMENU_BOLD, "Otwórz", ico::link);
    UIActionAdd(act::popup::popup, act::popup::copyUrl, 0, "Kopiuj adres");
    UIActionAdd(act::popup::popup, act::popup::urlSep, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::saveImage, ACTSMENU_BOLD, "Zapisz obrazek", ico::save);
    UIActionAdd(act::popup::popup, act::popup::imageSep, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::print, 0, "Drukuj", ico::print);
    UIActionAdd(act::popup::popup, act::popup::copySelection, 0, "Kopiuj", ico::copy);
    UIActionAdd(act::popup::popup, act::popup::selectAll, 0, "Zaznacz wszystko");
    UIActionAdd(act::popup::popup, act::popup::showSource, 0, "Poka¿ Ÿród³o", ico::source);
    UIActionAdd(act::popup::popup, act::popup::historySep, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::lastMsgs, 0, "Wczytaj z historii");
    UIActionAdd(act::popup::popup, act::popup::lastSession, 0, "Wczytaj ostatni¹ sesjê");
    UIActionAdd(act::popup::popup, act::popup::clearSep, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::clear, 0, "Wyczyœæ okno", 0x74);

    if (config->getInt(cfg::showFormatTb)) {
      UIGroupAdd(IMIG_MSGBAR, act::formatTb::formatTb);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::emots, 0, "Emotikony", ico::emots);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::bold, ACTT_CHECK, "Pogrubienie", ico::bold);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::italic, ACTT_CHECK, "Kursywa", ico::italic);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::underline, ACTT_CHECK, "Podkreœlenie", ico::underline);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::color, ACTT_CHECK, "Kolor", ico::color);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::autoScroll, ACTR_INIT, "Przewijaj", ico::autoScroll);
    }

    UIGroupAdd(IMIG_CFG_PLUGS, ui::cfgGroup, 0, "kIEview2", ico::logo);
    UIActionCfgAddPluginInfoBox2(ui::cfgGroup, 
      "Wtyczka kIEview2 zastêpuje standardowe okno rozmowy Konnekta dziêki czemu mo¿liwe jest "
      "wyœwietlanie emotikon oraz modyfikacja wygl¹du okna przy pomocy styli CSS i skryptów JS.",
      "<span class='note'>Skompilowano: <b>" __DATE__ "</b> [<b>" __TIME__ "</b>]</span><br/>"
      "Informacje o wtyczce na stronie projektu "
      "<b>KPlugins</b> (http://kplugins.net/)<br/><br/>"
      "Copyright © 2006-2007 <b>Sijawusz Pur Rahnama</b><br/>"
      "Copyright © 2006-2007 <b>Micha³ \"Dulek\" Dulko</b><br/>"
      "Copyright © 2005 <b>Kuba \"nix\" Niegowski</b>", Helpers::icon16(ico::logo).a_str(), -3);

    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUP, "Ustawienia");
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK, "Automatycznie przewijaj okno rozmowy", cfg::autoScroll);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_NEEDRESTART, "Wyœwietlaj toolbar w oknie rozmowy", cfg::showFormatTb);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK, "Zamieniaj linki na 'klikalne'", cfg::linkify);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUPEND);

    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUP, "Emotikony");
    UIActionCfgAdd(ui::cfgGroup, cfg::useEmots, ACTT_CHECK, "U¿ywaj emotikon", cfg::useEmots);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_COMMENT, "Katalog w którym znajduj¹ siê pakiety emotikon");
    UIActionCfgAdd(ui::cfgGroup, cfg::emotsDir, ACTT_DIR, "", cfg::emotsDir);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUPEND);
  }

  void Controller::_onAction() {
    // locking
    LockerCS lock(_locker);

    sUIActionNotify* an = this->getAN();

    switch(an->act.id) {
      case act::popup::popup: {
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(
          sUIAction(an->act.cnt != -1 ? IMIG_MSGWND : IMIG_HISTORYWND, UI::ACT::msg_ctrlview, an->act.cnt)
        ));
        if (ctrl) {
          wndObjCollection[ctrl].actionHandler->selectedMenuItem = 0;
        }
        break;
      }
      case act::popup::openUrl:
      case act::popup::copyUrl:
      case act::popup::saveImage:
      case act::popup::copySelection:
      case act::popup::print:
      case act::popup::selectAll:
      case act::popup::showSource:
      case act::popup::lastMsgs:
      case act::popup::lastSession:
      case act::popup::clear: {
        if (an->code != ACTN_ACTION) break;
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(
          sUIAction(an->act.cnt != -1 ? IMIG_MSGWND : IMIG_HISTORYWND, UI::ACT::msg_ctrlview, an->act.cnt)
        ));
        if (ctrl) {
          wndObjCollection[ctrl].actionHandler->selectedMenuItem = an->act.id;
        }
        break;
      }
      case act::formatTb::bold: {
        if (an->code == ACTN_ACTION) {
          handleTextFlag(CFE_BOLD, CFM_BOLD);
        }
        break;
      }
      case act::formatTb::italic: {
        if (an->code == ACTN_ACTION) {
          handleTextFlag(CFE_ITALIC, CFM_ITALIC);
        }
        break;
      }
      case act::formatTb::underline: {
        if (an->code == ACTN_ACTION) {
          handleTextFlag(CFE_UNDERLINE, CFM_UNDERLINE);
        }
        break;
      }
      case act::formatTb::color: {
        if (an->code != ACTN_ACTION) break;

        if (UIActionGetStatus(an->act) & ACTS_CHECKED) {
          //
        } else {
          //
        }
        break;
      }
      case act::formatTb::emots: {
        if (an->code != ACTN_ACTION) break;
        break;
      }
      case act::formatTb::autoScroll: {
        if (an->code == ACTN_ACTION) {
          UIActionSetStatus(an->act, (UIActionGetStatus(an->act) & ACTS_CHECKED) ? 0 : -1, ACTS_CHECKED);
        } else if (an->code == ACTN_CREATE) {
          UIActionSetStatus(an->act, config->getInt(cfg::autoScroll) ? -1 : 0, ACTS_CHECKED);
        }
        break;
      }
      case cfg::useEmots: {
        UIActionSetStatus(sUIAction(ui::cfgGroup, cfg::emotsDir), *UIActionCfgGetValue(an->act, 0, 0) == '0' ? -1 : 0, ACTS_DISABLED);
        break;
      }
    }
  }

  void Controller::_onCfgChanged() {
    IECtrl::setAutoCopySel(config->getInt(CFG_UIMSGVIEW_COPY));
  }

  void Controller::_msgCtrlView() {
    // locking
    LockerCS lock(_locker);

    switch (this->getAN()->code) {
      case ACTN_CREATEWINDOW: {
        sUIActionNotify_createWindow* an = (sUIActionNotify_createWindow*)this->getAN();
        IECtrl* ctrl = new IECtrl(an->hwndParent, an->x, an->y, an->w, an->h);
        an->hwnd = ctrl->getHWND();

        this->oldMsgWndProc = (WNDPROC)SetWindowLong(an->hwndParent, GWL_WNDPROC, (LONG)Controller::msgWndProc);
        SetProp(an->hwndParent, "CntID", (HANDLE)an->act.cnt);
        SetProp(an->hwndParent, "MsgSend", false);

        wndObjCollection[ctrl].actionHandler = new ActionHandler(ctrl, an->act.cnt);

        ctrl->enableSandbox(false);
        this->clearWnd(ctrl);
        break;
      }

      case ACTN_DESTROYWINDOW: {
        sUIActionNotify_destroyWindow* an = (sUIActionNotify_destroyWindow*)this->getAN();
        IECtrl* ctrl = IECtrl::get(an->hwnd);

        for (tWndObjCollection::iterator it = wndObjCollection.begin(); it != wndObjCollection.end(); it++) {
          if (it->first == ctrl) {
            wndObjCollection.erase(it); break;
          }
        }
        delete ctrl;
        break;
      }

      case UI::Notify::insertMsg: {
        UI::Notify::_insertMsg* an = (UI::Notify::_insertMsg*)this->getAN();
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));

        IECtrl::Var args;
        IECtrl::Var ret;

        // ctrl->isScrollOnBottom()
        bool autoScroll = UIActionGetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::autoScroll, an->act.cnt)) & ACTS_CHECKED;
        try {
          args[0] = _parseMsgTpl(an).a_str();
        } catch(const exception &e) { 
          args[0] = tplHandler->parseException(e).a_str();
        }

        Ctrl->Sleep(1000);
        ctrl->callJScript("addMessage", args, &ret);
        if (an->_scroll && autoScroll) ctrl->scrollToBottom();
        break;
      }

      case UI::Notify::insertStatus: {
        UI::Notify::_insertStatus* an = (UI::Notify::_insertStatus*)this->getAN();
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));

        IECtrl::Var args;
        IECtrl::Var ret;

        bool autoScroll = UIActionGetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::autoScroll, an->act.cnt)) & ACTS_CHECKED;
        try {
          args[0] = _parseStatusTpl(an).a_str();
        } catch(const exception &e) { 
          args[0] = tplHandler->parseException(e).a_str();
        }

        Ctrl->Sleep(1000);
        ctrl->callJScript("addStatus", args, &ret);
        if (autoScroll) ctrl->scrollToBottom();
        break;
      }

      case UI::Notify::lock: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*)this->getAN();
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));
        // TODO: Mo¿emy spodziewaæ siê, ¿e zaraz dodamy du¿o danych, np. przy przegl¹daniu historii
        break;
      }

      case UI::Notify::unlock: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*)this->getAN();
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));
        // TODO: Du¿a iloœæ danych zosta³a dodana
        break;
      }

      case UI::Notify::clear: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*)this->getAN();
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));
        this->clearWnd(ctrl);
        break;
      }

      case ACTN_SETCNT: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*)this->getAN();
        an->notify2 = (int)GetDlgItem((HWND)UIActionHandleDirect(sUIAction(0, an->act.parent, an->act.cnt)), UI::ACT::msg_ctrlview);
        break;
      }
    }
  }

  void Controller::_msgCtrlSend() {
    // locking
    LockerCS lock(_locker);

    switch (getAN()->code) {
      case UI::Notify::supportsFormatting: {
        return setSuccess();
      }

      case UI::Notify::getMessageSize: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*) getAN();

        String text;
        EDITSTREAM es;

        es.dwError = 0;
        es.pfnCallback = Controller::streamOut;
        es.dwCookie = (DWORD)&text;
        SendMessage((HWND)UIActionHandleDirect(an->act), EM_STREAMOUT, SF_RTF, (LPARAM)&es);

        text = htmlEscape(text);
        text = rtfHtml->rtfParse((char*)text.a_str(), text.size());

        return setReturnCode(text.size());
      }

      case UI::Notify::getMessage: {
        UI::Notify::_getMessage* an = (UI::Notify::_getMessage*) getAN();

        String text;
        EDITSTREAM es;

        es.dwError = 0;
        es.pfnCallback = Controller::streamOut;
        es.dwCookie = (DWORD)&text;
        SendMessage((HWND)UIActionHandleDirect(an->act), EM_STREAMOUT, SF_RTF, (LPARAM)&es);

        text = htmlEscape(text);
        text = rtfHtml->rtfParse((char*)text.a_str(), text.size());

        strcpy(an->_message->body, text.a_str());
        an->_message->flag |= MF_HTML;

        SetProp(GetParent((HWND)UIActionHandleDirect(an->act)), "MsgSend", (HANDLE) true);
        return;
      }
    }
    forwardAction();
  }

  LRESULT CALLBACK Controller::msgWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) { 
      case WM_NOTIFY: {
        if (wParam == ICMessage(IMI_ACTION_GETINDEX, (int)&sUIAction(IMIG_MSGWND, UI::ACT::msg_ctrlsend), 0)) {
          switch (((NMHDR*)lParam)->code) {
            case EN_SELCHANGE: {
              int cntID = (int) GetProp(hWnd, "CntID");
              HWND hEdit = (HWND)UIActionHandleDirect(sUIAction(IMIG_MSGWND, UI::ACT::msg_ctrlsend, cntID));

              CHARFORMAT cf;
              ZeroMemory(&cf, sizeof(CHARFORMAT));
              cf.cbSize = sizeof(CHARFORMAT);
              cf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_COLOR;
              SendMessage(hEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

              if((cf.dwEffects & CFE_BOLD) && (cf.dwMask & CFM_BOLD)) {
                UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::bold, cntID), -1, ACTS_CHECKED);
              } else {
                UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::bold, cntID), 0, ACTS_CHECKED);
              }
              if((cf.dwEffects & CFE_ITALIC) && (cf.dwMask & CFM_ITALIC)) {
                UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::italic, cntID), -1, ACTS_CHECKED);
              } else {
                UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::italic, cntID), 0, ACTS_CHECKED);
              }
              if((cf.dwEffects & CFE_UNDERLINE) && (cf.dwMask & CFM_UNDERLINE)) {
                UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::underline, cntID), -1, ACTS_CHECKED);
              } else {
                UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::underline, cntID), 0, ACTS_CHECKED);
              }
              break;
            }
          }
        }
        break;
      }
    }
    return CallWindowProc(getInstance()->oldMsgWndProc, hWnd, msg, wParam, lParam);
  }

  IECtrl::Var Controller::getJSWndController(IECtrl::Var& args, IECtrl::iObject* obj) {
    IECtrl* ctrl = obj->getIECtrl();
    if (!wndObjCollection[ctrl].jsWndController) {
      wndObjCollection[ctrl].jsWndController = new JSWndController(ctrl, args);
    }
    return wndObjCollection[ctrl].jsWndController;
  }

  IECtrl::Var Controller::getJSController(IECtrl::Var& args, IECtrl::iObject* obj) {
    if (!jsController) {
      jsController = new JSController(args);
    }
    return jsController;
  }

  bool Controller::hasMsgHandler(int type) {
    // locking
    LockerCS lock(_locker);

    return msgHandlers.find(type) != msgHandlers.end();
  }

  bool Controller::registerMsgHandler(int type, fMessageHandler f, StringRef label, int priority, 
    signals::connect_position pos, bool overwrite) 
  {
    // locking
    LockerCS lock(_locker);

    if (f.empty()) {
      return false;
    }
    if (!hasMsgHandler(type)) {
      msgHandlers[type] = new sMessageHandler;
    }
    if (msgHandlers[type]->connections.find(label) != msgHandlers[type]->connections.end()) {
      if (overwrite) {
        msgHandlers[type]->connections[label].disconnect();
      } else {
        return false;
      }
    }
    msgHandlers[type]->label = label;
    return (msgHandlers[type]->connections[label] = msgHandlers[type]->signal.connect(priority, f, pos)).connected();
  }

  int Controller::getIEVersion() {
    // HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Internet Settings\ -> User Agent
    // Mozilla/4.0 (compatible; MSIE 6.0; Win32)

    int ver = 0;
    HKEY hKey;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
      char userAgent[256] = "";
      DWORD length = 256;
      RegQueryValueEx(hKey, "User Agent", 0, NULL, (BYTE*)&userAgent, &length);

      RegEx preg;
      preg.match("/MSIE +(\\d+)/", userAgent);

      if (preg.isMatched()) {
        ver = atoi(preg[1].c_str());
      }
      RegCloseKey(hKey);
    }
    return ver;
  }

  bool Controller::loadMsgTable(tCntId cnt) {
    if (historyTable->isLoaded()) return false;

    string dir = (const char*) Ctrl->ICMessage(IMC_PROFILEDIR);
    dir += "history\\messages\\";

    string file = "u";
    file += urlEncode(config->getChar(CNT_UID, cnt), '#') + ".";
    file += inttostr(config->getInt(CNT_NET, cnt)) + ".dtb";

    historyTable->setDirectory(dir.c_str());
    historyTable->setFilename(file.c_str());
    historyTable->load(true);

    return true;
  }

  char* Controller::getStringCol(Tables::oTable& table, tRowId row, int pos) {
    const char encryptKey[] = "\x16\x48\xf0\x85\xa9\x12\x03\x98\xbe\xcf\x42\x08\x76\xa5\x22\x84";
    const char decryptKey[] = "\x40\x13\xf8\xb2\x84\x23\x04\xae\x6f\x3d";

    char* resultChar = table->getStr(row, table->getColIdByPos(pos));

    if (table->getColType(table->getColIdByPos(pos)) & cflagXor) {
      xor1_encrypt((unsigned char*)encryptKey, (unsigned char*)resultChar, strlen(resultChar));
      xor1_decrypt((unsigned char*)decryptKey, (unsigned char*)resultChar, strlen(resultChar));

      IMLOG("colPos = %i, value = %s", pos, resultChar);
    }
    return resultChar;
  }

  String Controller::getSettingStr(const string& name, tTable table, tRowId row) {
    tColId col = Ctrl->DTgetNameID(table, name.c_str());
    if (col == colNotFound) {
      throw std::logic_error("Cannot find setting '" + name + "'.");
    }

    int colType = Ctrl->DTgetType(table, col);
    if ((colType & cflagSecret) || (colType & cflagXor)) {
      throw std::logic_error("Access to setting '" + name + "' is forbidden.");
    }

    switch (colType & ctypeMask) {
      case ctypeInt64: return i64tostr(Ctrl->DTgetInt64(table, row, col));
      case ctypeInt: return inttostr(Ctrl->DTgetInt(table, row, col));
      case ctypeString: return Ctrl->DTgetStr(table, row, col);
    }
    throw std::logic_error("Unknown column type in '" + name + "'.");
  }

  int Controller::readMsgs(tCntId cnt, int howMany, int sessionOffset) {
    if (!howMany) {
      Message::quickEvent(cnt, "Brak wiadomoœci do wczytania.", false, false, true);
      return 0;
    }

    // locking
    LockerCS lock(_locker);

    IMLOG("[Controller::readMsgs()]: cnt = %i, howMany = %i, sessionOffset = %i",
      cnt, howMany, sessionOffset);

    Tables::oTable table = historyTable;
    bool dataLoaded = loadMsgTable(cnt);

    list<UI::Notify::_insertMsg> msgs;
    int m = 0;

    for (int i = table->getRowCount() - 1, s = 0; (i >= 0) && (m < howMany); i--) {
      if (sessionOffset) {
        if (!table->getInt(i, table->getColIdByPos(fieldSession))) {
          s++;
        }
        if (s <= sessionOffset) {
          continue;
        }
      }
      IMLOG("[Controller::readMsgs()]: i = %i, m = %i, s = %i", i, m, s);

      cMessage* msg = new cMessage;
      msg->id = table->getInt(i, table->getColIdByPos(fieldId));
      msg->net = table->getInt(i, table->getColIdByPos(fieldNet));
      msg->type = table->getInt(i, table->getColIdByPos(fieldType));
      msg->fromUid = getStringCol(table, i, fieldFromUid);
      msg->toUid = getStringCol(table, i, fieldToUid);
      msg->body = getStringCol(table, i, fieldBody);
      msg->ext = getStringCol(table, i, fieldExt);
      msg->flag = table->getInt(i, table->getColIdByPos(fieldFlag));
      msg->time = table->getInt64(i, table->getColIdByPos(fieldTime));

      msgs.push_back(UI::Notify::_insertMsg(msg, getStringCol(table, i, fieldDisplay), false));
      m++;
    }

    if (m) {
      Message::quickEvent(cnt, "Wczytujê wiadomoœci z historii.");
    }

    for (list<UI::Notify::_insertMsg>::reverse_iterator it = msgs.rbegin(); it != msgs.rend(); it++) {
      Message::inject(it->_message, cnt, it->_display, false);

      delete it->_message;
    }

    String msg;
    if (!m) {
      msg = "Nie wczytano ¿adnych wiadomoœci.";
    } else if (m == 1) {
      msg = "Wczytano <b>jedn¹</b> wiadomoœæ.";
    } else {
      msg = "Wczytano <b>" + inttostr(m) + "</b> ostatnich wiadomoœci.";
    }

    Message::quickEvent(cnt, msg, true);

    if (dataLoaded) {
      table->unloadData();
    }
    return m;
  }

  int Controller::readLastMsgSession(tCntId cnt, int sessionOffset) {
    Tables::oTable table = historyTable;
    loadMsgTable(cnt);

    bool skippedSession = sessionOffset ? false : true;
    int howMany = 0;

    for (int i = table->getRowCount() - 1, s = 0; i >= 0; i--) {
      if (sessionOffset) {
        if (!table->getInt(i, table->getColIdByPos(fieldSession))) {
          s++;
        }
        if (s <= sessionOffset) {
          if (s == sessionOffset) {
            howMany++;
          }
          continue;
        } else {
          skippedSession = true;
        }
      } else {
        howMany++;
      }
      if (skippedSession && !table->getInt(i, table->getColIdByPos(fieldSession))) {
        break;
      }
    }

    int msgCount = readMsgs(cnt, howMany, sessionOffset);
    table->unloadData();

    return msgCount;
  }

  void Controller::clearWnd(IECtrl* ctrl) {
    // locking
    LockerCS lock(_locker);

    ctrl->clear();
    SetProp(GetParent(ctrl->getHWND()), "MsgSend", false);

    // We create structure of the data
    param_data data(param_data::HASH);

    try {
      ctrl->write(tplHandler->parseTpl(&data, "skeleton").a_str());
    } catch(const exception &e) {
      ctrl->write("obsra³em siê, przepraszam<br/>");
      ctrl->write(e.what());
    }
  }

  DWORD CALLBACK Controller::streamOut(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb) {
    String* entry = (String*)dwCookie;
    *entry += (char*)pbBuff;
    return 0;
  }

  String Controller::getStatusLabel(int status) {
    String name = "Nieznany (" + inttostr(status) + ")";

    switch (status & ST_MASK) {
      case ST_ONLINE: name = "Dostêpny"; break;
      case ST_CHAT: name = "Pogadam"; break;
      case ST_AWAY: name = "Zaraz wracam"; break;
      case ST_NA: name = "Nieosi¹galny"; break;
      case ST_DND: name = "Nie przeszkadzaæ"; break;
      case ST_HIDDEN: name = "Ukryty"; break;
      case ST_OFFLINE: name = "Niedostêpny"; break;
    }
    return name;
  }

  string Controller::getMsgTypeLabel(int type) {
    string name = "unknown";

    if (msgHandlers.find(type) != msgHandlers.end()) {
      name = msgHandlers[type]->label;
    }
    return name;
  }

  string Controller::bytesToString(double bytes) {
    char buff[20];

    if (bytes >= (1024 * 1024 * 1024)) { // GiB
      sprintf(buff, "%.2f GiB", bytes / (1024 * 1024 * 1024));
    } else if (bytes >= (1024 * 1024)) { // MiB
      sprintf(buff, "%.2f MiB", bytes / (1024 * 1024));
    } else if (bytes >= 1024) { // KiB
      sprintf(buff, "%.2f KiB", bytes / 1024);
    } else {
      sprintf(buff, "%.2f B", bytes);
    }
    return buff;
  }

  string Controller::timeToString(int time) {
    char buff[20];

    if (time >= (60 * 60 * 24)) {
      int days = int(time / (60 * 60 * 24));
      sprintf(buff, "%dd ", days);
      time -= days * 60 * 60 * 24;
    }
    if (time >= (60 * 60)) {
      int hours = int(time / (60 * 60));
      sprintf(buff, "%dh ", hours);
      time -= hours * 60 * 60;
    }
    if (time >= 60) {
      int mins = int(time / 60);
      sprintf(buff, "%dm ", mins);
      time -= mins * 60;
    }
    if (time > 0) {
      sprintf(buff, "%ds ", time);
    }
    return buff;
  }

  tCntId Controller::getCntFromMsg(cMessage* msg) {
    tCntId cnt = 0;

    if (msg->flag & MF_SEND) {
      cnt = Ctrl->ICMessage(IMC_CNT_FIND, msg->net, (int)msg->toUid);
    } else {
      cnt = Ctrl->ICMessage(IMC_CNT_FIND, msg->net, (int)msg->fromUid);
    }
    return cnt;
  }

  String Controller::htmlEscape(StringRef& txt) {
    return tplHandler->runFunc("htmlEscape", txt);
  }

  string __stdcall Controller::linkifyDo(RegEx* reg) {
    RegEx& r = *reg;

    if (r.hasSub(1) && r.hasSub(4)) {
      return r[0];
    }
    return r[1] + "<a href=\"" + ((r[3].find(":") == r[3].npos) ? "http://" : "") + 
      r[2] + "\" class=\"autolink\" target=\"_blank\">" + r[2] + "</a>" + r[4];
  }

  string __stdcall Controller::mailifyDo(RegEx* reg) {
    RegEx& r = *reg;

    if (r.hasSub(1)) {
      return r[0];
    }
    return "<a href=\"mailto:" + r[2] + "\" class=\"autolink\">" + r[2] + "</a>";
  }

  String Controller::linkify(StringRef& txt) {
    txt = RegEx::doReplace("~([\"|']mailto:)?((?:[a-z0-9_'+*$%\\^&!\\.-])+@(?:(?:[a-z0-9-])+\\.)+(?:[a-z]{2,6})+)~i", &Controller::mailifyDo, txt.c_str());
    txt = RegEx::doReplace("~([\"|']|&quot;|&apos;)?((?>([a-z+]{2,}://|www\\.|ftp\\.))(?:[a-z0-9]+(?:\\:[a-z0-9]+)?@)?(?:(?:[a-z](?:[a-z0-9]|(?<!-)-)*[a-z0-9])(?:\\.[a-z](?:[a-z0-9]|(?<!-)-)*[a-z0-9])+|(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))(?:\\:\\d+)?(?:/[^\\\/:?*\"<>|\\s]*[a-z0-9])*/?(?:\\?[a-z0-9_.%]+(?:=[a-z0-9_.%:/+-]*)?(?:&[a-z0-9_.%]+(?:=[a-z0-9_.%:/+-]*)?)*)?(?:#[a-z0-9_%.]+)?)(\\1)?~i", &Controller::linkifyDo, txt.c_str());

    return PassStringRef(txt);
  }

  String Controller::normalizeSpaces(StringRef& txt) {
    txt = RegEx::doReplace("/(  )+/U", "&nbsp;", txt.c_str());
    txt = RegEx::doReplace("/\t/", "&nbsp;&nbsp;", txt.c_str());
    return PassStringRef(txt);
  }

  String Controller::nl2br(StringRef& txt) {
    txt = RegEx::doReplace("/\r?\n/m", "<br />\r\n", txt.c_str());
    return PassStringRef(txt);
  }

  String Controller::getDisplayFromMsg(UI::Notify::_insertMsg* an) {
    cMessage* msg = an->_message;
    String display = GetExtParam(msg->ext, MEX_DISPLAY);
    tCntId cnt = getCntFromMsg(msg);

    if (display.length()) {
      return display;
    }
    if (an->_display && strlen(an->_display)) {
      display = an->_display;
    } else {
      if (msg->flag & MF_SEND) {
        display = !strlen(config->getChar(CNT_DISPLAY, 0)) ? "Ja" : config->getChar(CNT_DISPLAY, 0);
      } else {
        display = !strlen(config->getChar(CNT_DISPLAY, cnt)) ? msg->fromUid : config->getChar(CNT_DISPLAY, cnt);
      }
    }
    return display;
  }

  bool Controller::isMsgFromHistory(sUIActionNotify_base* an) {
    return an->act.parent != IMIG_MSGWND;
  }

  void Controller::handleTextFlag(int flag, int mask) {
    HWND hwnd = (HWND) UIActionHandleDirect(sUIAction(IMIG_MSGWND, UI::ACT::msg_ctrlsend, getAN()->act.cnt));
    CHARFORMAT cf;
    ZeroMemory(&cf, sizeof(CHARFORMAT));
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = mask;
    DWORD dwSelMask = SendMessage(hwnd, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

    if ((cf.dwMask & mask) && (dwSelMask & mask)) {
      cf.dwEffects ^= flag;
    } else {
      cf.dwEffects |= flag;
    }
    cf.dwMask = mask;
    SendMessage(hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
  }

  String Controller::_parseStatusTpl(UI::Notify::_insertStatus* an) {
    Date64 date(true);

    // We create structure of the data
    param_data data(param_data::HASH);
    data.hash_insert_new_var("@time", i64tostr(date.getInt64()));
    data.hash_insert_new_var("time", date.strftime("%H:%M"));
    data.hash_insert_new_var("status", getStatusLabel(an->_status));

    if (an->_info) {
      String info = an->_info;
      info = htmlEscape(info);

      if (config->getInt(cfg::linkify)) {
        info = linkify(info);
      }
      info = normalizeSpaces(info);
      data.hash_insert_new_var("info", info);
    }
    if (an->_status & ST_IGNORED) {
      data.hash_insert_new_var("ignored?", "1");
    }
    return tplHandler->parseTpl(&data, "status");
  }

  String Controller::_parseMsgTpl(UI::Notify::_insertMsg* an) {
    // locking
    LockerCS lock(_locker);

    if (!isMsgFromHistory(an) && (an->_message->flag & MF_HIDE)) {
      return "";
    }

    cMessage* msg = an->_message;
    string type = getMsgTypeLabel(msg->type);
    String body = msg->body;
    tCntId cnt = getCntFromMsg(msg);
    Date64 date(msg->time);

    // @todo MF_LEAVEASIS

    if (msg->flag & MF_HTML) {
      // body = makeSafeHtml(body);
    } else {
      body = htmlEscape(body);
      body = nl2br(body);
    }
    if (config->getInt(cfg::linkify)) {
      body = linkify(body);
    }
    body = normalizeSpaces(body);

    // We create structure of the data
    param_data data(param_data::HASH);
    data.hash_insert_new_var("@time", i64tostr(date.getInt64()));
    data.hash_insert_new_var("@id", inttostr(msg->id));
    data.hash_insert_new_var("@cnt", inttostr(cnt));

    data.hash_insert_new_var("display", getDisplayFromMsg(an));
    data.hash_insert_new_var("type", type);
    data.hash_insert_new_var("ext", msg->ext);
    data.hash_insert_new_var("time", date.strftime("%H:%M"));
    data.hash_insert_new_var("body", body);

    if (msgHandlers.find(msg->type) != msgHandlers.end()) {
      msgHandlers[msg->type]->signal(data, an);
    }
    return tplHandler->parseTpl(&data, ("content-types/" + type).c_str());
  }

  /*
   * Message types specific methods
   */
  void Controller::_handleQuickEventTpl(param_data& data, UI::Notify::_insertMsg* an) {
    if (an->_message->flag & MF_QE_SHOWTIME) {
      data.hash_insert_new_var("showTime?", "1");
    }
    if (!(an->_message->flag & MF_QE_NORMAL)) {
      data.hash_insert_new_var("warning?", "1");
    }
  }

  void Controller::_handleStdMsgTpl(param_data& data, UI::Notify::_insertMsg* an) {
    string extInfo = GetExtParam(an->_message->ext, MEX_ADDINFO);
    String title = GetExtParam(an->_message->ext, MEX_TITLE);
    tCntId cnt = getCntFromMsg(an->_message);

    data.hash_insert_new_var("@net", inttostr(an->_message->net));
    data.hash_insert_new_var("uid", config->getChar(CNT_UID, cnt));
    data.hash_insert_new_var("nick", config->getChar(CNT_NICK, cnt));
    data.hash_insert_new_var("name", config->getChar(CNT_NAME, cnt));
    data.hash_insert_new_var("surname", config->getChar(CNT_SURNAME, cnt));

    if (extInfo.length()) {
      data.hash_insert_new_var("extInfo", extInfo);
    }
    if (title.length()) {
      data.hash_insert_new_var("title", title);
    }
    if (isMsgFromHistory(an)) {
      data.hash_insert_new_var("inHistory?", "1");
    }
    if (an->_message->flag & MF_SEND) {
      data.hash_insert_new_var("sent?", "1");
    }
  }

  void Controller::_handleSmsTpl(param_data& data, UI::Notify::_insertMsg* an) {
    String from = GetExtParam(an->_message->ext, Sms::extFrom);
    string gate = GetExtParam(an->_message->ext, Sms::extGate);

    if (from.length()) {
      data.hash_insert_new_var("from", from);
    }
    data.hash_insert_new_var("to", an->_message->toUid);
    data.hash_insert_new_var("gate", gate);
  }

  void Controller::_handleFileTpl(param_data& data, UI::Notify::_insertMsg* an) {
    int transferTime = atoi(GetExtParam(an->_message->ext, MEX_FILE_TRANSFER_TIME).c_str());
    double transfered = atoi(GetExtParam(an->_message->ext, MEX_FILE_TRANSFERED).c_str());
    double size = atoi(GetExtParam(an->_message->ext, MEX_FILE_SIZE).c_str());

    string error = GetExtParam(an->_message->ext, MEX_FILE_ERROR);
    string filePath = GetExtParam(an->_message->ext, MEX_FILE_PATH);
    String name = GetExtParam(an->_message->ext, MEX_TITLE);

    if (transferTime > 0) {
      data.hash_insert_new_var("transferTime", timeToString(transferTime));
    }
    if (transfered > 0) {
      char buff[20];
      if (transfered == size) {
        sprintf(buff, "%d%%", 100);
      } else {
        sprintf(buff, "%.1f%%", (transfered / size) * 100);
      }
      data.hash_insert_new_var("transferedP", buff);
      data.hash_insert_new_var("transfered", bytesToString(transfered));
    }
    if (transferTime && transfered) {
      data.hash_insert_new_var("avgSpeed", bytesToString(transfered / transferTime) + "/s");
    }
    if (size > 0) {
      data.hash_insert_new_var("size", bytesToString(size));
    }
    if (name.length()) {
      data.hash_insert_new_var("name", name);
    }
    if (filePath.length()) {
      int pos = filePath.find_last_of('\\');
      if (pos != -1) {
        data.hash_insert_new_var("fileName", filePath.substr(pos + 1));
        data.hash_insert_new_var("path", filePath.substr(0, pos + 1));
      } else {
        data.hash_insert_new_var("fileName", filePath);
      }
      data.hash_insert_new_var("filePath", filePath);
    }
    if (error.length()) {
      data.hash_insert_new_var("error", error);
    }
    if (an->_message->flag & MF_SEND) {
      data.hash_insert_new_var("sent?", "1");
    }
  }
}
