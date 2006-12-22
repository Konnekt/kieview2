/**
  *  kIEview2 Controller class
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

    config->setColumn(DTCFG, cfg::useEmots, DT_CT_INT, 1, "kIEview2/emots/use");
    config->setColumn(DTCFG, cfg::emotsDir, DT_CT_STR, "emots", "kIEview2/emots/dir");
    config->setColumn(DTCFG, cfg::emotsPack, DT_CT_STR, "", "kIEview2/emots/pack");

    this->subclassAction(UI::ACT::msg_ctrlview, IMIG_MSGWND);
    this->subclassAction(UI::ACT::msg_ctrlview, IMIG_HISTORYWND);
    // TODO: Podmieniamy kontrolkê wpisywania wiadomoœci
    //this->subclassAction(UI::ACT::msg_ctrlsend, IMIG_MSGWND);
    //this->subclassAction(IMIA_MSG_SEND, IMIG_MSGTB);

    IECtrl::init();

    this->actionsHandler = new ActionsHandler;
    this->tplHandler = new TplHandler("./tpl/");

    tplHandler->bindStdFunctions();
    tplHandler->bindUdf("formatTime", new udf_strftime);
    tplHandler->bindUdf("sprintf", new udf_sprintf);
  }

  Controller::~Controller() {
    IECtrl::deinit();

    delete actionsHandler;
    delete tplHandler;
  }

  void Controller::_onPrepare() {
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
      UIActionAdd(act::formatTb::formatTb, act::formatTb::bold, 0, "Pogrubienie", ico::bold);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::italic, 0, "Kursywa", ico::italic);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::underline, 0, "Podkreœlenie", ico::underline);
      UIActionAdd(act::formatTb::formatTb, act::formatTb::color, 0, "Kolor", ico::color);
    }

    UIGroupAdd(IMIG_CFG_PLUGS, ui::cfgGroup, 0, "kIEview2", ico::logo);
    UIActionCfgAddPluginInfoBox2(ui::cfgGroup, 
      "Wtyczka kIEview2 zastêpuje standardowe okno rozmowy Konnekta dziêki czemu mo¿liwe jest "
      "wyœwietlanie emotikon oraz modyfikacja wygl¹du okna przy pomocy styli CSS i skryptów JS.",
      "<span class='note'>Skompilowano: <b>" __DATE__ "</b> [<b>" __TIME__ "</b>]</span><br/>"
      "Informacje o wtyczce na stronie projektu "
      "<b>KPlugins</b> (http://kplugins.net/)<br/><br/>"
      "Copyright © 2006 <b>Sijawusz Pur Rahnama</b><br/>"
      "Copyright © 2006 <b>Micha³ \"Dulek\" Dulko</b><br/>"
      "Copyright © 2005 <b>Kuba \"nix\" Niegowski</b>", Helpers::icon16(ico::logo).a_str());

    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUP, "Ustawienia");
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_NEEDRESTART, "Wyœwietlaj toolbar w oknie rozmowy", cfg::showFormatTb);
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
        if (an->code == ACTN_CREATEGROUP) {
          this->actionsHandler->selectedMenuItem = 0;
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
        if (an->code == ACTN_ACTION) {
          this->actionsHandler->selectedMenuItem = an->act.id;
        }
        break;
      }
      case act::formatTb::bold: {
        if(an->code == ACTN_ACTION) {
        
        }
        break;
      }
      case act::formatTb::italic: {
        if(an->code == ACTN_ACTION) {
        
        }
        break;
      }
      case act::formatTb::underline: {
        if(an->code == ACTN_ACTION) {
        
        }
        break;
      }
      case act::formatTb::color: {
        if(an->code == ACTN_ACTION) {
        
        }
        break;
      }
      case act::formatTb::emots: {
        if(an->code == ACTN_ACTION) {
        
        }
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
        IECtrl* ctrl = new IECtrl(an->hwndParent, an->x, an->y, an->w, an->h, an->act.cnt);
        an->hwnd = ctrl->getHWND();

        ctrl->setPopupMenuListener(this->actionsHandler);
        ctrl->setAnchorClickListener(this->actionsHandler);
        ctrl->setDropListener(this->actionsHandler);
        ctrl->setExternalListener(this->actionsHandler);
        ctrl->setScriptMessageListener(this->actionsHandler);
        ctrl->setKeyDownListener(this->actionsHandler);

        ctrl->enableSandbox(false);
        this->clearWnd(ctrl);
        break;
      }

      case ACTN_DESTROYWINDOW: {
        sUIActionNotify_destroyWindow* an = (sUIActionNotify_destroyWindow*)this->getAN();
        IECtrl* ctrl = IECtrl::get(an->hwnd);
        delete ctrl;
        break;
      }

      case UI::Notify::insertMsg: {
        UI::Notify::_insertMsg* an = (UI::Notify::_insertMsg*)this->getAN();
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));

        IECtrl::Var args;
        IECtrl::Var ret;

        try {
          args[0] = _parseMsgTpl(an->_message).a_str();
        } catch(const exception &e) { 
          args[0] = tplHandler->parseException(e).a_str();
        }

        ctrl->callJScript("addMessage", args, &ret);
        ctrl->scrollToBottom();
        break;
      }

      case UI::Notify::insertStatus: {
        UI::Notify::_insertStatus* an = (UI::Notify::_insertStatus*)this->getAN();
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));

        IECtrl::Var args;
        IECtrl::Var ret;

        try {
          args[0] = _parseStatusTpl(an->_status, an->_info).a_str();
        } catch(const exception &e) { 
          args[0] = tplHandler->parseException(e).a_str();
        }

        ctrl->callJScript("addStatus", args, &ret);
        ctrl->scrollToBottom();
        break;
      }

      case UI::Notify::lock: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*)this->getAN();
        IECtrl* ctrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));
        // TODO: Mo¿emy spodziewaæ siê, ¿e zaraz dodamy du¿o danych, np. przy przegl¹daniu historii
        Ctrl->Sleep(1000);
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
    switch (this->getAN()->code) {
      case ACTN_CREATEWINDOW: {
        sUIActionNotify_createWindow* an = (sUIActionNotify_createWindow*)this->getAN();
        this->forwardAction();
        break;
      }

      case ACTN_DESTROYWINDOW: {
        sUIActionNotify_destroyWindow* an = (sUIActionNotify_destroyWindow*)this->getAN();
        DestroyWindow(an->hwnd);
        break;
      }

      case UI::Notify::supportsFormatting: {
        return setSuccess();
      }
    }
  }

  void Controller::_msgSend() {
    switch (this->getAN()->code) {
      case ACTN_ACTION: {
        // TODO: Wysy³amy wiadomoœæ (RichEdit powinien wzywaæ t¹ akcjê przy wysy³aniu).
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
      ctrl->write("obsralem sie, przepraszam<br/>");
      ctrl->write(e.what());
    }
  }

  String Controller::getStatusLabel(int status) {
    String name = "Nieznany (" + inttostr(status) + ")";
    switch (status) {
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

  string Controller::getMsgLabel(int type) {
    string name = "unknown";
    switch (type) {
      case MT_MESSAGE: name = "message"; break;
      case MT_SMS: name = "sms"; break;
      case MT_FILE: name = "file"; break;
      case MT_QUICKEVENT: name = "quickevent"; break;
    }
    return name;
  }

  String Controller::_parseStatusTpl(int status, const char* info) {
    Date64 date(true);

    // We create structure of the data
    param_data data(param_data::HASH);
    data.hash_insert_new_var("_time", inttostr(date.getInt64()));
    data.hash_insert_new_var("time", date.strftime("%H:%M"));
    data.hash_insert_new_var("status", getStatusLabel(status));
    if (info) {
      data.hash_insert_new_var("info", info);
    }
    return tplHandler->parseTpl(&data, "status");
  }

  String Controller::_parseMsgTpl(cMessage* msg) {
    Date64 date(msg->time);
    string display;
    int cnt = 0;

    if (msg->flag & MF_SEND) {
      cnt = ICMessage(IMC_CNT_FIND, msg->net, (int)msg->toUid);
      display = !strlen(config->getChar(CNT_DISPLAY, 0)) ? "Ja" : config->getChar(CNT_DISPLAY, 0);
    } else {
      cnt = ICMessage(IMC_CNT_FIND, msg->net, (int)msg->fromUid);
      display = !strlen(config->getChar(CNT_DISPLAY, cnt)) ? msg->fromUid : config->getChar(CNT_DISPLAY, cnt);
    }

    // We create structure of the data
    param_data data(param_data::HASH);
    data.hash_insert_new_var("_time", inttostr(date.getInt64()));
    data.hash_insert_new_var("time", date.strftime("%H:%M"));
    data.hash_insert_new_var("display", display);
    data.hash_insert_new_var("id", inttostr(cnt));
    data.hash_insert_new_var("net", inttostr(msg->net));
    data.hash_insert_new_var("uid", config->getChar(CNT_UID, cnt));
    data.hash_insert_new_var("name", config->getChar(CNT_NAME, cnt));
    data.hash_insert_new_var("surname", config->getChar(CNT_SURNAME, cnt));
    data.hash_insert_new_var("type", getMsgLabel(msg->type));
    data.hash_insert_new_var("body", msg->body);

    if (msg->flag & MF_HTML)
      data.hash_insert_new_var("isHtml", "1");
    if (msg->flag & MF_SEND)
      data.hash_insert_new_var("isSent", "1");

    return tplHandler->parseTpl(&data, "message");
  }
}