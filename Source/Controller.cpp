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
  SharedPtr<Controller> Controller::instance = 0;

  Controller::Controller() {
    this->config = new CfgController(this);

    /* Static values like net, type or version */
    this->setStaticValue(IM_PLUG_TYPE, IMT_CONFIG | IMT_MSGUI | IMT_UI);
    this->setStaticValue(IM_PLUG_PRIORITY, PLUGP_HIGH + 1);
    this->setStaticValue(IM_PLUG_NAME, (int) "kIEview2");
    this->setStaticValue(IM_PLUG_SIG, (int) sig);
    this->setStaticValue(IM_PLUG_NET, net);

    /* Callbacks */
    this->registerObserver(IM_UI_PREPARE, bind(resolve_cast0(&Controller::_onPrepare), this));
    this->registerActionObserver(UI::ACT::msg_ctrlview, bind(resolve_cast0(&Controller::_msgCtrlView), this));
    this->registerActionObserver(UI::ACT::msg_ctrlsend, bind(resolve_cast0(&Controller::_msgCtrlSend), this));
    this->registerActionObserver(IMIA_MSG_SEND, bind(resolve_cast0(&Controller::_msgSend), this));

    // podmieniamy kontrolki widoku wiadomosci
    this->subclassAction(UI::ACT::msg_ctrlview, IMIG_MSGWND);
    // podmieniamy kontrolki w historii
    this->subclassAction(UI::ACT::msg_ctrlview, IMIG_HISTORYWND);
    // podmieniamy kontrolki wpisywania wiadomosci
    this->subclassAction(UI::ACT::msg_ctrlsend, IMIG_MSGWND);
    // podmieniamy przycisk "Wyœlij" z okna rozmowy
    this->subclassAction(IMIA_MSG_SEND, IMIG_MSGTB);

    IECtrl::init();
  }

  Controller::~Controller() {
    IECtrl::deinit();
  }

  void Controller::_msgCtrlView() {
  }

  void Controller::_msgCtrlSend() {
  }

  void Controller::_msgSend() {
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

    // menu akcji pod prawym klawiszem myszy
    UIGroupAdd(IMIG_MSGWND, act::popup::popup, 0);
    UIActionAdd(act::popup::popup, act::popup::openUrl, ACTSMENU_BOLD | ACTR_INIT, "Otwórz", ico::link);
    UIActionAdd(act::popup::popup, act::popup::copyUrl, 0, "Kopiuj adres");
    UIActionAdd(act::popup::popup, 0, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::saveImage, ACTSMENU_BOLD | ACTR_INIT, "Zapisz obrazek", ico::save);
    UIActionAdd(act::popup::popup, 0, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::copySelection, ACTR_INIT, "Kopiuj", ico::copy);
    UIActionAdd(act::popup::popup, act::popup::selectAll, 0, "Zaznacz wszystko");
    UIActionAdd(act::popup::popup, act::popup::history, ACTR_INIT, "Poprzednia rozmowa");
    UIActionAdd(act::popup::popup, 0, ACTT_SEP);
    UIActionAdd(act::popup::popup, act::popup::clear, 0, "Wyczyœæ okno", 0x74);
  }
}