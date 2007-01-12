/**
  *  Message class
  *
  *  Licensed under The GNU Lesser General Public License
  *  Redistributions of files must retain the above copyright notice.
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kaway2/ kAway2 plugin SVN Repo
  *  @version      $Revision: 58 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-11-12 19:22:00 +0100 (N, 12 lis 2006) $
  *  @license      http://creativecommons.org/licenses/LGPL/2.1/
  */

#include "stdafx.h"
#include "Message.h"

void Message::reply(cMessage *msg, const StringRef& body, const StringRef& ext, bool html) {
  Message::send(msg->fromUid, "", msg->net, body, msg->type, ext, html);
}

cMessage Message::prepare(const StringRef& to, const StringRef& from, int net, const StringRef& body, 
  int type, const StringRef& ext, int flag) {
  cMessage msg;

  msg.flag = flag;
  msg.fromUid = (char*) from.a_str();
  msg.toUid = (char*) to.a_str();
  msg.net = net;
  msg.time = _time64(0);
  msg.type = type;
  msg.body = (char*) body.a_str();
  msg.ext = (char*) ext.a_str();

  return msg;
}

void Message::send(cMessage *msg) {
  sMESSAGESELECT ms;
  if ((ms.id = Ctrl->ICMessage(IMC_NEWMESSAGE, (int)msg)) > 0) {
    Ctrl->ICMessage(IMC_MESSAGEQUEUE, (int)&ms);
  }
  logDebug("[Message::send().msg]: id = %i, net = %i, fromUid = %s, toUid = %s, body = %s", 
    ms.id, msg->net, nullChk(msg->fromUid), nullChk(msg->toUid), msg->body);
}

void Message::send(int cnt, const StringRef& from, const StringRef& body, int type, const StringRef& ext, bool html, bool inject) {
  String to = GETCNTC(cnt, CNT_UID);
  cMessage msg = Message::prepare(to, from, GETCNTI(cnt, CNT_NET), body, type, ext, MF_SEND | (html ? MF_HTML : 0));

  if (inject) {
    Message::inject(&msg, cnt);
  }
  Message::send(&msg);
}

void Message::send(const StringRef& to, const StringRef& from, int net, const StringRef& body, int type, const StringRef& ext, bool html) {
  cMessage msg = Message::prepare(to, from, net, body, type, ext, MF_SEND | (html ? MF_HTML : 0));
  Message::send(&msg);
}

void Message::sms(const StringRef& to, const StringRef& body, const StringRef& gate, const StringRef& from, StringRef ext) {
  ext = SetExtParam(ext, Sms::extFrom, from); // nie zawsze wymagany
  ext = SetExtParam(ext, Sms::extGate, gate);  // wymagany!
  // Je¿eli NIE chcemy, ¿eby wtyczka SMS dzieli³a wiadomoœæ, ustawiamy parametr extPart...
  // ext = SetExtParam(ext, Sms::extPart, "0");

  cMessage msg = Message::prepare(to, "", Sms::net, body, MT_SMS, ext, MF_SEND);
  Message::send(&msg);
}

void Message::inject(cMessage *msg, int cntID, const char * display, bool scroll) {
  Konnekt::UI::Notify::_insertMsg ni(msg, display, scroll);
  ni.act = sUIAction(IMIG_MSGWND, Konnekt::UI::ACT::msg_ctrlview, cntID);

  UIActionCall(&ni);
}