/**
  *  kIEview2 Control class
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

#pragma once

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "kIEview2.h"
#include "PlugController.h"
#include "Helpers.h"
#include "IECtrl.h"
#include "ActionHandler.h"
#include "TplHandler.h"
#include "TplUdf.h"
#include "RtfHtml.h"

using namespace kIEview2;

namespace kIEview2 {
  class Controller : public PlugController<Controller> {
  public:
    friend class PlugController<Controller>;

  public:
    typedef std::map<IECtrl*, ActionHandler*> tActionHandlers;

    struct sMsg {
      Date64 time;
      tCntId cnt;
    };

  public:
    /* Class version */
    STAMINA_OBJECT_CLASS_VERSION(Controller, PlugController, Version(0,1,0,0));

  protected:
    Controller();

  public:
    ~Controller();

    string getMsgTypeLabel(int type);
    String getStatusLabel(int status);

    string bytesToString(double bytes);
    string timeToString(int time);

    String htmlEscape(StringRef& txt);
    String linkify(StringRef& txt);
    String nl2br(StringRef& txt);

  protected:
    void _onPrepare();
    void _onAction();

    void _msgCtrlView();
    void _msgCtrlSend();
    void _msgSend();

    tCntId getCntFromMsg(cMessage* msg);
    String getDisplayFromMsg(UI::Notify::_insertMsg* an);
    bool isMsgFromHistory(sUIActionNotify_base* an);
    void handleTextFlag(int flag);

    String _parseStatusTpl(UI::Notify::_insertStatus* an);
    String _parseMsgTpl(UI::Notify::_insertMsg* an);

    void _handleQuickEventTpl(param_data& data, UI::Notify::_insertMsg* an);
    void _handleStdMsgTpl(param_data& data, UI::Notify::_insertMsg* an);
    void _handleSmsTpl(param_data& data, UI::Notify::_insertMsg* an);
    void _handleFileTpl(param_data& data, UI::Notify::_insertMsg* an);

  public:
    void clearWnd(IECtrl* ctrl);
    static DWORD CALLBACK streamOut(DWORD, LPBYTE, LONG, LONG*);

  protected:
    tActionHandlers actionHandlers;
    TplHandler* tplHandler;
    RtfHtmlTag* rtfHtml;
  };
}

#endif // __CONTROLLER_H__