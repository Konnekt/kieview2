/**
  *  IMessage Controller class
  *
  *  Licensed under The GNU Lesser General Public License
  *  Redistributions of files must retain the above copyright notice.
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kaway2/ kAway2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  *  @license      http://creativecommons.org/licenses/LGPL/2.1/
  */

#pragma once

#ifndef __IMESSAGECTRL_H__
#define __IMESSAGECTRL_H__

#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace Stamina;
using namespace boost;

namespace Konnekt {
  typedef void tIMCallback;

  template<typename TC, typename TR>
  TR (TC::* resolve_cast0(TR (TC::* pFunc)(void)))(void) {
    return pFunc;
  }

  template<typename TP, typename TC, typename TR>
  TR (TC::* resolve_cast1(TR (TC::* pFunc)(TP)))(TP) {
    return pFunc;
  }

  class IMController : public SharedObject<iSharedObject>, public signals::trackable {
  public:
    /* Class version */
    STAMINA_OBJECT_CLASS_VERSION(IMController, iSharedObject, Version(0,2,1,0));

  public:
    typedef function<tIMCallback(IMController*)> fOnIMessage;
    typedef signal<tIMCallback(IMController*)> sigOnIMessage;

  public:
    typedef std::map<String, signals::connection> tConnections;

    struct sObserver {
      tConnections connections;
      sigOnIMessage signal;
    };

    struct sIMsgStackItem {
      sIMessage_base* im;
      bool returnCodeSet;
      int returnCode;

      sIMsgStackItem(sIMessage_base* _im): im(_im), returnCodeSet(false), returnCode(0) { }
    };

    enum sGlobalObserver {
      beforeIM, afterIM,
      beforeActions, afterActions,
      beforeForward, afterForward
    };

    /*
     * @see registerObserver
     * @see subclassAction
     */
    struct sSubclassedAction {
      static const int notFound = -1;

      bool autoForward;
      int prevOwner;

      int parent;
      int id;

      sSubclassedAction(): id(0), parent(0), prevOwner(notFound), autoForward(false) { }

      sSubclassedAction(int _id, int _parent, bool _autoForward = false):
        id(_id), parent(_parent), prevOwner(notFound), autoForward(_autoForward) { }

      sSubclassedAction(int _id, int _parent, bool _autoForward, int _prevOwner):
        id(_id), parent(_parent), prevOwner(_prevOwner), autoForward(_autoForward) { }
    };

    typedef std::deque<sSubclassedAction> tSubclassedActions;
    typedef std::deque<sIMsgStackItem> tIMsgStack;
    typedef std::map<int, sObserver*> tObservers;
    typedef std::map<int, int> tStaticValues;

  public:
    inline IMController() {
      // setting/unsetting Ctrl global pointer
      registerObserver(IM_PLUG_INIT, bind(resolve_cast0(&IMController::_plugInit), this));
      registerObserver(IM_PLUG_DEINIT, bind(resolve_cast0(&IMController::_plugDeInit), this));
      // actions subclassing
      registerObserver(IM_UI_PREPARE, bind(resolve_cast0(&IMController::_subclass), this));

      // default values for common messages, may be overriden
      setStaticValue(IM_PLUG_NETNAME, 0);
      setStaticValue(IM_PLUG_PRIORITY, 0);
      setStaticValue(IM_PLUG_SDKVERSION, KONNEKT_SDK_V);
      setStaticValue(IM_PLUG_VERSION, 0);
      setStaticValue(IM_PLUG_CORE_V, (int) "W98");
      setStaticValue(IM_PLUG_UI_V, 0);
    }

    inline virtual ~IMController() {
      for (tObservers::iterator it = _globalObservers.begin(); it != _globalObservers.end(); it++) {
        delete it->second;
      }
      for (tObservers::iterator it = _imObservers.begin(); it != _imObservers.end(); it++) {
        delete it->second;
      }
      for (tObservers::iterator it = _actionObservers.begin(); it != _actionObservers.end(); it++) {
        delete it->second;
      }
    }

  public:
    /*
     * Process incoming IMessage
     * @see registerObserver
     */
    inline int process(sIMessage_base* msgBase) {
      // locking
      LockerCS lock(_locker);

      // set im
      setIM(msgBase);

      // looking for static values
      if (_staticValues.find(getIM()->id) != _staticValues.end()) {
        setReturnCode(_staticValues[getIM()->id]);
      }
      // notify global observers
      notifyIMObservers();

      // ui action message
      if (isAction()) {
        // notify action observers
        notifyActionObservers();

        // auto-forward action
        if (isForwardable()) {
          forwardAction();
        }
      }

      // log IMessage
      if (isReturnCodeSet()) {
        IMLOG("[IMController::process()]: id = %i, returnCode = %i", getIM()->id, getReturnCode());
      } else {
        // set error if no result
        if (Ctrl) {
          Ctrl->setError(IMERROR_NORESULT);
        }
        IMLOG("[IMController::process()]: id = %i", getIM()->id);
      }

      int returnCode = getReturnCode();
      _imStack.pop_back();

      return returnCode;
    }

    inline bool registerGlobalObserver(sGlobalObserver type, fOnIMessage f, int priority = 0, signals::connect_position pos = signals::at_back, 
      const StringRef& name = "", bool overwrite = true) 
    {
      return _registerObserver(type, f, priority, pos, name, overwrite, _globalObservers);
    }

    /*
     * Attach observer to specific IMessage
     * @param id IMessage id
     * @param f function which should be fired on @a id IMessage
     * @param priority callback priority (group)
     * @param pos callback position in group (signals::at_back, signals::at_front)
     * @param name callback name
     * @param overwrite overrides callback with the same @a name if any exists
     * @return logical true if observer was attached
     */
    inline bool registerObserver(tIMid id, fOnIMessage f, int priority = 0, signals::connect_position pos = signals::at_back, 
      const StringRef& name = "", bool overwrite = true) 
    {
      return _registerObserver(id, f, priority, pos, name, overwrite, _imObservers);
    }

    inline bool registerActionObserver(const sSubclassedAction& an, fOnIMessage f, int priority = 0, 
      signals::connect_position pos = signals::at_back, const StringRef& name = "", bool overwrite = true) 
    {
      if (_registerObserver(an.id, f, priority, pos, name, overwrite, _actionObservers)) {
        subclassAction(an);
        return true;
      }
      return false;
    }

    inline bool registerActionObserver(int id, fOnIMessage f, int priority = 0, signals::connect_position pos = signals::at_back, 
      const StringRef& name = "", bool overwrite = true) 
    {
      return _registerObserver(id, f, priority, pos, name, overwrite, _actionObservers);
    }

    /* Subclassing */
    inline bool isSublassed(int id, int parent) {
      // locking
      LockerCS lock(_locker);

      for (tSubclassedActions::iterator it = _subclassedActions.begin(); it != _subclassedActions.end(); it++) {
        if (it->id == id && it->parent == parent) return true;
      }
      return false;
    }

    inline bool isSublassed() {
      if (!isAction()) return false;
      return isSublassed(getAN()->act.id, getAN()->act.parent);
    }

    inline bool isForwardable(int id, int parent) {
      if (isSublassed(id, parent)) {
        return _getSubclassedAction(id, parent)->autoForward;
      }
      return false;
    }

    inline bool isForwardable() {
      if (!isAction()) return false;
      return isForwardable(getAN()->act.id, getAN()->act.parent);
    }

    inline int getPrevOwner(int id, int parent) {
      if (isSublassed(id, parent)) {
        return _getSubclassedAction(id, parent)->prevOwner;
      }
      return sSubclassedAction::notFound;
    }

    inline int getPrevOwner() {
      if (!isAction()) return sSubclassedAction::notFound;
      return getPrevOwner(getAN()->act.id, getAN()->act.parent);
    }

    inline void setForwardable(int id, int parent, bool set) {
      if (isSublassed(id, parent)) {
        _getSubclassedAction(id, parent)->autoForward = set;
      }
    }

    inline void setForwardable(bool set) {
      if (!isAction()) return;
      setForwardable(getAN()->act.id, getAN()->act.parent, set);
    }

    inline void subclassAction(const sSubclassedAction& an) {
      if (!isSublassed(an.id, an.parent)) {
        _subclassedActions.push_back(an);
      }
    }

    inline void subclassAction(int id, int parent, bool autoForward = false) {
      subclassAction(sSubclassedAction(id, parent, autoForward));
    }

    inline IMController* forwardAction() {
      if (isAction() && isSublassed()) {
        if (notifyGlobalObservers(beforeForward)) {
          setReturnCode(Ctrl->IMessageDirect(IM_UIACTION, getPrevOwner(), (int) getAN()));
          notifyGlobalObservers(afterForward);
        }
      }
      return this;
    }

    inline int getReturnCode() {
      // locking
      LockerCS lock(_locker);

      return getIMsgStackItem()->returnCode;
    }

    inline void setReturnCode(int code) {
      // locking
      LockerCS lock(_locker);

      getIMsgStackItem()->returnCodeSet = true;
      getIMsgStackItem()->returnCode = code;
    }

    /*
     * Set string as return value
     * @warning { this method uses internal buffer which can be overridden by
     * commonly used functions like @c GETSTR in all flavors. }
     */
    inline void setReturnValue(const StringRef& value) {
      // creating temp buffer
      char* buff = (char*) Ctrl->GetTempBuffer(value.size() + 1);

      // cleaning memory
      memset(buff, 0, value.size() + 1);

      // copying data
      memcpy(buff, value.a_str(), value.size());

      // setting return code to pointer to internal, temp buffer
      setReturnCode((int) buff);
    }

    inline void setSuccess() {
      setReturnCode(1);
    }

    inline void setFailure() {
      setReturnCode(-1);
    }

    inline bool isReturnCodeSet() {
      return getIMsgStackItem()->returnCodeSet;
    }

    inline sIMessage_2params* getIM() {
      return static_cast<sIMessage_2params*>(getIMsgStackItem()->im);
    }

    inline bool isAction() {
      return getIM()->id == IM_UIACTION;
    }

    inline sUIActionNotify_2params* getAN() {
      if (!isAction()) return NULL;
      return static_cast<sUIActionNotify_2params*>((sUIActionNotify_base*) getIM()->p1);
    }

    inline void setStaticValue(int id, int value) {
      // locking
      LockerCS lock(_locker);

      _staticValues[id] = value;
    }

    inline bool isObserved(int id) {
      return _isObserved(id, _imObservers);
    }

    inline bool isActionObserved(int id) {
      return _isObserved(id, _actionObservers);
    }

  protected:
    inline sIMsgStackItem* getIMsgStackItem() {
      // locking
      LockerCS lock(_locker);

      return &_imStack.at(_imStack.size() - 1);
    }

    inline bool notifyGlobalObservers(sGlobalObserver type) {
      try {
        _notifyObservers(type, _globalObservers);
        return true;
      } catch (const Exception& e) {
        IMLOG("[IMController::notifyGlobalObservers()]: exception caught, reason = %s", e.getReason().a_str());
      } catch (const exception& e) {
        IMLOG("[IMController::notifyGlobalObservers()]: exception caught, reason = %s", e.what());
      }
      return false;
    }

    inline void notifyIMObservers() {
      if (notifyGlobalObservers(beforeIM)) {
        _notifyObservers(getIM()->id, _imObservers);
        notifyGlobalObservers(afterIM);
      }
    }

    inline void notifyActionObservers() {
      if (notifyGlobalObservers(beforeActions)) {
        _notifyObservers(getAN()->act.id, _actionObservers);
        notifyGlobalObservers(afterActions);
      }
    }

    // dumb setter
    inline void setIM(sIMessage_base* msgBase) { 
      // locking
      LockerCS lock(_locker);

      _imStack.push_back(sIMsgStackItem(msgBase));
    }

    /* Actions subclassing */
    inline sSubclassedAction* _getSubclassedAction(int id, int parent) {
      // locking
      LockerCS lock(_locker);

      int i = 0;
      for (tSubclassedActions::iterator it = _subclassedActions.begin(); it != _subclassedActions.end(); it++, i++) {
        if (id == it->id && parent == it->parent) return &_subclassedActions.at(i);
      }
      return NULL;
    }

    /* Observers related methods */
    inline bool _isObserved(int id, tObservers& list) {
      // locking
      LockerCS lock(_locker);

      if (list.find(id) != list.end()) {
        return !list[id]->signal.empty();
      }
      return false;
    }

    /* inline void _dbgObservers() {
      for (tObservers::iterator it = _imObservers.begin(); it != _imObservers.end(); it++) {
        for (tConnections::iterator it2 = it->second->connections.begin(); it2 != it->second->connections.end(); it2++) {
          IMLOG("IMObserver[%i].connection: %s", it->first, it2->first.c_str());
        }
      }
      for (tObservers::iterator it = _actionObservers.begin(); it != _actionObservers.end(); it++) {
        for (tConnections::iterator it2 = it->second->connections.begin(); it2 != it->second->connections.end(); it2++) {
          IMLOG("ActionObserver[%i].connection: %s", it->first, it2->first.c_str());
        }
      }
    } */

    inline void _notifyObservers(int id, tObservers& list) {
      // locking
      LockerCS lock(_locker);

      if (!_isObserved(id, list)) {
        return;
      }
      list[id]->signal(this);
    }

    inline bool _registerObserver(int id, fOnIMessage f, int priority, signals::connect_position pos, 
      StringRef name, bool overwrite, tObservers& list) 
    {
      // locking
      LockerCS lock(_locker);

      if (f.empty()) {
        return false;
      }
      if (list.find(id) == list.end()) {
        list[id] = new sObserver;
      }
      if (!name.length()) {
        name = "unnamed." + inttostr(list[id]->connections.size());
      }
      if (list[id]->connections.find(name) != list[id]->connections.end()) {
        if (overwrite) {
          list[id]->connections[name].disconnect();
        } else {
          return false;
        }
      }
      return (list[id]->connections[name] = list[id]->signal.connect(priority, f, pos)).connected();
    }

    /* Callbacks */
    inline tIMCallback _plugInit() {
      Plug_Init(getIM()->p1, getIM()->p2);
      return setSuccess();
    }

    inline tIMCallback _plugDeInit() {
      Plug_Deinit(getIM()->p1, getIM()->p2);
      return setSuccess();
    }

    inline tIMCallback _subclass() {
      // locking
      LockerCS lock(_locker);

      for (tSubclassedActions::iterator it = _subclassedActions.begin(); it != _subclassedActions.end(); it++) {
        sUIActionInfo nfo(it->parent, it->id);
        nfo.mask = UIAIM_ALL;
        nfo.txt = new char[100];
        nfo.txtSize = 99;

        UIActionGet(nfo);
        if (!(it->prevOwner = Ctrl->ICMessage(IMI_ACTION_GETOWNER, (int)&nfo.act))) {
          it->prevOwner = Ctrl->ICMessage(IMC_PLUG_ID, 0);
        }

        IMLOG("[IMController::subclass()]: name = %s, id = %i, parent = %i, prevOwner = %i", nfo.txt, 
          it->id, it->parent, it->prevOwner);

        Ctrl->ICMessage(IMI_ACTION_REMOVE, (int)&nfo.act);
        Ctrl->ICMessage(IMI_ACTION, (int)&nfo);
        delete [] nfo.txt;
      }
    }

  protected:
    CriticalSection _locker;
    tSubclassedActions _subclassedActions;
    tStaticValues _staticValues;
    tObservers _actionObservers;
    tObservers _globalObservers;
    tObservers _imObservers;
    tIMsgStack _imStack;
  };
}

#endif // __IMESSAGECTRL_H__