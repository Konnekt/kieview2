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

namespace kIEview2 {
  Controller::Controller() {
    /* Static values like net, type or version */
    this->setStaticValue(IM_PLUG_TYPE, IMT_CONFIG | IMT_MSGUI | IMT_UI);
    this->setStaticValue(IM_PLUG_PRIORITY, PLUGP_HIGH + 1);
    this->setStaticValue(IM_PLUG_NAME, (int) "kIEview2");
    this->setStaticValue(IM_PLUG_SIG, (int) sig);
    this->setStaticValue(IM_PLUG_NET, net);

    /* Callbacks */
    this->registerObserver(IM_UI_PREPARE, bind(resolve_cast0(&Controller::_onPrepare), this));
    this->registerObserver(IM_UIACTION, bind(resolve_cast0(&Controller::_onAction), this));
    this->registerActionObserver(UI::ACT::msg_ctrlview, bind(resolve_cast0(&Controller::_msgCtrlView), this));
    this->registerActionObserver(UI::ACT::msg_ctrlsend, bind(resolve_cast0(&Controller::_msgCtrlSend), this));
    this->registerActionObserver(IMIA_MSG_SEND, bind(resolve_cast0(&Controller::_msgSend), this));

    /* Configuration columns */
    config->setColumn(DTCFG, cfg::showFormatTb, DT_CT_INT, 1, "kIEview2/showFormatTb");
    config->setColumn(DTCFG, cfg::linkify, DT_CT_INT, 1, "kIEview2/linkify");
    config->setColumn(DTCFG, cfg::useEmots, DT_CT_INT, 1, "kIEview2/emots/use");
    config->setColumn(DTCFG, cfg::emotsDir, DT_CT_STR, "emots", "kIEview2/emots/dir");
    config->setColumn(DTCFG, cfg::emotsPack, DT_CT_STR, "", "kIEview2/emots/pack");

    this->subclassAction(UI::ACT::msg_ctrlview, IMIG_MSGWND);
    this->subclassAction(UI::ACT::msg_ctrlview, IMIG_HISTORYWND);
    this->subclassAction(UI::ACT::msg_ctrlsend, IMIG_MSGWND);
    //this->subclassAction(IMIA_MSG_SEND, IMIG_MSGTB);

    IECtrl::init();
    setlocale(LC_ALL, "polish");

    this->tplHandler = new TplHandler;
    this->rtfHtml = new RtfHtmlTag;

    tplHandler->bindStdFunctions();
    tplHandler->bindUdf("getExtParam", new udf_get_ext_param);
    tplHandler->bindUdf("formatTime", new udf_strftime);
    tplHandler->bindUdf("match?", new udf_match);
    tplHandler->bindUdf("replace", new udf_replace);
    tplHandler->bindUdf("sprintf", new udf_sprintf);
  }

  Controller::~Controller() {
    IECtrl::deinit();
    delete tplHandler;
    delete rtfHtml;

    for (tActionHandlers::iterator it = actionHandlers.begin(); it != actionHandlers.end(); it++) {
      delete it->second;
    }
  }

  void Controller::_onPrepare() {
    this->historyTable = Tables::registerTable(Ctrl, Tables::enTableId::dtCnt);

    // @debug replace with user selected tpl directory
    tplHandler->addIncludeDir("./tpl");
    tplHandler->setTplDir("./tpl/");

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
    UIActionAdd(act::popup::popup, act::popup::history, 0, "Poprzednia rozmowa");
    UIActionAdd(act::popup::popup, act::popup::clearSep, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::clear, 0, "Wyczyœæ okno", 0x74);

    if (config->getInt(cfg::showFormatTb)) {
      UIGroupAdd(IMIG_MSGBAR, act::formatTb::formatTb);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::emots, 0, "Emotikony", ico::emots);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::bold, ACTT_CHECK, "Pogrubienie", ico::bold);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::italic, ACTT_CHECK, "Kursywa", ico::italic);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::underline, ACTT_CHECK, "Podkreœlenie", ico::underline);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::color, ACTT_CHECK, "Kolor", ico::color);
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
      "Copyright © 2005 <b>Kuba \"nix\" Niegowski</b>", Helpers::icon16(ico::logo).a_str());

    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUP, "Ustawienia");
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
    sUIActionNotify_2params* an = this->getAN();

    switch(an->act.id) {
      case act::popup::popup: {
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(
          sUIAction(an->act.cnt != -1 ? IMIG_MSGWND : IMIG_HISTORYWND, UI::ACT::msg_ctrlview, an->act.cnt)
        ));
        if (ctrl) {
          actionHandlers[ctrl]->selectedMenuItem = 0;
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
      case act::popup::history:
      case act::popup::clear: {
        if (an->code != ACTN_ACTION) break;
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(
          sUIAction(an->act.cnt != -1 ? IMIG_MSGWND : IMIG_HISTORYWND, UI::ACT::msg_ctrlview, an->act.cnt)
        ));
        if (ctrl) {
          actionHandlers[ctrl]->selectedMenuItem = an->act.id;
        }
        break;
      }
      case act::formatTb::bold: {
        if (an->code == ACTN_ACTION) {
          handleTextFlag(CFM_BOLD);
        }
        break;
      }
      case act::formatTb::italic: {
        if (an->code == ACTN_ACTION) {
          handleTextFlag(CFM_ITALIC);
        }
        break;
      }
      case act::formatTb::underline: {
        if (an->code == ACTN_ACTION) {
          handleTextFlag(CFM_UNDERLINE);
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
      case cfg::useEmots: {
        UIActionSetStatus(sUIAction(ui::cfgGroup, cfg::emotsDir), *UIActionCfgGetValue(an->act, 0, 0) == '0' ? -1 : 0, ACTS_DISABLED);
        break;
      }
    }
  }

  void Controller::_msgCtrlView() {
    switch (this->getAN()->code) {
      case ACTN_CREATEWINDOW: {
        sUIActionNotify_createWindow* an = (sUIActionNotify_createWindow*)this->getAN();
        this->oldMsgWndProc = (WNDPROC)SetWindowLong(an->hwndParent, GWL_WNDPROC, (LONG)Controller::msgWndProc);
        SetProp(an->hwndParent, "CntID", (HANDLE)an->act.cnt);
        IECtrl* ctrl = new IECtrl(an->hwndParent, an->x, an->y, an->w, an->h);
        an->hwnd = ctrl->getHWND();

        actionHandlers[ctrl] = new ActionHandler;
        actionHandlers[ctrl]->cntId = an->act.cnt;

        ctrl->setPopupMenuListener(actionHandlers[ctrl]);
        ctrl->setAnchorClickListener(actionHandlers[ctrl]);
        ctrl->setDropListener(actionHandlers[ctrl]);
        ctrl->setExternalListener(actionHandlers[ctrl]);
        ctrl->setScriptMessageListener(actionHandlers[ctrl]);
        ctrl->setKeyDownListener(actionHandlers[ctrl]);

        ctrl->enableSandbox(false);
        this->clearWnd(ctrl);
        break;
      }

      case ACTN_DESTROYWINDOW: {
        sUIActionNotify_destroyWindow* an = (sUIActionNotify_destroyWindow*)this->getAN();
        IECtrl* ctrl = IECtrl::get(an->hwnd);

        for (tActionHandlers::iterator it = actionHandlers.begin(); it != actionHandlers.end(); it++) {
          if (it->first == ctrl) {
            delete it->second;
            actionHandlers.erase(it);
            break;
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

        try {
          args[0] = _parseMsgTpl(an).a_str();
        } catch(const exception &e) { 
          args[0] = tplHandler->parseException(e).a_str();
        }

        Ctrl->Sleep(1000);
        ctrl->callJScript("addMessage", args, &ret);
        if (an->_scroll) ctrl->scrollToBottom();
        break;
      }

      case UI::Notify::insertStatus: {
        UI::Notify::_insertStatus* an = (UI::Notify::_insertStatus*)this->getAN();
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));

        IECtrl::Var args;
        IECtrl::Var ret;

        try {
          args[0] = _parseStatusTpl(an).a_str();
        } catch(const exception &e) { 
          args[0] = tplHandler->parseException(e).a_str();
        }

        Ctrl->Sleep(1000);
        ctrl->callJScript("addStatus", args, &ret);
        ctrl->scrollToBottom();
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
    switch (getAN()->code) {
      case UI::Notify::supportsFormatting: {
        return setSuccess();
      }

      case UI::Notify::getMessageSize: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*) getAN();

        EDITSTREAM es;
	      es.dwError = 0;
	      es.pfnCallback = Controller::streamOut;
	      String text;
	      es.dwCookie = (DWORD)&text;
	      SendMessage((HWND)UIActionHandleDirect(an->act), EM_STREAMOUT, SF_RTF, (LPARAM)&es);

        return setReturnCode(rtfHtml->rtfParse((char*)text.a_str(), text.size()).size());
      }

      case UI::Notify::getMessage: {
        UI::Notify::_getMessage* an = (UI::Notify::_getMessage*) getAN();

        EDITSTREAM es;
	      es.dwError = 0;
	      es.pfnCallback = Controller::streamOut;
	      String text;
	      es.dwCookie = (DWORD)&text;
	      SendMessage((HWND)UIActionHandleDirect(an->act), EM_STREAMOUT, SF_RTF, (LPARAM)&es);
	      strcpy(an->_message->body, rtfHtml->rtfParse((char*)text.a_str(), text.size()).c_str());
	      an->_message->flag |= MF_HTML;
        return;
      }
    }
    forwardAction();
  }

  void Controller::_msgSend() {
    switch (this->getAN()->code) {
      case ACTN_ACTION: {
        // TODO: Wysy³amy wiadomoœæ (RichEdit powinien wzywaæ t¹ akcjê przy wysy³aniu).
        this->forwardAction();
      }
    }
  }

  void Controller::clearWnd(IECtrl* ctrl) {
    ctrl->clear();

    // We create structure of the data
    param_data data(param_data::HASH);

    try {
      ctrl->write(tplHandler->parseTpl(&data, "skeleton").a_str());
    } catch(const exception &e) {
      ctrl->write("obsra³em siê, przepraszam<br/>");
      ctrl->write(e.what());
    }
  }

  LRESULT CALLBACK Controller::msgWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) { 
      case WM_NOTIFY: {
        if (wParam == ICMessage(IMI_ACTION_GETINDEX, (int)&sUIAction(IMIG_MSGWND, Konnekt::UI::ACT::msg_ctrlsend), 0)) {
          switch (((NMHDR*)lParam)->code) {
            case EN_SELCHANGE: {
              int cntID = (int)GetProp(hWnd, "CntID");
              HWND hEdit = (HWND)UIActionHandleDirect(sUIAction(IMIG_MSGWND, Konnekt::UI::ACT::msg_ctrlsend, cntID));
              CHARFORMAT cf;
              ZeroMemory(&cf, sizeof(CHARFORMAT));
              cf.cbSize = sizeof(CHARFORMAT);
              cf.dwMask = CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_COLOR;
              SendMessage(hEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
              if((cf.dwEffects & CFE_BOLD) && (cf.dwMask & CFM_BOLD)) {
                UIActionSetStatus(sUIAction(kIEview2::act::formatTb::formatTb, kIEview2::act::formatTb::bold, cntID), -1, ACTS_CHECKED);
              } else {
                UIActionSetStatus(sUIAction(kIEview2::act::formatTb::formatTb, kIEview2::act::formatTb::bold, cntID), 0, ACTS_CHECKED);
              }
              if((cf.dwEffects & CFE_ITALIC) && (cf.dwMask & CFM_ITALIC)) {
                UIActionSetStatus(sUIAction(kIEview2::act::formatTb::formatTb, kIEview2::act::formatTb::italic, cntID), -1, ACTS_CHECKED);
              } else {
                UIActionSetStatus(sUIAction(kIEview2::act::formatTb::formatTb, kIEview2::act::formatTb::italic, cntID), 0, ACTS_CHECKED);
              }
              if((cf.dwEffects & CFE_UNDERLINE) && (cf.dwMask & CFM_UNDERLINE)) {
                UIActionSetStatus(sUIAction(kIEview2::act::formatTb::formatTb, kIEview2::act::formatTb::underline, cntID), -1, ACTS_CHECKED);
              } else {
                UIActionSetStatus(sUIAction(kIEview2::act::formatTb::formatTb, kIEview2::act::formatTb::underline, cntID), 0, ACTS_CHECKED);
              }
              break;
            }
          }
        }
        break;
      }
    }
    return CallWindowProc(Controller::getInstance()->oldMsgWndProc, hWnd, msg, wParam, lParam);
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
    switch (type) {
      case MT_MESSAGE: name = "message"; break;
      case MT_SMS: name = "sms"; break;
      case MT_FILE: name = "file"; break;
      case MT_QUICKEVENT: name = "quickevent"; break;
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
      cnt = ICMessage(IMC_CNT_FIND, msg->net, (int)msg->toUid);
    } else {
      cnt = ICMessage(IMC_CNT_FIND, msg->net, (int)msg->fromUid);
    }
    return cnt;
  }

  String Controller::htmlEscape(StringRef& txt) {
    template_parser_ns::udf_fn* escape = tplHandler->getUdfFactory()->get("htmlEscape");
    escape->param(txt);
    escape->handler();
    txt = escape->result();

    return PassStringRef(txt);
  }

  String Controller::linkify(StringRef& txt) {
    try {
      // zamiana linkow
    } catch(const RegEx::CompileException& e) {
      txt = "RegEx compile error: <b>"; txt += e.error;
      txt += "</b> at pos <b>" + inttostr(e.pos) + "</b>";
    }
    return PassStringRef(txt);
  }

  String Controller::nl2br(StringRef& txt) {
    txt = RegEx::doReplace("/\r?\n/m", "<br />\r\n", txt.c_str());
    return PassStringRef(txt);
  }

  String Controller::getDisplayFromMsg(UI::Notify::_insertMsg* an) {
    cMessage* msg = an->_message;
    tCntId cnt = getCntFromMsg(msg);

    String mex = GetExtParam(msg->ext, MEX_DISPLAY);
    String display;

    if (mex.length()) {
      display = mex;
    } else if (an->_display && strlen(an->_display)) {
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

  void Controller::handleTextFlag(int flag) {
    sUIActionNotify_2params* an = this->getAN();

    HWND hwnd = (HWND) UIActionHandleDirect(sUIAction(IMIG_MSGWND, UI::ACT::msg_ctrlsend, an->act.cnt));
    CHARFORMAT cf;
    ZeroMemory(&cf, sizeof(CHARFORMAT));
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = flag;
    DWORD dwSelMask = SendMessage(hwnd, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

    if ((cf.dwMask & flag) && (dwSelMask & flag)) {
      cf.dwEffects ^= flag;
    } else {
      cf.dwEffects |= flag;
    }

    cf.dwMask = flag;
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
        data.hash_insert_new_var("info", linkify(info));
      } else {
        data.hash_insert_new_var("info", info);
      }
    }
    if (an->_status & ST_IGNORED) {
      data.hash_insert_new_var("ignored?", "1");
    }
    return tplHandler->parseTpl(&data, "status");
  }

  String Controller::_parseMsgTpl(UI::Notify::_insertMsg* an) {
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
      if (config->getInt(cfg::linkify)) {
        body = linkify(body);
      }
    }

    // We create structure of the data
    param_data data(param_data::HASH);
    data.hash_insert_new_var("@time", i64tostr(date.getInt64()));
    data.hash_insert_new_var("@id", inttostr(msg->id));
    data.hash_insert_new_var("@cnt", inttostr(cnt));
    data.hash_insert_new_var("type", type);
    data.hash_insert_new_var("ext", msg->ext);
    data.hash_insert_new_var("time", date.strftime("%H:%M"));
    data.hash_insert_new_var("body", body);

    switch (msg->type) {
      case MT_MESSAGE: _handleStdMsgTpl(data, an); break;
      case MT_SMS:_handleSmsTpl(data, an); break;
      case MT_FILE: _handleFileTpl(data, an); break;
      case MT_QUICKEVENT: _handleQuickEventTpl(data, an); break;
    }
    return tplHandler->parseTpl(&data, type.c_str());
  }

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
    data.hash_insert_new_var("display", getDisplayFromMsg(an));
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
    data.hash_insert_new_var("display", getDisplayFromMsg(an));
  }
}