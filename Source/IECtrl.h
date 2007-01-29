/**
  *  IECtrl class
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

#pragma once

#ifndef __IECTRL_H__
#define __IECTRL_H__

#include <objbase.h>
#include <exdisp.h>
#include <initguid.h>
#include <shlguid.h>
#include <memory.h>
#include <shlobj.h>
#include <mshtml.h>
#include <mshtmhst.h>
#include <dispex.h> // @@@
#include <oleauto.h>
#include <comutil.h>
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "comsuppw.lib")
#include <stdio.h>
#include <crtdbg.h>

// na wszelki wypadek ;>
static const CLSID CLSID_MozillaBrowser = { 0x1339B54C, 0x3453, 0x11D2, { 0x93, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

class IECtrl {
private:
  class ClientSite;
  class EventSink;
  class External;

public:
  class Var;

  class AnchorClickListener {
  public:
    virtual void anchorClicked(const char* url, IECtrl* ctrl) = 0;
  };

  class PopupMenuListener {
  public:
    enum MenuType {
      Unknown,
      Anchor,
      Selection,
      Image
    };
    enum MakeAction {
      None = 0,
      SelectAll = 1,
      CopyLink = 2,
      CopySelection = 3,
      OpenLink = 4,
      SaveImage = 2270,
      Print = 27,
      ShowSource = 2139
    };
    virtual MakeAction popupMenu(MenuType type, POINT pt, IECtrl* ctrl) = 0;
  };

  class DropListener {
  public:
    typedef std::vector<const char*> tFiles;

  public:
    virtual void fileDropped(const char *url, IECtrl* ctrl) = 0;
    virtual void fileDropped(const tFiles& urls, IECtrl* ctrl) {
      for (tFiles::const_iterator it = urls.begin(); it != urls.end(); it++) {
        fileDropped(*it, ctrl);
      }
    }
  };

  class KeyDownListener {
  public:
    virtual bool keyDown(UINT uCode, DWORD dwFlags) = 0; // zwraca true jesli pozwala przepuscic klawisz
  };

  class ExternalListener {
  public:
    virtual long getMemberID(const char *name) = 0;
    virtual string getMemberName(long id) = 0;
    virtual Var trigger(long id, Var &args, IECtrl* ctrl, bool construct = false) = 0;
    virtual bool isObject(long id) = 0;
  };

  class ScriptMessageListener {
  public:
    // dwType tak jak MessageBox, zwraca to co MessageBox
    virtual int showMessage(HWND hWnd, const char * lpText, DWORD dwType) = 0;
  };

public:
  IECtrl(HWND parent, int x, int y, int cx, int cy, bool staticEdge = false);
  virtual ~IECtrl();

  static void init();
  static void deinit();
  static IECtrl * get(HWND);

  static LRESULT CALLBACK IECtrlServerWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK IECtrlDocWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK IECtrlWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

  HWND getHWND();
  void setWindowPos(int x, int y, int cx, int cy);

  void enableSandbox(bool bSandbox = true);
  void navigate(const char *);
  void clear();
  void write(const WCHAR *text);
  void write(const char *text);
  void close();
  bool isScrollOnBottom();
  void scrollToTop();
  void scrollToBottom();
  void saveDocument();
  void selectAll();
  bool callJScript(const char* szFunc, Var &args, Var *ret);

  static std::string humanize(const char* text);

  static void setAutoCopySel(bool autoCopy);
  static bool getAutoCopySel();

  static void setOnCopyEmptySel(bool emptySel) {
    m_bOnCopyEmptySel = emptySel;
  }
  static bool getOnCopyEmptySel() {
    return m_bOnCopyEmptySel;
  }

  char* getSelection(bool gettext = false);
  bool copySelection(bool gettext = false);

  void setAnchorClickListener(AnchorClickListener * listener) { m_pAnchorClickListener = listener; }
  void setPopupMenuListener(PopupMenuListener * listener) { m_pPopupMenuListener = listener; }
  void setDropListener(DropListener * listener) { m_pDropListener = listener; }
  void setKeyDownListener(KeyDownListener * listener) { m_pKeyDownListener = listener; }
  void setExternalListener(ExternalListener * listener) { m_pExternalListener = listener; }
  void setScriptMessageListener(ScriptMessageListener * listener) { m_pScriptMessageListener = listener; }

  IHTMLDocument2* getDocument();

  IDispatch* getDispatch() {
    IHTMLDocument2* document = getDocument();
    IDispatch* dispatch;
    document->get_Script(&dispatch);

    return dispatch;
  }

private:
  BSTR getHrefFromAnchor(IHTMLElement* element);
  BSTR getSelectionFunc(bool gettext = false);

  void translateAccelerator(UINT uMsg, WPARAM wParam, LPARAM lParam);
  bool mouseClick(POINT pt);
  bool setFocus(HWND prevFocus);

  void setUserWndProc(WNDPROC);
  WNDPROC getUserWndProc();

  // podlaczanie zdarzen (events) do IDispatch i tylko metoda Invoke -> DISPID_HTMLELEMENTEVENTS2_ONCLICK
  // to nie bylo uzywanie i nie testowane
  DWORD connectEvents(IHTMLElement* pElem, IDispatch* pDisp);
  void disconnectEvents(IHTMLElement* pElem, DWORD dwCookie);

  static bool m_bInited;
  HWND m_hParentWnd;
  HWND m_hWnd;

  static CRITICAL_SECTION m_mutex;
  static IECtrl * m_pList;
  IECtrl * m_pPrev;
  IECtrl * m_pNext;

  RECT m_rcClient;
  BOOL m_bInPlaceActive;
  DWORD m_dwCookie;
  IConnectionPoint * m_pConnectionPoint;
  EventSink * m_pEventSink;
  ClientSite * m_pClientSite;
  IDispatch * m_pExternal;
  IDropTarget * m_pDropTarget;
  IWebBrowser2 * m_pWebBrowser;

  WNDPROC m_fUserWndProc;
  bool m_bGetFocus;
  bool m_bGetSelection;
  char * m_szSelectedText;
  bool m_bClosed;
  bool m_bSandbox;

  static bool m_bOnCopyEmptySel;
  static bool m_bAutoCopySel;

  AnchorClickListener * m_pAnchorClickListener;
  PopupMenuListener * m_pPopupMenuListener;
  DropListener * m_pDropListener;
  KeyDownListener * m_pKeyDownListener;
  ExternalListener * m_pExternalListener;
  ScriptMessageListener * m_pScriptMessageListener;

private:
  class ClientSite : public IOleClientSite, 
                     public IOleInPlaceSite, 
                     public IDocHostUIHandler,
                     public IDocHostShowUI,
                     public IServiceProvider,
                     public IInternetSecurityManager
  {
  public:
    ClientSite(IECtrl* pCtrl);
    virtual ~ClientSite();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);
    // IOleWindow
    STDMETHOD(GetWindow)(HWND *phwnd);
    STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

    // IOleInPlace
    STDMETHOD(CanInPlaceActivate)(void);
    STDMETHOD(OnInPlaceActivate)(void);
    STDMETHOD(OnUIActivate)(void);
    STDMETHOD(GetWindowContext)(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
    STDMETHOD(Scroll)(SIZE scrollExtant);
    STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
    STDMETHOD(OnInPlaceDeactivate)(void);
    STDMETHOD(DiscardUndoState)(void);
    STDMETHOD(DeactivateAndUndo)(void);
    STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

    // IOleClientSite
    STDMETHOD(SaveObject)(void);
    STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
    STDMETHOD(GetContainer)(IOleContainer **ppContainer);
    STDMETHOD(ShowObject)(void);
    STDMETHOD(OnShowWindow)(BOOL fShow);
    STDMETHOD(RequestNewObjectLayout)(void);

    // IDocHostUIHandler
    STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc);
    STDMETHOD(HideUI)(void);
    STDMETHOD(UpdateUI)(void);
    STDMETHOD(EnableModeless)(BOOL fEnable);
    STDMETHOD(OnDocWindowActivate)(BOOL fEnable);
    STDMETHOD(OnFrameWindowActivate)(BOOL fEnable);
    STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
    STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw);
    STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
    STDMETHOD(GetExternal)(IDispatch **ppDispatch);
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
    STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet);

    // IDocHostShowUI
    STDMETHOD(ShowHelp)(HWND hwnd, LPOLESTR pszHelpFile, UINT uCommand, DWORD dwData, POINT ptMouse, IDispatch __RPC_FAR *pDispatchObjectHit);
    STDMETHOD(ShowMessage)(HWND hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT __RPC_FAR *plResult);

    // IServiceProvider
    STDMETHOD(QueryService)(REFGUID guidService, REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject);

    // IInternetSecurityManager
    STDMETHOD(SetSecuritySite)(IInternetSecurityMgrSite *pSite);
    STDMETHOD(GetSecuritySite)(IInternetSecurityMgrSite **ppSite);
    STDMETHOD(MapUrlToZone)(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags);
    STDMETHOD(GetSecurityId)(LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved);
    STDMETHOD(ProcessUrlAction)(LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved);
    STDMETHOD(QueryCustomPolicy)(LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved);
    STDMETHOD(SetZoneMapping)(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags);
    STDMETHOD(GetZoneMappings)(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags);

  private:
    LONG m_cRef;
    IECtrl * m_pCtrl;
  };

private:
  class External : public IDispatchEx {
  public:
    External(IECtrl* pCtrl);
    virtual ~External();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // IDispatch
    STDMETHOD(GetTypeInfoCount)(UINT*);
    STDMETHOD(GetTypeInfo)(UINT, LCID, LPTYPEINFO*);
    STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR*, UINT, LCID, DISPID*);
    STDMETHOD(Invoke)(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);

    // IDispatchEx
    STDMETHOD(DeleteMemberByName)(BSTR bstrName, DWORD grfdex);
    STDMETHOD(DeleteMemberByDispID)(DISPID id);
    STDMETHOD(GetDispID)(BSTR bstrName, DWORD grfdex, DISPID *pid);
    STDMETHOD(GetMemberName)(DISPID id, BSTR *pbstrName);
    STDMETHOD(GetMemberProperties)(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex);
    STDMETHOD(GetNameSpaceParent)(IUnknown **ppunk);
    STDMETHOD(GetNextDispID)(DWORD grfdex, DISPID id, DISPID *pid);
    STDMETHOD(InvokeEx)(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller);

  private:
    LONG m_cRef;
    IECtrl * m_pCtrl;
    long m_lobjID;
  };

private:
  class EventSink : public DWebBrowserEvents2 {
  public:
    EventSink(IECtrl* pCtrl);
    virtual ~EventSink();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // IDispatch
    STDMETHOD(GetTypeInfoCount)(UINT*);
    STDMETHOD(GetTypeInfo)(UINT, LCID, LPTYPEINFO*);
    STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR*, UINT, LCID, DISPID*);
    STDMETHOD(Invoke)(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);

    // DWebBrowserEvents2
    STDMETHODIMP_(void) StatusTextChange(BSTR);
    STDMETHODIMP_(void) ProgressChange(long, long);
    STDMETHODIMP_(void) CommandStateChange(long, VARIANT_BOOL);
    STDMETHODIMP_(void) DownloadBegin();
    STDMETHODIMP_(void) DownloadComplete();
    STDMETHODIMP_(void) TitleChange(BSTR Text);
    STDMETHODIMP_(void) PropertyChange(BSTR Text);
    STDMETHODIMP_(void) BeforeNavigate2(IDispatch*,VARIANT*,VARIANT*,VARIANT*,VARIANT*,VARIANT*,VARIANT_BOOL*);
    STDMETHODIMP_(void) NewWindow2(IDispatch**, VARIANT_BOOL*);
    STDMETHODIMP_(void) NavigateComplete(IDispatch*, VARIANT*);
    STDMETHODIMP_(void) DocumentComplete(IDispatch*, VARIANT*);
    STDMETHODIMP_(void) OnQuit();
    STDMETHODIMP_(void) OnVisible(VARIANT_BOOL);
    STDMETHODIMP_(void) OnToolBar(VARIANT_BOOL);
    STDMETHODIMP_(void) OnMenuBar(VARIANT_BOOL);
    STDMETHODIMP_(void) OnStatusBar(VARIANT_BOOL);
    STDMETHODIMP_(void) OnFullScreen(VARIANT_BOOL);
    STDMETHODIMP_(void) OnTheaterMode(VARIANT_BOOL);
    STDMETHODIMP_(void) WindowSetResizable(VARIANT_BOOL);
    STDMETHODIMP_(void) WindowSetLeft(long);
    STDMETHODIMP_(void) WindowSetTop(long);
    STDMETHODIMP_(void) WindowSetWidth(long);
    STDMETHODIMP_(void) WindowSetHeight(long);
    STDMETHODIMP_(void) WindowClosing(VARIANT_BOOL, VARIANT_BOOL*);
    STDMETHODIMP_(void) ClientToHostWindow(long*,long*);
    STDMETHODIMP_(void) SetSecureLockIcon(long);
    STDMETHODIMP_(void) FileDownload(VARIANT_BOOL*);

  private:
    LONG m_cRef;
    IECtrl * m_pCtrl;
  };

public:
  class Object;
  class Var {
  public:
    Var();
    Var(int value);
    Var(bool value);
    Var(double value);
    Var(const char * value);
    Var(const Var & copy);
    Var(Var & copy);
    Var(VARIANT &v);
    Var(Date64 &v);
    Var(Object &v);
    Var(IDispatch *v);
    ~Var();

    int getInteger();
    bool getBool();
    double getReal();
    const char * getString();
    VARIANT * getVariant(VARIANT *v = NULL);
    Date64 getDate();
    Object getObject(IECtrl* ctrl = NULL);
    IDispatch* getDispatch();

    void setValue(int value);
    void setValue(bool value);
    void setValue(double value);
    void setValue(const char * value);
    void setValue(Var* value[], unsigned int count);
    void setValue(VARIANT &v);
    void setValue(Date64 &value);
    void setValue(Object &value);
    void setValue(IDispatch* value);

    int length();
    Var & operator[](int i);

    const Var & operator=(const Var &copy);
    Var & operator=(Var &copy);
    Var & operator=(int value);
    Var & operator=(double value);
    Var & operator=(const char * value);
    Var & operator=(VARIANT &v);
    Var & operator=(Date64 &value);
    Var & operator=(Object &value);
    Var & operator=(IDispatch* value);

    void operator+=(Var & var);
    void operator+=(int var);
    void operator+=(double var);
    void operator+=(const char* var);
    void operator+=(Date64 &var);

    Var operator+(Var & var2);
    Var operator+(int var2);
    Var operator+(double var2);
    Var operator+(const char *var2);
    Var operator+(Date64 &var);

  private:
    void clear();
    void copy(Var & copy);

    Var & getElement(int i);

  private:
    enum Type {
      Unknown,
      Integer,
      Boolean,
      Real,
      String,
      Array,
      Date,
      Object,
      Dispatch
    };
    Type m_eType;

    union {
      int m_iValue;
      double m_dValue;
      char * m_szValue;
      Var ** m_aValue;
      Date64 * m_dtValue;
      IECtrl::Object * m_objValue;
      IDispatch * m_dispValue;
    };
    int m_nLength;
  };

  class Object {
  public:
    Object() { }
    Object(IECtrl* ctrl, bool defaultDestruct = true, const char* name = "Object", Var args = Var());
    Object(IECtrl* ctrl, IDispatch* dispatch);
    Object(const Object &object);
    virtual ~Object();

    void bindMethod(const char* name, const char* func);

    void addProperty(const char *name, IECtrl::Var var, bool ref = true);
    void setProperty(const char *name, IECtrl::Var var, bool ref = true);

    IECtrl::Var getProperty(const char *name);
    VARIANT* getVariant(VARIANT* v);

    IDispatchEx* getDispatchEx() {
      return _pdexObj;
    }

    inline bool ownObject() {
      return _ownObject;
    }

  protected:
    DISPID getDispID(IDispatchEx* dispatch, const char* name, DWORD flags = 0);

  protected:
    IDispatchEx* _pdexScript;
    IDispatchEx* _pdexObj;
    IDispatch* _pdispObj;
    bool _ownObject;
    bool _defDestruct;
    UINT* _refs;
  };

  class DropTarget : public IDropTarget {
  public:
    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // IDropTarget
    STDMETHOD(DragEnter)(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    STDMETHOD(DragOver)(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    STDMETHOD(DragLeave)(void);
    STDMETHOD(Drop)(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

    // Constructor
    DropTarget(IECtrl* ctrl);
    ~DropTarget();

  private:
    // internal helper function
    DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
    bool QueryDataObject(IDataObject *pDataObject);
    void DropData(IDataObject *pDataObject);

    // Private member variables
    long m_cRef;
    bool m_fAllowDrop;

    IECtrl* m_pCtrl;
  };

  class iObject : public IDispatchEx {
  public:
    typedef function<IECtrl::Var(IECtrl::Var&, IECtrl*)> fCallback;
    typedef signal<IECtrl::Var(IECtrl::Var&, IECtrl*)> CallbackSig;

    struct sCallback {
      CallbackSig signal;
      string name;
      long id;

      sCallback(const StringRef& _name, fCallback f): name(_name), id(random()) {
        if (!f.empty()) signal.connect(f);
      }
    };

    struct sValue {
      IECtrl::Var var;
      long id;
      sValue(): id(random()) {}
    };

    typedef std::vector<sCallback*> tCallbacks;
    typedef std::map <string, sValue*> tValues;

  public:
    iObject(IECtrl* pCtrl);
    virtual ~iObject();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // IDispatch
    STDMETHOD(GetTypeInfoCount)(UINT*);
    STDMETHOD(GetTypeInfo)(UINT, LCID, LPTYPEINFO*);
    STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR*, UINT, LCID, DISPID*);
    STDMETHOD(Invoke)(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);

    // IDispatchEx
    STDMETHOD(InvokeEx)(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller) { 
      return HRESULT(0);
    }
    STDMETHOD(GetDispID)(BSTR bstrName, DWORD grfdex, DISPID *pid) { return HRESULT(0); }
    STDMETHOD(DeleteMemberByName)(BSTR bstrName, DWORD grfdex) { return HRESULT(0); }
    STDMETHOD(DeleteMemberByDispID)(DISPID id) { return HRESULT(0); }
    STDMETHOD(GetMemberProperties)(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex) { return HRESULT(0); }
    STDMETHOD(GetMemberName)(DISPID id, BSTR *pbstrName) { return HRESULT(0); }
    STDMETHOD(GetNextDispID)(DWORD grfdex, DISPID id, DISPID *pid) { return HRESULT(0); }
    STDMETHOD(GetNameSpaceParent)(IUnknown **ppunk) { return HRESULT(0); }

    virtual sCallback* registerCallback(const char* name, fCallback f);
    virtual bool deleteCallback(const char* name);

    virtual sCallback* getCallback(const char* name);
    virtual sCallback* getCallback(long id);
    virtual long getMemberID(const char *name);

    virtual bool hasProperty(const string& name);
    virtual bool hasProperty(long id);

    virtual IECtrl::Var getProperty(const string& name);
    virtual IECtrl::Var getProperty(long id);

    virtual long getPropertyID(const string& name);
    virtual string getPropertyName(long id);

    virtual void setProperty(const string& name, IECtrl::Var v);

  protected:
    virtual IECtrl::Var trigger(long id, IECtrl::Var& args, IECtrl* ctrl);

  protected:
    tCallbacks _callbacks;
    tValues _values;

  private:
    IECtrl* m_pCtrl;
    LONG m_cRef;
  };
};

#endif // __IECTRL_H__