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
#include "EmotUI.h"

namespace kIEview2 {
  // initialization
  Controller::Controller(): jsController(0), styleLV(0), emotLV(0), ieVersion(getIEVersion()) {
    IECtrl::getGlobal()->bindMethod("oController", bind(resolve_cast0(&Controller::getJSController), this), true);
    IECtrl::getGlobal()->bindMethod("oWindow", bind(&Controller::getJSWndController, this, _1, _2));

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
    this->registerActionObserver(Konnekt::UI::ACT::msg_ctrlview, bind(resolve_cast0(&Controller::_msgCtrlView), this));
    this->registerActionObserver(Konnekt::UI::ACT::msg_ctrlsend, bind(resolve_cast0(&Controller::_msgCtrlSend), this));

    this->registerActionObserver(ui::styleLV, bind(resolve_cast0(&Controller::_styleLV), this));
    this->registerActionObserver(ui::emotLV, bind(resolve_cast0(&Controller::_emotLV), this));

    /* Configuration columns */
    config->setColumn(DTCFG, cfg::lastMsgCount, DT_CT_INT, 10, "kIEview2/lastMsgCount");
    config->setColumn(DTCFG, cfg::relativeTime, DT_CT_INT, 1, "kIEview2/relativeTime");
    config->setColumn(DTCFG, cfg::autoScroll, DT_CT_INT, 0, "kIEview2/autoScroll");

    config->setColumn(DTCFG, cfg::showFormattingBtns, DT_CT_INT, 1, "kIEview2/showFormattingBtns");
    config->setColumn(DTCFG, cfg::showEmotChooser, DT_CT_INT, 1, "kIEview2/showEmotChooser");
    config->setColumn(DTCFG, cfg::showColorChooser, DT_CT_INT, 1, "kIEview2/showColorChooser");
    config->setColumn(DTCFG, cfg::showAutoScroll, DT_CT_INT, 1, "kIEview2/showAutoScroll");

    config->setColumn(DTCFG, cfg::linkify, DT_CT_INT, 1, "kIEview2/linkify/use");
    config->setColumn(DTCFG, cfg::linkifyInHistory, DT_CT_INT, 1, "kIEview2/linkify/inHistory");

    config->setColumn(DTCFG, cfg::useEmots, DT_CT_INT, 1, "kIEview2/emots/use");
    config->setColumn(DTCFG, cfg::useEmotsInHistory, DT_CT_INT, 1, "kIEview2/emots/inHistory");

    config->setColumn(DTCFG, cfg::emotsDir, DT_CT_STR, "emots", "kIEview2/emots/dir");
    config->setColumn(DTCFG, cfg::emotPacks, DT_CT_STR, "", "kIEview2/emots/packs");

    config->setColumn(DTCFG, cfg::stylesDir, DT_CT_STR, "templates", "kIEview2/styles/dir");
    config->setColumn(DTCFG, cfg::currentStyle, DT_CT_STR, "", "kIEview2/styles/current");

    this->subclassAction(Konnekt::UI::ACT::msg_ctrlview, IMIG_MSGWND);
    this->subclassAction(Konnekt::UI::ACT::msg_ctrlview, IMIG_HISTORYWND);
    this->subclassAction(Konnekt::UI::ACT::msg_ctrlsend, IMIG_MSGWND);

    IECtrl::init();
    setlocale(LC_ALL, "polish");

    tplHandler.bindStdFunctions();
    tplHandler.bindUdf("getCntSetting", new udf_get_cnt_setting);
    tplHandler.bindUdf("getSetting", new udf_get_cfg_setting);

    tplHandler.bindUdf("formatString", new udf_stringf);
    tplHandler.bindUdf("formatTime", new udf_strftime);

    tplHandler.bindUdf("getExtParam", new udf_get_ext_param);
    tplHandler.bindUdf("replace", new udf_replace);
    tplHandler.bindUdf("match?", new udf_match);

    bindMsgHandler(MT_QUICKEVENT, bind(&Controller::_handleQuickEventTpl, this, _1, _2), "quickevent");
    bindMsgHandler(MT_MESSAGE, bind(&Controller::_handleStdMsgTpl, this, _1, _2), "message");
    bindMsgHandler(MT_FILE, bind(&Controller::_handleFileTpl, this, _1, _2), "file");
    bindMsgHandler(MT_SMS, bind(&Controller::_handleSmsTpl, this, _1, _2), "sms");
  }

  Controller::~Controller() {
    wndObjCollection.clear();

    if (jsController) {
      delete jsController;
    }
    for (tMsgHandlers::iterator it = msgHandlers.begin(); it != msgHandlers.end(); it++) {
      delete it->second;
    }
    IECtrl::deinit();
  }

  void Controller::_onPluginsLoaded() {
    if (ieVersion < 6) {
      Ctrl->IMessage(&sIMessage_plugOut(Ctrl->ID(), "kIEview2 potrzebuje do dzia³ania conajmniej IE6.",
        sIMessage_plugOut::erNo, sIMessage_plugOut::euNowAndOnNextStart));
      return setFailure();
    }
    if (int oldId = pluginExists(plugsNET::kieview)) {
      Ctrl->IMessage(&sIMessage_plugOut(oldId, "Wykryto starsz¹ wersjê wtyczki, od³¹czanie...",
        sIMessage_plugOut::erNo, sIMessage_plugOut::euNowAndOnNextStart));
      return setFailure();
    }
    setSuccess();
  }

  void Controller::_onPrepare() {
    historyTable = Tables::registerTable(Ctrl, tableNotFound, optPrivate);
    kPath = (char*) Ctrl->ICMessage(IMC_KONNEKTDIR);

    IECtrl::setAutoCopySel(config->getInt(CFG_UIMSGVIEW_COPY));

    // emotHandler << new JispParser;
    emotHandler << new GGParser;

    tplHandler.load();
    emotHandler.load();

    IconRegister(IML_16, ico::logo, Ctrl->hDll(), IDI_LOGO);
    IconRegister(IML_16, ico::link, Ctrl->hDll(), IDI_LINK);
    IconRegister(IML_16, ico::copy, Ctrl->hDll(), IDI_COPY);
    IconRegister(IML_16, ico::save, Ctrl->hDll(), IDI_SAVE);
    IconRegister(IML_16, ico::styles, Ctrl->hDll(), IDI_STYLES);
    IconRegister(IML_16, ico::emots, Ctrl->hDll(), IDI_EMOTS);
    IconRegister(IML_16, ico::bold, Ctrl->hDll(), IDI_BOLD);
    IconRegister(IML_16, ico::italic, Ctrl->hDll(), IDI_ITALIC);
    IconRegister(IML_16, ico::underline, Ctrl->hDll(), IDI_UNDERLINE);
    IconRegister(IML_16, ico::color, Ctrl->hDll(), IDI_COLOR);
    IconRegister(IML_16, ico::print, Ctrl->hDll(), IDI_PRINT);
    IconRegister(IML_16, ico::source, Ctrl->hDll(), IDI_SOURCE);
    IconRegister(IML_16, ico::autoScroll, Ctrl->hDll(), IDI_AUTOSCROLL);

    IconRegister(IML_16, ico::checked, Ctrl->hDll(), IDI_CHECKED);
    IconRegister(IML_16, ico::unchecked, Ctrl->hDll(), IDI_UNCHECKED);
    IconRegister(IML_16, ico::emotsinfo, Ctrl->hDll(), IDI_EMOTSINFO);

    // menu akcji pod prawym klawiszem myszy
    UIGroupAdd(IMIG_MSGWND, act::popup::popup, ACTR_INIT);
    UIActionAdd(act::popup::popup, act::popup::openUrl, ACTSMENU_BOLD, "Otwórz", ico::link);
    UIActionAdd(act::popup::popup, act::popup::copyUrl, 0, "Kopiuj adres");
    UIActionAdd(act::popup::popup, act::popup::urlSep, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::saveImage, ACTSMENU_BOLD, "Zapisz obrazek", ico::save);
    UIActionAdd(act::popup::popup, act::popup::imageSep, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::copySelection, 0, "Kopiuj", ico::copy);
    UIActionAdd(act::popup::popup, act::popup::selectAll, 0, "Zaznacz wszystko");
    UIActionAdd(act::popup::popup, act::popup::print, 0, "Drukuj", ico::print);
    UIActionAdd(act::popup::popup, act::popup::showSource, 0, "Poka¿ Ÿród³o", ico::source);
    UIActionAdd(act::popup::popup, act::popup::historySep, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::lastSession, 0, "Wczytaj ostatni¹ sesjê");
    UIActionAdd(act::popup::popup, act::popup::lastMsgs, 0, "Wczytaj ostatnie wiad.");
    UIActionAdd(act::popup::popup, act::popup::clearSep, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::clear, 0, "Wyczyœæ okno", 0x74);

    UIGroupAdd(IMIG_MSGBAR, act::formatTb::formatTb);
    UIActionAdd(act::formatTb::formatTb, act::formatTb::emots, 0, "Emotikony", ico::emots);
    UIActionAdd(act::formatTb::formatTb, act::formatTb::bold, ACTT_CHECK, "Pogrubienie", ico::bold);
    UIActionAdd(act::formatTb::formatTb, act::formatTb::italic, ACTT_CHECK, "Kursywa", ico::italic);
    UIActionAdd(act::formatTb::formatTb, act::formatTb::underline, ACTT_CHECK, "Podkreœlenie", ico::underline);
    UIActionAdd(act::formatTb::formatTb, act::formatTb::color, ACTT_CHECK, "Kolor", ico::color);
    UIActionAdd(act::formatTb::formatTb, act::formatTb::autoScroll, ACTR_INIT, "Przewijaj", ico::autoScroll);
    setActionsStatus();

    char desc[] = 
      "<span class='note'>Skompilowano: <b>" __DATE__ "</b> [<b>" __TIME__ "</b>]</span><br/>"
      "Du¿e podziêkowania za pomoc nale¿¹ siê dla <b>dulka</b> i <b>ursusa</b> :)<br/><br/>"
      "Copyright © 2006-2007 <b>Sijawusz Pur Rahnama</b><br/>"
      "Copyright © 2005 <b>Kuba \"nix\" Niegowski</b>";

    UIGroupAdd(IMIG_CFG_UI, ui::cfgGroup, 0, "Okno rozmowy", ico::logo);
    UIGroupAdd(ui::cfgGroup, ui::styleCfgGroup, 0, "Szablony", ico::styles);
    UIGroupAdd(ui::cfgGroup, ui::emotCfgGroup, 0, "Emotikony", ico::emots);

    UIActionCfgAddPluginInfoBox2(ui::cfgGroup, 
      "Wtyczka kIEview2 zastêpuje standardowe okno rozmowy Konnekta dziêki czemu mo¿liwe jest wyœwietlanie "
      "emotikon oraz modyfikacja wygl¹du okna przy pomocy szablonów, styli <b>CSS</b> i <b>J</b>ava<b>S</b>cript-u.",
      desc, icon16(ico::logo).a_str(), -3);
    UIActionCfgAddPluginInfoBox2(ui::styleCfgGroup, 
      "{{<b>Szablony</b>}}",
      desc, icon16(ico::styles).a_str(), -3);
    UIActionCfgAddPluginInfoBox2(ui::emotCfgGroup, 
      "Poni¿ej znajduj¹ siê opcje dotycz¹ce emotikon. Wybór zestawów emotikon oraz ustawienia menu emotikon.",
      desc, icon16(ico::emots).a_str(), -3);

    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUP, "Ustawienia");
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK, "Automatycznie przewijaj okno rozmowy", cfg::autoScroll);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK, "Stosuj czas relatywny (jeœli siê da)", cfg::relativeTime);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_SPINNER | ACTSC_INLINE, AP_MINIMUM "0" AP_MAXIMUM "1000", cfg::lastMsgCount, 0, 0, 65);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_COMMENT, "Iloœæ ostatnich wiadomoœci do wczytania");
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUPEND);

    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUP, "Linki");
    UIActionCfgAdd(ui::cfgGroup, cfg::linkify, ACTT_CHECK | ACTR_INIT, "Zamieniaj linki na 'klikalne'", cfg::linkify);
    UIActionCfgAdd(ui::cfgGroup, cfg::linkifyInHistory, ACTT_CHECK, "+ równie¿ w historii" AP_TIPRICH 
      "<b>Uwaga!</b> wyd³u¿a czas ³adowania rozmowy", cfg::linkifyInHistory);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUPEND);

    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUP, "Toolbar formatowania");
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_INLINE, "Przycisk wyboru emotikon", cfg::showEmotChooser);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_IMAGE, icon16(ico::emots).a_str());

    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_INLINE, "Przyciski formatowania", cfg::showFormattingBtns);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_IMAGE | ACTSC_INLINE, icon16(ico::bold).a_str());
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_IMAGE | ACTSC_INLINE, icon16(ico::italic).a_str());
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_IMAGE, icon16(ico::underline).a_str());

    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_INLINE, "Przycisk wyboru koloru", cfg::showColorChooser);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_IMAGE, icon16(ico::color).a_str());

    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_INLINE, "Przycisk przewijania okna", cfg::showAutoScroll);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_IMAGE, icon16(ico::autoScroll).a_str());
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUPEND);

    UIActionCfgAdd(ui::emotCfgGroup, 0, ACTT_GROUP, "Emotikony");
    UIActionCfgAdd(ui::emotCfgGroup, cfg::useEmots, ACTT_CHECK | ACTR_INIT, "Wyœwietlaj emotikony", cfg::useEmots);
    UIActionCfgAdd(ui::emotCfgGroup, cfg::useEmotsInHistory, ACTT_CHECK, "+ równie¿ w historii" AP_TIPRICH 
      "<b>Uwaga!</b> wyd³u¿a czas ³adowania rozmowy", cfg::useEmotsInHistory);
    UIActionCfgAdd(ui::emotCfgGroup, 0, ACTT_SEPARATOR, "Katalog w którym znajduj¹ siê \"pakiety\" emotikon");
    UIActionCfgAdd(ui::emotCfgGroup, cfg::emotsDir, ACTT_DIR, "", cfg::emotsDir);
    UIActionCfgAdd(ui::emotCfgGroup, 0, ACTT_SEPARATOR, "Wybierz pakiety emotikon:");
    UIActionCfgAdd(ui::emotCfgGroup, ui::emotLV, ACTT_HWND | ACTSC_INLINE);
    UIActionCfgAdd(ui::emotCfgGroup, ui::refreshEmotLV, ACTT_BUTTON, "Odœwie¿" AP_ICO "702", 0, 0, 0, 75);
    UIActionCfgAdd(ui::emotCfgGroup, 0, ACTT_GROUPEND);

    UIActionCfgAdd(ui::styleCfgGroup, 0, ACTT_GROUP, "Style");
    UIActionCfgAdd(ui::styleCfgGroup, 0, ACTT_SEPARATOR, "Katalog w którym znajduj¹ siê \"pakiety\" styli");
    UIActionCfgAdd(ui::styleCfgGroup, cfg::stylesDir, ACTT_DIR, "", cfg::stylesDir);
    UIActionCfgAdd(ui::styleCfgGroup, 0, ACTT_SEPARATOR, "Wybierz aktywny styl:");
    UIActionCfgAdd(ui::styleCfgGroup, ui::styleLV, ACTT_HWND | ACTSC_INLINE);
    UIActionCfgAdd(ui::styleCfgGroup, ui::refreshStyleLV, ACTT_BUTTON, "Odœwie¿" AP_ICO "702", 0, 0, 0, 75);
    UIActionCfgAdd(ui::styleCfgGroup, 0, ACTT_GROUPEND);
  }

  void Controller::_onAction() {
    // locking
    LockerCS lock(_locker);

    sUIActionNotify* an = this->getAN();

    switch(an->act.id) {
      case act::popup::popup: {
        IECtrl* pCtrl = IECtrl::get((HWND)UIActionHandleDirect(
          sUIAction(an->act.cnt != -1 ? IMIG_MSGWND : IMIG_HISTORYWND, Konnekt::UI::ACT::msg_ctrlview, an->act.cnt)
        ));
        if (pCtrl) {
          wndObjCollection[pCtrl].actionHandler->selectedMenuItem = 0;
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
        IECtrl* pCtrl = IECtrl::get((HWND)UIActionHandleDirect(
          sUIAction(an->act.cnt != -1 ? IMIG_MSGWND : IMIG_HISTORYWND, Konnekt::UI::ACT::msg_ctrlview, an->act.cnt)
        ));
        if (pCtrl) {
          wndObjCollection[pCtrl].actionHandler->selectedMenuItem = an->act.id;
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
        if (an->code == ACTN_CREATE || an->code == ACTN_ACTION) {
          bool useEmots = *UIActionCfgGetValue(an->act, 0, 0) == '1';

          UIActionSetStatus(sUIAction(ui::emotCfgGroup, cfg::useEmotsInHistory), !useEmots ? -1 : 0, ACTS_DISABLED);
          UIActionSetStatus(sUIAction(ui::emotCfgGroup, cfg::emotsDir), !useEmots ? -1 : 0, ACTS_DISABLED);
          UIActionSetStatus(sUIAction(ui::emotCfgGroup, ui::refreshEmotLV), !useEmots ? -1 : 0, ACTS_DISABLED);
          if (EmotLV::isValidLV(emotLV)) emotLV->setEnabled(useEmots);
        }
        break;
      }
      case cfg::linkify: {
        if (an->code == ACTN_CREATE || an->code == ACTN_ACTION) {
          UIActionSetStatus(sUIAction(ui::cfgGroup, cfg::linkifyInHistory), *UIActionCfgGetValue(an->act, 0, 0) == '0' ? -1 : 0, ACTS_DISABLED);
        }
        break;
      }
      case ui::refreshEmotLV: {
        if (an->code == ACTN_ACTION) {
          emotHandler.reload(emotLV);
        }
        break;
      }
      case ui::refreshStyleLV: {
        if (an->code == ACTN_ACTION) {
          tplHandler.reload(styleLV);
        }
        break;
      }
    }
  }

  void Controller::_onCfgChanged() {
    IECtrl::setAutoCopySel(config->getInt(CFG_UIMSGVIEW_COPY));
    setActionsStatus();

    if (StyleLV::isValidLV(styleLV)) {
      styleLV->saveState();
    }
    tplHandler.saveSettings();
    tplHandler.reload(styleLV);

    if (EmotLV::isValidLV(emotLV)) {
      emotLV->saveState();
    }
    emotHandler.saveSettings();
    emotHandler.reload(emotLV);
  }

  void Controller::_msgCtrlView() {
    // locking
    LockerCS lock(_locker);

    switch (getAN()->code) {
      case ACTN_CREATEWINDOW: {
        sUIActionNotify_createWindow* an = (sUIActionNotify_createWindow*)this->getAN();
        IECtrl* pCtrl = new IECtrl(an->hwndParent, an->x, an->y, an->w, an->h);
        an->hwnd = pCtrl->getHWND();

        oldMsgWndProc = (WNDPROC) SetWindowLong(an->hwndParent, GWL_WNDPROC, (LONG)Controller::msgWndProc);
        SetProp(an->hwndParent, "CntID", (HANDLE)an->act.cnt);

        wndObjCollection[pCtrl].actionHandler = new ActionHandler(pCtrl, an->act.cnt);

        pCtrl->enableSandbox(false);
        this->initWnd(pCtrl);
        break;
      }

      case ACTN_DESTROYWINDOW: {
        sUIActionNotify_destroyWindow* an = (sUIActionNotify_destroyWindow*)this->getAN();
        IECtrl* pCtrl = IECtrl::get(an->hwnd);

        for (tWndObjCollection::iterator it = wndObjCollection.begin(); it != wndObjCollection.end(); it++) {
          if (it->first == pCtrl) {
            wndObjCollection.erase(it); break;
          }
        }
        delete pCtrl;
        break;
      }

      case Konnekt::UI::Notify::insertMsg: {
        Konnekt::UI::Notify::_insertMsg* an = (Konnekt::UI::Notify::_insertMsg*)this->getAN();
        IECtrl* pCtrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));

        IECtrl::Var args;
        IECtrl::Var ret;

        bool autoScroll = an->_scroll && this->autoScroll(an, pCtrl);
        try {
          args[0] = _parseMsgTpl(an).a_str();
        } catch(const exception& e) { 
          args[0] = tplHandler.parseException(e).a_str();
        } catch(const Exception& e) {
          break;
        }

        waitForIECtrlReady(pCtrl);
        pCtrl->callJScript("addMessage", args, &ret);
        if (autoScroll) pCtrl->scrollToBottom();
        break;
      }

      case Konnekt::UI::Notify::insertStatus: {
        Konnekt::UI::Notify::_insertStatus* an = (Konnekt::UI::Notify::_insertStatus*)this->getAN();
        IECtrl* pCtrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));

        IECtrl::Var args;
        IECtrl::Var ret;

        bool autoScroll = this->autoScroll(an, pCtrl);
        try {
          args[0] = _parseStatusTpl(an).a_str();
        } catch(const exception& e) { 
          args[0] = tplHandler.parseException(e).a_str();
        } catch(const Exception& e) {
          break;
        }

        waitForIECtrlReady(pCtrl);
        pCtrl->callJScript("addStatus", args, &ret);
        if (autoScroll) pCtrl->scrollToBottom();
        break;
      }

      case Konnekt::UI::Notify::lock: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*)this->getAN();
        IECtrl* pCtrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));
        // TODO: Mo¿emy spodziewaæ siê, ¿e zaraz dodamy du¿o danych, np. przy przegl¹daniu historii
        break;
      }

      case Konnekt::UI::Notify::unlock: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*)this->getAN();
        IECtrl* pCtrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));
        // TODO: Du¿a iloœæ danych zosta³a dodana
        break;
      }

      case Konnekt::UI::Notify::clear: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*)this->getAN();
        IECtrl* pCtrl = IECtrl::get((HWND)UIActionHandleDirect(an->act));
        this->initWnd(pCtrl);
        break;
      }

      case ACTN_SETCNT: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*)this->getAN();
        an->notify2 = (int)GetDlgItem((HWND)UIActionHandleDirect(sUIAction(0, an->act.parent, an->act.cnt)), Konnekt::UI::ACT::msg_ctrlview);
        break;
      }
    }
  }

  void Controller::_styleLV() {
    if (getAN()->code == ACTN_CREATEWINDOW) {
      styleLV = new StyleLV((sUIActionNotify_createWindow*) getAN(), 220, 120);
      tplHandler.fillLV(styleLV);
    }
  }

  void Controller::_emotLV() {
    if (getAN()->code == ACTN_CREATEWINDOW) {
      emotLV = new EmotLV((sUIActionNotify_createWindow*) getAN(), 220, 120);
      emotLV->setEnabled(config->getInt(cfg::useEmots));
      emotHandler.fillLV(emotLV);
    }
  }

  void Controller::_msgCtrlSend() {
    // locking
    LockerCS lock(_locker);

    switch (getAN()->code) {
      case Konnekt::UI::Notify::supportsFormatting: {
        return setSuccess();
      }

      case Konnekt::UI::Notify::getMessageSize: {
        sUIActionNotify_2params* an = (sUIActionNotify_2params*) getAN();

        String text;
        EDITSTREAM es;

        es.dwError = 0;
        es.pfnCallback = Controller::streamOut;
        es.dwCookie = (DWORD)&text;
        SendMessage((HWND)UIActionHandleDirect(an->act), EM_STREAMOUT, SF_RTF, (LPARAM)&es);

        char sla[] = {'\\', '\'', 0};
        char xsla[] = {253, 254, 0};

        text.replaceChars(sla, xsla);
        text = htmlEscape(text);
        text.replaceChars(xsla, sla);
        text = text.substr(29, text.length() - 29 - 13); // @debug chwilowe obejscie buga z przymusowym kolorem wysylanego tekstu

        return setReturnCode(text.size());
      }

      case Konnekt::UI::Notify::getMessage: {
        Konnekt::UI::Notify::_getMessage* an = (Konnekt::UI::Notify::_getMessage*) getAN();

        String text;
        EDITSTREAM es;

        es.dwError = 0;
        es.pfnCallback = Controller::streamOut;
        es.dwCookie = (DWORD)&text;
        SendMessage((HWND)UIActionHandleDirect(an->act), EM_STREAMOUT, SF_RTF, (LPARAM)&es);

        char sla[] = {'\\', '\'', 0};
        char xsla[] = {253, 254, 0};

        text.replaceChars(sla, xsla);
        text = htmlEscape(text);
        text.replaceChars(xsla, sla);
        text = rtfHtml.rtfParse((char*)text.a_str(), text.size());
        text = text.substr(29, text.length() - 29 - 13); // @debug chwilowe obejscie buga z przymusowym kolorem wysylanego tekstu

        strcpy(an->_message->body, text.a_str());
        an->_message->flag |= MF_HTML;
        return;
      }
    }
    forwardAction();
  }

  LRESULT CALLBACK Controller::msgWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) { 
      case WM_NOTIFY: {
        if (wParam == ICMessage(IMI_ACTION_GETINDEX, (int)&sUIAction(IMIG_MSGWND, Konnekt::UI::ACT::msg_ctrlsend), 0)) {
          switch (((NMHDR*)lParam)->code) {
            case EN_SELCHANGE: {
              int cntID = (int) GetProp(hWnd, "CntID");
              HWND hEdit = (HWND)UIActionHandleDirect(sUIAction(IMIG_MSGWND, Konnekt::UI::ACT::msg_ctrlsend, cntID));

              CHARFORMAT cf;
              ZeroMemory(&cf, sizeof(CHARFORMAT));
              cf.cbSize = sizeof(CHARFORMAT);
              cf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_COLOR;
              SendMessage(hEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

              UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::bold, cntID), 
                ((cf.dwEffects & CFE_BOLD) && (cf.dwMask & CFM_BOLD)) ? -1 : 0, ACTS_CHECKED
              );
              UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::italic, cntID), 
                ((cf.dwEffects & CFE_ITALIC) && (cf.dwMask & CFM_ITALIC)) ? -1 : 0, ACTS_CHECKED
              );
              UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::underline, cntID), 
                ((cf.dwEffects & CFE_UNDERLINE) && (cf.dwMask & CFM_UNDERLINE)) ? -1 : 0, ACTS_CHECKED
              );
              UIActionSetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::color, cntID), 
                ((cf.dwEffects & CFE_AUTOCOLOR) && (cf.dwMask & CFM_COLOR)) ? -1 : 0, ACTS_CHECKED
              );
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
    if (args.length() > 0) {
      IECtrl* pCtrl = IECtrl::get((HWND)args[0].getInteger());
      if (pCtrl) {
        if (!wndObjCollection[pCtrl].jsWndController) {
          wndObjCollection[pCtrl].jsWndController = new JS::WndController(pCtrl, args);
        }
        return wndObjCollection[pCtrl].jsWndController;
      }
    }
    throw IECtrl::JSException("Invalid IE Control ref ID");
  }

  IECtrl::Var Controller::getJSController() {
    if (!jsController) {
      jsController = new JS::Controller;
    }
    return jsController;
  }

  bool Controller::hasMsgHandler(int type) {
    // locking
    LockerCS lock(_locker);

    return msgHandlers.find(type) != msgHandlers.end();
  }

  bool Controller::bindMsgHandler(int type, fMessageHandler f, const string& label, int priority, 
    signals::connect_position pos, bool overwrite) 
  {
    if (f.empty()) return false;
    // locking
    LockerCS lock(_locker);

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

  bool Controller::autoScroll(sUIActionNotify_base* an, IECtrl* pCtrl) {
    if (!an->act.cnt) return false;

    if ((UIActionGetStatus(sUIAction(act::formatTb::formatTb, act::formatTb::autoScroll, an->act.cnt)) & ACTS_CHECKED) || pCtrl->isScrollOnBottom()) {
      return true;
    }
    return false;
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

    /*
    Value v(Tables::ctypeString);
    v.buffSize = -1;
    v.vChar = 0;

    if (!table->get(row, table->getColIdByPos(pos), v)) {
      IMLOG("[Controller::getStringCol()] Cannot access msg row");
      return "";
    }
    */

    Value v(Tables::ctypeString);
    v.vCChar = (char*) table->getStr(row, table->getColIdByPos(pos));
    v.buffSize = strlen(v.vCChar);

    if (table->getColType(table->getColIdByPos(pos)) & cflagXor) {
      xor1_encrypt((unsigned char*)encryptKey, (unsigned char*)v.vCChar, v.buffSize);
      xor1_decrypt((unsigned char*)decryptKey, (unsigned char*)v.vCChar, v.buffSize);

      IMLOG("colPos = %i, value = %s", pos, v.vCChar);
    }
    return (char*) v.vCChar;
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

    list<Konnekt::UI::Notify::_insertMsg> msgs;
    int m = 0;

    for (int i = table->getRowCount() - 1, s = 0; (i >= 0) && (m < howMany); i--) {
      if (sessionOffset) {
        if (!table->getInt(i, table->getColIdByPos(fieldSession))) {
          if (++s == sessionOffset) {
            continue;
          }
        }
        if (s < sessionOffset) {
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

      msgs.push_back(Konnekt::UI::Notify::_insertMsg(msg, getStringCol(table, i, fieldDisplay), false));
      m++;
    }

    if (m) {
      Message::quickEvent(cnt, "Wczytujê wiadomoœci z historii.");
    }

    for (list<Konnekt::UI::Notify::_insertMsg>::reverse_iterator it = msgs.rbegin(); it != msgs.rend(); it++) {
      Message::inject(it->_message, cnt, it->_display);

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
    // locking
    LockerCS lock(_locker);

    Tables::oTable table = historyTable;
    loadMsgTable(cnt);

    int howMany = 0;

    for (int i = table->getRowCount() - 1, s = 0; i >= 0; i--) {
      if (sessionOffset) {
        if (!table->getInt(i, table->getColIdByPos(fieldSession))) {
          s++;
        }
        if (s == sessionOffset) {
          howMany++;
        }
        continue;
      }
      howMany++;
      if (!table->getInt(i, table->getColIdByPos(fieldSession))) {
        break;
      }
    }

    int msgCount = readMsgs(cnt, howMany, sessionOffset);
    table->unloadData();

    return msgCount;
  }

  void Controller::initWnd(IECtrl* pCtrl) {
    // locking
    LockerCS lock(_locker);

    // czyscimy wiadomosci grupowania
    clearGroupedMsgs(pCtrl);

    pCtrl->navigate(("file:///" + unifyPath(tplHandler.getCurrentStyleDir(), false, '/') + "/__bootstrap.html").c_str());
    // pCtrl->clear();

    SetProp(GetParent(pCtrl->getHWND()), "MsgSession", (HANDLE) 0);
  }

  DWORD CALLBACK Controller::streamOut(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb) {
    String* entry = (String*) dwCookie;
    *entry += (char*) pbBuff;
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
    string result;

    if (!time) {
      return "0s";
    }
    if (time >= (60 * 60 * 24 * 7)) {
      int weeks = int(time / (60 * 60 * 24 * 7));
      result += stringf("%dw, ", weeks);
      time -= weeks * 60 * 60 * 24 * 7;
    }
    if (time >= (60 * 60 * 24)) {
      int days = int(time / (60 * 60 * 24));
      result += stringf("%dd, ", days);
      time -= days * 60 * 60 * 24;
    }
    if (time >= (60 * 60)) {
      int hours = int(time / (60 * 60));
      result += stringf("%dh, ", hours);
      time -= hours * 60 * 60;
    }
    if (time >= 60) {
      int mins = int(time / 60);
      result += stringf("%dm, ", mins);
      time -= mins * 60;
    }
    if (!result.length() && time > 0) {
      int secs = int(time);
      result += stringf("%ds, ", secs);
    }
    return rtrim(result, ", ");
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
    return tplHandler.runFunc("htmlEscape", txt);
  }

  string __stdcall Controller::eMailInsertion(RegEx* reg) {
    Controller* pCtrl = Controller::getInstance();
    RegEx& r = *reg;

    if (r.hasSub(1)) {
      return r[0];
    }

    sEmailInsertion ei(pCtrl->eMailInsertions.size(), r[2]);
    pCtrl->eMailInsertions.push_back(ei);

    return "<kiev2:email:insertion id=\"" + inttostr(ei.id) + "\" />";
  }

  string __stdcall Controller::replaceEmail(RegEx* reg) {
    sEmailInsertion* ei = &Controller::getInstance()->eMailInsertions.at(atoi(reg->getSub(1).c_str()));
    IMLOG("[Controller::replaceEmail()] email = %s", ei->email.c_str());

    return "<a href=\"mailto:" + ei->email + "\" class=\"autolink\">" + ei->email + "</a>";
  }

  string __stdcall Controller::linkInsertion(RegEx* reg) {
    Controller* pCtrl = Controller::getInstance();
    RegEx& r = *reg;

    if (r.hasSub(1) && r.hasSub(4)) {
      return r[0];
    }

    sLinkInsertion li(pCtrl->linkInsertions.size());
    li.url = ((r[3].find(":") == r[3].npos) ? "http://" : "") + r[2];
    li.name = r[2];

    pCtrl->linkInsertions.push_back(li);
    return r[1] + "<kiev2:link:insertion id=\"" + inttostr(li.id) + "\" />" + r[4];
  }

  string __stdcall Controller::replaceLink(RegEx* reg) {
    sLinkInsertion* li = &Controller::getInstance()->linkInsertions.at(atoi(reg->getSub(1).c_str()));
    IMLOG("[Controller::replaceLink()] url = %s, name = %s", li->url.c_str(), li->name.c_str());

    return "<a href=\"" + li->url + "\" class=\"autolink\" target=\"_blank\">" + li->name + "</a>";
  }

  String Controller::preLinkify(StringRef& txt) {
    if (txt.length() > 20480) return PassStringRef(txt);

    txt = RegEx::doReplace("~([\"|']|&quot;|&apos;|&#0?39;)?((?>([a-z+]{2,}://|www\\.|ftp\\.))(?:[a-z0-9]+(?:\\:[a-z0-9]+)?@)?(?:(?:[a-z0-9](?:[a-z0-9]|(?<!-)-)*[a-z0-9])(?:\\.[a-z0-9](?:[a-z0-9]|(?<!-)-)*[a-z0-9])+|(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))(?:\\:\\d+)?(?:/[^\\\/?*\"<>|\\s]*[a-z0-9])*/?(?:\\?[a-z0-9_.%-]+(?:=[a-z0-9_.%:/+-]*)?(?:&[a-z0-9_.%-;]+(?:=[a-z0-9_.%:/+-]*)?)*)?(?:#[a-z0-9_%.]+)?)(\\1)?~i", &Controller::linkInsertion, txt.c_str());
    txt = RegEx::doReplace("~([\"|']mailto:)?((?:[a-z0-9_'+*$%\\^&!\\.-]+)@(?:(?:[a-z0-9-])+\\.)+(?:[a-z]{2,6}))~i", &Controller::eMailInsertion, txt.c_str());

    return PassStringRef(txt);
  }

  String Controller::postLinkify(StringRef& txt) {
    if (txt.length() > 20480) return PassStringRef(txt);

    txt = RegEx::doReplace("#<kiev2:email:insertion id=\"([0-9]+)\" />#", &Controller::replaceEmail, txt.c_str());
    txt = RegEx::doReplace("#<kiev2:link:insertion id=\"([0-9]+)\" />#", &Controller::replaceLink, txt.c_str());

    eMailInsertions.clear();
    linkInsertions.clear();

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

  String Controller::getDisplayFromMsg(Konnekt::UI::Notify::_insertMsg* an) {
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
    HWND hwnd = (HWND) UIActionHandleDirect(sUIAction(IMIG_MSGWND, Konnekt::UI::ACT::msg_ctrlsend, getAN()->act.cnt));
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

  String Controller::_parseStatusTpl(Konnekt::UI::Notify::_insertStatus* an) {
    Date64 date(true);

    // We create structure of the data
    param_data data(param_data::HASH);
    data.hash_insert_new_var("@time", i64tostr(date.getInt64()));
    data.hash_insert_new_var("@status", inttostr(an->_status));

    data.hash_insert_new_var("display", strlen(config->getChar(CNT_DISPLAY, an->act.cnt)) ? 
      config->getChar(CNT_DISPLAY, an->act.cnt) : 
      config->getChar(CNT_UID, an->act.cnt)
    );
    data.hash_insert_new_var("time", date.strftime("%H:%M"));
    data.hash_insert_new_var("status", getStatusLabel(an->_status));

    if (an->_info) {
      String info = an->_info;
      info = parseBody(info, true, false, config->getInt(cfg::linkify), config->getInt(cfg::useEmots));

      data.hash_insert_new_var("info", info);
    }

    tGroupedSt& groupedSt = getWndObjects(an)->groupedSt;

    bool groupStatus = false;
    bool groupTime = false;
    bool groupInfo = false;

    if (groupedSt.size()) {
      sGroupedSt& lastSt = groupedSt[0];

      int timeFromLastSt = date.getTime64() - lastSt.time.getTime64();
      bool relativeTime = config->getInt(cfg::relativeTime);

      groupTime = timeFromLastSt <= (relativeTime ? (60 * 60 * 24) : (60 * 60));
      groupStatus = lastSt.status == an->_status;
      groupInfo = lastSt.info == an->_info;

      // @debug dodac opcje w konfiguracjis
      if (groupStatus && groupInfo) {
        throw ExceptionString("Duplicated status change notification");
      }
      if (groupStatus) data.hash_insert_new_var("groupStatus", "1");
      if (groupTime) data.hash_insert_new_var("groupTime", "1");
      if (groupInfo) data.hash_insert_new_var("groupInfo", "1");

      if (groupStatus && groupTime) {
        data.hash_erase_var("time");
        data.hash_insert_new_var("time", timeToString(timeFromLastSt) + " póŸniej");
      }
      data.hash_insert_new_var("@lastStTime", i64tostr(lastSt.time.getInt64()));
      data.hash_insert_new_var("timeFromLastSt", timeToString(timeFromLastSt));
      data.hash_insert_new_var("grouped", "1");
    }

    clearGroupedMsgs(an);
    groupedSt.push_back(sGroupedSt(an->_status, date, an->_info));

    if (an->_status & ST_IGNORED) {
      data.hash_insert_new_var("ignored?", "1");
    }
    return tplHandler.parseTpl(&data, "status");
  }

  String Controller::_parseMsgTpl(Konnekt::UI::Notify::_insertMsg* an) {
    // locking
    LockerCS lock(_locker);

    cMessage* msg = an->_message;
    string type = getMsgTypeLabel(msg->type);
    String body = msg->body;
    tCntId cnt = getCntFromMsg(msg);
    Date64 date;
    date = msg->time;

    bool leaveAsIs = msg->flag & MF_LEAVEASIS;
    bool inHistory = isMsgFromHistory(an);
    bool isHtml = msg->flag & MF_HTML;

    body = parseBody(body, !isHtml, !isHtml, 
      !leaveAsIs && config->getInt(cfg::linkify) && (!inHistory || config->getInt(cfg::linkifyInHistory)), 
      !leaveAsIs && config->getInt(cfg::useEmots) && (!inHistory || config->getInt(cfg::useEmotsInHistory)));

    // We create structure of the data
    param_data data(param_data::HASH);
    data.hash_insert_new_var("@time", i64tostr(date.getInt64()));
    data.hash_insert_new_var("@id", inttostr(msg->id));
    data.hash_insert_new_var("@cnt", inttostr(cnt));

    data.hash_insert_new_var("display", getDisplayFromMsg(an));
    data.hash_insert_new_var("type", type);
    data.hash_insert_new_var("ext", msg->ext);
    data.hash_insert_new_var("time", date.strftime(!inHistory ? "%H:%M" : "%A, %d.%m.%Y - %H:%M"));
    data.hash_insert_new_var("body", body);

    if (msg->flag & MF_HIDE) {
      data.hash_insert_new_var("hidden?", "1");
    }

    tCntId senderID = !(msg->flag & MF_SEND) ? Ctrl->ICMessage(IMC_CNT_FIND, msg->net, (int) msg->fromUid) : 0;
    tGroupedMsgs& groupedMsgs = getWndObjects(an)->groupedMsgs;

    bool groupDisplay = false;
    bool groupTime = false;

    if (groupedMsgs.size() && groupedMsgs[0].type == msg->type) {
      sGroupedMsg& lastMsg = groupedMsgs[0];

      int timeFromLastMsg = date.getTime64() - lastMsg.time.getTime64();
      bool relativeTime = config->getInt(cfg::relativeTime);

      if (lastMsg.cnt == senderID) {
        groupTime = timeFromLastMsg <= (relativeTime ? (60 * 60 * 24 * 21) : (60 * 60));
        groupDisplay = true;
      }

      if (groupDisplay) data.hash_insert_new_var("grouped", "1");
      if (groupTime) data.hash_insert_new_var("groupTime", "1");

      if (groupTime) {
        data.hash_erase_var("time");
        data.hash_insert_new_var("time", timeToString(timeFromLastMsg) + " póŸniej");
      }
      data.hash_insert_new_var("@lastMsgTime", i64tostr(lastMsg.time.getInt64()));
      data.hash_insert_new_var("timeFromLastMsg", timeToString(timeFromLastMsg));
    }

    if (msgHandlers.find(msg->type) != msgHandlers.end()) {
      msgHandlers[msg->type]->signal(data, an);
    }

    clearGroupedMsgs(an);
    groupedMsgs.push_back(sGroupedMsg(senderID, msg->type, date));

    return tplHandler.parseTpl(&data, ("content-types/" + type).c_str());
  }

  /*
   * Message types specific methods
   */
  void Controller::_handleQuickEventTpl(param_data& data, Konnekt::UI::Notify::_insertMsg* an) {
    if (an->_message->flag & MF_QE_SHOWTIME) {
      data.hash_insert_new_var("showTime?", "1");
    }
    if (!(an->_message->flag & MF_QE_NORMAL)) {
      data.hash_insert_new_var("warning?", "1");
    }
  }

  void Controller::_handleStdMsgTpl(param_data& data, Konnekt::UI::Notify::_insertMsg* an) {
    cMessage* msg = an->_message;
    tCntId cnt = getCntFromMsg(msg);

    string extInfo = GetExtParam(msg->ext, MEX_ADDINFO);
    String title = GetExtParam(msg->ext, MEX_TITLE);

    data.hash_insert_new_var("@net", inttostr(msg->net));
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
    if (msg->flag & MF_SEND) {
      data.hash_insert_new_var("sent?", "1");
    }
  }

  void Controller::_handleSmsTpl(param_data& data, Konnekt::UI::Notify::_insertMsg* an) {
    String from = GetExtParam(an->_message->ext, Sms::extFrom);
    string gate = GetExtParam(an->_message->ext, Sms::extGate);

    if (from.length()) {
      data.hash_insert_new_var("from", from);
    }
    data.hash_insert_new_var("to", an->_message->toUid);
    data.hash_insert_new_var("gate", gate);
  }

  void Controller::_handleFileTpl(param_data& data, Konnekt::UI::Notify::_insertMsg* an) {
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
      int pos = filePath.find_last_of('\\') + 1;
      if (pos > 0) {
        data.hash_insert_new_var("fileName", filePath.substr(pos));
        data.hash_insert_new_var("path", filePath.substr(0, pos));
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
