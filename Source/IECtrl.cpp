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

#include "stdafx.h"
#include "IECtrl.h"

#define DISPID_BEFORENAVIGATE2 250

IECtrl* IECtrl::m_pList = NULL;
CRITICAL_SECTION IECtrl::m_mutex;
bool IECtrl::m_bInited = false;

void IECtrl::init() {
  if (m_bInited) return;

  m_bInited = true;
  InitializeCriticalSection(&m_mutex);

  if (FAILED(OleInitialize(NULL))) {
    MessageBox(NULL, "OleInitialize failed", "IECtrl", MB_ICONERROR);
  }
}

void IECtrl::deinit() {
  while (m_pList != NULL) {
    delete m_pList;
  }
  DeleteCriticalSection(&m_mutex);
  m_bInited = false;
}

IECtrl::IECtrl(HWND parent, int x, int y, int cx, int cy, int cntId, bool staticEdge) {
  MSG msg;
  IOleObject* pOleObject = NULL;
  IOleInPlaceObject* pOleInPlace = NULL;

  m_hParentWnd = parent;
  m_cntId = cntId;
  m_pPrev = m_pNext = NULL;
  m_hWnd = NULL;
  m_pEventSink = NULL;
  m_pClientSite = NULL;
  m_bClosed = false;
  m_bSandbox = true;
  m_pExternal = new External(this);
  m_pDropTarget = new DropTarget(this);

  m_pAnchorClickListener = NULL;
  m_pPopupMenuListener = NULL;
  m_pDropListener = NULL;
  m_pKeyDownListener = NULL;
  m_pExternalListener = NULL;
  m_pScriptMessageListener = NULL;

  m_pWebBrowser = NULL;
  m_pConnectionPoint = NULL;
  m_szSelectedText = NULL;

  if (SUCCEEDED(CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IWebBrowser2, (LPVOID*)&m_pWebBrowser))) {
    // m_pWebBrowser->put_RegisterAsBrowser(VARIANT_FALSE);
    // m_pWebBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);
    m_pWebBrowser->put_Offline(m_bSandbox ? VARIANT_TRUE : VARIANT_FALSE);

    if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IOleObject, (void**)&pOleObject))) {
      m_rcClient.left = x;
      m_rcClient.top = y;
      m_rcClient.right = x + cx;
      m_rcClient.bottom = y + cy;
      m_pClientSite = new ClientSite(this);
      pOleObject->SetClientSite(m_pClientSite);
      pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, &msg, m_pClientSite, 0, m_hParentWnd, &m_rcClient);
      pOleObject->Release();
    } else {
      MessageBox(NULL, "IOleObject failed", "IECtrl", MB_ICONERROR);
    }

    if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IOleInPlaceObject, (void**)&pOleInPlace))) {
      pOleInPlace->GetWindow(&m_hWnd);
      pOleInPlace->Release();
    } else {
      MessageBox(NULL, "IOleInPlaceObject failed", "IECtrl", MB_ICONERROR);
    }

    if (staticEdge) {
      LONG style = GetWindowLong(m_hWnd, GWL_EXSTYLE);
      style |= (WS_EX_STATICEDGE);
      SetWindowLong(m_hWnd, GWL_EXSTYLE, style);
    }

    IConnectionPointContainer* pCPContainer;
    if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPContainer))) {
      if (SUCCEEDED(pCPContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pConnectionPoint))) {
        m_pEventSink = new EventSink(this);
        if (FAILED(m_pConnectionPoint->Advise((IUnknown *)m_pEventSink, &m_dwCookie))) {
          MessageBox(NULL, "Advise failed", "IECtrl", MB_ICONERROR);
        }
      }
      pCPContainer->Release();
    }
    setUserWndProc((WNDPROC)SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG) IECtrl::IECtrlWindowProcedure));
  }

  EnterCriticalSection(&m_mutex);
  m_pNext = m_pList;
  if (m_pNext != NULL) {
    m_pNext->m_pPrev = this;
  }
  m_pList = this;
  LeaveCriticalSection(&m_mutex);

  clear();
}

IECtrl::~IECtrl() {
  IOleObject* pOleObject = NULL;

  if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IOleObject, (void**)&pOleObject)) && pOleObject != NULL) {
    pOleObject->SetClientSite(NULL);
    pOleObject->Release();
  } else {
    MessageBox(NULL, "IOleObject failed", "IECtrl", MB_ICONERROR);
  }

  EnterCriticalSection(&m_mutex);
  if (m_pList == this) {
    m_pList = m_pNext;
  } else if (m_pPrev != NULL) {
    m_pPrev->m_pNext = m_pNext;
  }
  if (m_pNext != NULL) {
    m_pNext->m_pPrev = m_pPrev;
  }
  LeaveCriticalSection(&m_mutex);

  if (m_pConnectionPoint != NULL) {
    m_pConnectionPoint->Unadvise(m_dwCookie);
    m_pConnectionPoint->Release();
  }
  if (m_pEventSink != NULL) {
    delete m_pEventSink;
  }
  if (m_szSelectedText != NULL) {
    delete m_szSelectedText;
  }
  m_pWebBrowser->Release();
  DestroyWindow(m_hWnd);
  if (m_pClientSite != NULL) {
    delete m_pClientSite;
  }
  if (m_pExternal != NULL) {
    delete m_pExternal;
  }
  if (m_pDropTarget != NULL) {
    delete m_pDropTarget;
  }
}

void IECtrl::setUserWndProc(WNDPROC wndProc) {
  m_fUserWndProc = wndProc;
}

WNDPROC IECtrl::getUserWndProc() {
  return m_fUserWndProc;
}

LRESULT CALLBACK IECtrl::IECtrlServerWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  IECtrl *ctrl = IECtrl::get(GetParent(GetParent(hwnd)));
  if (ctrl != NULL) {
    switch (message) {
      case WM_KEYUP:
        if (LOWORD(wParam) == VK_ESCAPE && !(GetKeyState(VK_SHIFT) & 0x8000) && !(GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000)) {
          SendMessage(GetParent(GetParent(GetParent(hwnd))), WM_COMMAND, IDCANCEL, 0);
        }
        break;
      case WM_KEYDOWN:
        ctrl->translateAccelerator(message, wParam, lParam);
        break;
      case WM_SYSKEYDOWN:
        ctrl->translateAccelerator(message, wParam, lParam);
        break;
      case WM_SETFOCUS:
        if (ctrl->setFocus((HWND)wParam)) {
          return true;
        }
        break;
      case WM_LBUTTONDOWN:
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        if (ctrl->mouseClick(pt)) {
          return true;
        }
        break;
    }
    return CallWindowProc(ctrl->getUserWndProc(), hwnd, message, wParam, lParam);
  }
  return DefWindowProc (hwnd, message, wParam, lParam);
}

LRESULT CALLBACK IECtrl::IECtrlDocWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  IECtrl *ctrl = IECtrl::get(GetParent(hwnd));

  if (ctrl != NULL) {
    WNDPROC oldWndProc = ctrl->getUserWndProc();
    if (message == WM_PARENTNOTIFY && wParam == WM_CREATE) {
      SetWindowLong(hwnd, GWL_WNDPROC, (LONG) oldWndProc);
      ctrl->setUserWndProc((WNDPROC) SetWindowLong((HWND)lParam, GWL_WNDPROC, (LONG) IECtrl::IECtrlServerWindowProcedure));
    }
    return CallWindowProc(oldWndProc, hwnd, message, wParam, lParam);
  }
  return DefWindowProc (hwnd, message, wParam, lParam);
}

LRESULT CALLBACK IECtrl::IECtrlWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  IECtrl *ctrl = IECtrl::get(hwnd);

  if (ctrl != NULL) {
    WNDPROC oldWndProc = ctrl->getUserWndProc();
    if (message == WM_PARENTNOTIFY && wParam == WM_CREATE) {
      SetWindowLong(hwnd, GWL_WNDPROC, (LONG) oldWndProc);
      ctrl->setUserWndProc((WNDPROC) SetWindowLong((HWND)lParam, GWL_WNDPROC, (LONG) IECtrl::IECtrlDocWindowProcedure));
    }
    return CallWindowProc(oldWndProc, hwnd, message, wParam, lParam);
  }
  return DefWindowProc (hwnd, message, wParam, lParam);
}

IHTMLDocument2* IECtrl::getDocument() {
  HRESULT hr = S_OK;
  IHTMLDocument2 *document = NULL;
  IDispatch *dispatch = NULL;

  if (SUCCEEDED(m_pWebBrowser->get_Document(&dispatch)) && dispatch != NULL)  {
    hr = dispatch->QueryInterface(IID_IHTMLDocument2, (void **)&document);
    dispatch->Release();
  }
  return document;
}

void IECtrl::setWindowPos(int x, int y, int cx, int cy)  {
  m_rcClient.left = x;
  m_rcClient.top = y;
  m_rcClient.right = cx;
  m_rcClient.bottom = cy; //y + cy;

  SetWindowPos(getHWND(), HWND_TOP, x, y, cx, cy, 0);
}

void IECtrl::scrollToTop() {
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    IHTMLWindow2* pWindow = NULL;
    if (SUCCEEDED(document->get_parentWindow(&pWindow)) && pWindow != NULL) {
      pWindow->scrollBy(-0x01FFFFFF, -0x01FFFFFF);
      pWindow->Release();
    }
    document->Release();
  }
}

void IECtrl::scrollToBottom() {
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    IHTMLElementCollection *collection;
    IHTMLElement *element;
    IDispatch *dispatch;

    if (SUCCEEDED(document->get_all(&collection)) && collection != NULL) {
      long len;

      if (SUCCEEDED(collection->get_length(&len))) {
        VARIANT  variant;
        variant.vt = VT_I4;
        variant.lVal = len-1;

        if (SUCCEEDED(collection->item(variant, variant, &dispatch)) && dispatch != NULL) {
          if (SUCCEEDED(dispatch->QueryInterface(IID_IHTMLElement,(void**)&element)) && element != NULL) {
            variant.vt = VT_BOOL;
            variant.boolVal = VARIANT_FALSE;
            if (SUCCEEDED(element->scrollIntoView(variant))) {
              //
            }
            element->Release();
          }
          dispatch->Release();
        }
      }
      collection->Release();
    }

    IHTMLWindow2* pWindow = NULL;
    if (SUCCEEDED(document->get_parentWindow(&pWindow)) && pWindow != NULL) {
      pWindow->scrollBy(-0x01FFFFFF, 0x01FFFFFF);
      pWindow->Release();
    }
    document->Release();
  }
}

void IECtrl::write(const WCHAR *text) {
  if (!m_bClosed) {
    IHTMLDocument2 *document = getDocument();
    if (document != NULL)  {
      SAFEARRAY *safe_array = SafeArrayCreateVector(VT_VARIANT, 0, 1);
      if (safe_array != NULL) {
        VARIANT  *variant;
        BSTR bstr;
        SafeArrayAccessData(safe_array, (LPVOID *)&variant);
        variant->vt = VT_BSTR;
        variant->bstrVal = bstr = SysAllocString(text);
        SafeArrayUnaccessData(safe_array);
        document->write(safe_array);
        // SysFreeString(bstr); // -> SafeArrayDestroy should be enough
        SafeArrayDestroy(safe_array);
      }
      document->Release();
    }
  } else {
    MessageBox(NULL, "Can't write to closed document", "IECtrl", MB_ICONERROR);
  }
  m_bGetFocus = false;
}

void IECtrl::write(const char *text) {
  wchar_t *wcsTemp = _com_util::ConvertStringToBSTR(text);
  write(wcsTemp);
  SysFreeString(wcsTemp);
}

void IECtrl::close() {
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    if (!m_bClosed) {
      document->close();
    }
    m_bClosed = true;
    document->Release();
  }
  m_bGetFocus = false;
}

void IECtrl::navigate(const char *url) {
  int textLen = strlen(url) + 1;
  WCHAR *tTemp = new WCHAR[textLen];

  MultiByteToWideChar(CP_ACP, 0, url, -1, tTemp, textLen);
  m_pWebBrowser->Navigate(tTemp, NULL, NULL, NULL, NULL);
  delete tTemp;
}

void IECtrl::clear() {
  IHTMLDocument2 *document = getDocument();
  if (document == NULL) {
    m_pWebBrowser->Navigate(L"about:blank", NULL, NULL, NULL, NULL);
    HRESULT hr = S_OK;
    IHTMLDocument2 *document = NULL;
    while (document == NULL && hr == S_OK) {
      Sleep(0);
      IDispatch *dispatch = NULL;
      if (SUCCEEDED(m_pWebBrowser->get_Document(&dispatch)) && dispatch != NULL) {
        hr = dispatch->QueryInterface(IID_IHTMLDocument2, (void **)&document);
        dispatch->Release();
      }
    }
    if (document != NULL) 
      document->Release();
  } else {
    if (!m_bClosed) {
      document->close();
    }
    VARIANT open_name;
    VARIANT open_features;
    VARIANT open_replace;
    IDispatch * open_window  = NULL;
    VariantInit(&open_name);
    open_name.vt = VT_BSTR;
    open_name.bstrVal = SysAllocString(L"_self");
    VariantInit(&open_features);
    VariantInit(&open_replace);

    HRESULT hr = document->open(SysAllocString(L"text/html"), open_name, open_features, open_replace, &open_window);
    if (hr == S_OK) {
      m_pWebBrowser->Refresh();
    }
    if (open_window != NULL) {
      open_window->Release();
    }
    document->Release();
  }
  m_bGetFocus = false;
  m_bClosed = false;
}

void IECtrl::enableSandbox(bool bSandbox) {
  m_bSandbox = bSandbox;
  if (m_pWebBrowser) m_pWebBrowser->put_Offline(m_bSandbox ? VARIANT_TRUE : VARIANT_FALSE);
}

char * IECtrl::getSelection(bool gettext) {
  if (m_szSelectedText != NULL) {
    delete m_szSelectedText;
  }
  BSTR selectedTextW = getSelectionFunc(gettext);
  if (selectedTextW == NULL || wcslen(selectedTextW) == 0) {
    return NULL;
  }
  int len = wcslen(selectedTextW)+1;
  char *str = new char[len];

  WideCharToMultiByte(CP_ACP, 0, selectedTextW, len, str, len, NULL, FALSE);
  delete selectedTextW;
  m_szSelectedText = str;

  return m_szSelectedText;
}

IECtrl* IECtrl::get(HWND hwnd) {
  IECtrl *ptr;
  if (m_pList == NULL) {
    return NULL;
  }
  EnterCriticalSection(&m_mutex);
  for (ptr = m_pList; ptr != NULL; ptr = ptr->m_pNext) {
    if (ptr->m_hWnd == hwnd) break;
  }
  LeaveCriticalSection(&m_mutex);
  return ptr;
}

HWND IECtrl::getHWND() {
  return m_hWnd;
}

int IECtrl::getCntId() {
  return m_cntId;
}

void IECtrl::translateAccelerator(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  char chr = (char)LOWORD(MapVirtualKey(wParam, 2));
  if (m_pKeyDownListener != NULL && m_pKeyDownListener->KeyDown(wParam, lParam)
    || m_pKeyDownListener == NULL && GetKeyState(VK_CONTROL) < 0 && (chr == 'C' || chr == 'c' || chr == 'A' || chr == 'a'))
  {
    IOleInPlaceActiveObject* pIOIPAO;
    if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&pIOIPAO))) {
      MSG msg;
      msg.message = uMsg;
      msg.wParam = wParam;
      msg.lParam = lParam;
      pIOIPAO->TranslateAccelerator(&msg);
      pIOIPAO->Release();
    }
  }
}

BSTR IECtrl::getSelectionFunc(bool gettext) {
  BSTR text = NULL;
  IHTMLDocument2 *document = getDocument();

  if (document != NULL) {
    IHTMLSelectionObject *pSelection = NULL;
    if (SUCCEEDED(document->get_selection(&pSelection)) && pSelection != NULL) {
      IDispatch *pDisp = NULL;
      if (SUCCEEDED(pSelection->createRange(&pDisp)) &&  pDisp != NULL) {
        IHTMLTxtRange *pRange = NULL;
        if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLTxtRange, (void**)&pRange))) {
          if (gettext) {
            if (SUCCEEDED(pRange->get_text(&text))) {
              if (text != NULL) {
                wchar_t *newtext = new wchar_t[wcslen(text)+1];
                wcscpy(newtext, text);
                text = newtext;
              }
            }
          } else {
            if (SUCCEEDED(pRange->get_htmlText(&text))) {
              if (text != NULL) {
                wchar_t *newtext = new wchar_t[wcslen(text)+1];
                wcscpy(newtext, text);
                text = newtext;
              }
            }
          }
          pRange->Release();
        }
        pDisp->Release();
      }
      pSelection->Release();
    }
    document->Release();
  }
  return text;
}

BSTR IECtrl::getHrefFromAnchor(IHTMLElement *element) {
  if (!element) return NULL;

  IHTMLAnchorElement* pAnchor;
  if (SUCCEEDED(element->QueryInterface(IID_IHTMLAnchorElement, (void**)&pAnchor)) && (pAnchor!=NULL)) {
    VARIANT  variant;
    BSTR url;
    if (SUCCEEDED(element->getAttribute(L"href", 2, &variant) && variant.vt == VT_BSTR)) {
      wchar_t *newtext = new wchar_t[wcslen(variant.bstrVal)+1];
      wcscpy(newtext, variant.bstrVal);
      url = newtext;
      SysFreeString(variant.bstrVal);
    }
    pAnchor->Release();
    return url;
  } else {
    IHTMLElement * parent;
    if (SUCCEEDED(element->get_parentElement(&parent)) && (parent != NULL)) {
      BSTR url = getHrefFromAnchor(parent);
      parent->Release();
      return url;
    }
  }
  return NULL;
}

bool IECtrl::mouseClick(POINT pt) {
  bool result = false;
  if (GetFocus() != m_hWnd) {
    m_bGetFocus = true;
  }

  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    IHTMLElement *element;
    IMLOG("%i, %i", pt.x, pt.y);
    if (SUCCEEDED(document->elementFromPoint(pt.x, pt.y, &element)) && element != NULL) {
      BSTR url = getHrefFromAnchor(element);
      if (url != NULL) {
        IMLOG("bla3");
        /*
        if ((GetKeyState(VK_SHIFT) & 0x8000) && !(GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000)) 
          SendMessage(GetParent(m_hWnd), WM_COMMAND, IDCANCEL, 0);
        */
        int i = wcslen(url);
        char *tTemp = new char[i + 1];
        WideCharToMultiByte(CP_ACP, 0, url, -1, tTemp, i + 1, NULL, NULL);
        if (m_pAnchorClickListener != NULL) {
          m_pAnchorClickListener->AnchorClicked(tTemp, this);
        }
        delete tTemp;
        delete url;
        result = true;
      }
      element->Release();
    }
    document->Release();
  }
  return result;
}

bool IECtrl::setFocus(HWND prevFocus) {
  if (GetFocus() != m_hWnd && !m_bGetFocus) {
    SetFocus(prevFocus);
    return true;
  }
  m_bGetFocus = false;
  return false;
}

void IECtrl::saveDocument() {
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    BSTR bCmd = SysAllocString(L"SaveAs");
    VARIANT_BOOL vb;
    VARIANT vValue;
    vValue.vt = VT_BOOL;
    vValue.boolVal = TRUE;
    document->execCommand(bCmd, VARIANT_FALSE, vValue, &vb);
    SysFreeString(bCmd);
    document->Release();
  }
}

bool IECtrl::callJScript(const char* szFunc, Var &args, Var *ret) {
  bool bRet = false;
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    IDispatch * dispatch;
    if (!document->get_Script(&dispatch) && dispatch != NULL) {
      DISPID dispid = NULL;
      LPOLESTR str = _com_util::ConvertStringToBSTR(szFunc);
      if (!dispatch->GetIDsOfNames(IID_NULL, &str, 1, LOCALE_SYSTEM_DEFAULT, &dispid)) {
        DISPPARAMS dispparams;
        dispparams.cArgs = args.length();
        dispparams.rgvarg = new VARIANT[dispparams.cArgs];
        dispparams.cNamedArgs = 0;
        dispparams.rgdispidNamedArgs = NULL;
        for (UINT i = 0; i < dispparams.cArgs; i++) {
          args[i].getVariant(&dispparams.rgvarg[dispparams.cArgs-i-1]);
        }
        EXCEPINFO excepInfo; memset(&excepInfo, 0, sizeof(excepInfo));
        UINT nArgErr = (UINT)-1;  // initialize to invalid arg
        VARIANT vRet;
        VariantInit(&vRet);
        if (!dispatch->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &dispparams, &vRet, &excepInfo, &nArgErr)) {
          bRet = true;
        }
        for (UINT i = 0; i < dispparams.cArgs; i++) {
          VariantClear(&dispparams.rgvarg[i]);
        }
        delete [] dispparams.rgvarg;
        if (ret != NULL) {
          ret->setValue(vRet);
        }
        VariantClear(&vRet);
      }
      SysFreeString(str);
      dispatch->Release();
    }
    document->Release();
  }
  return bRet;
}

void IECtrl::selectAll() {
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    VARIANT_BOOL b = FALSE;
    _variant_t v;
    document->execCommand(_bstr_t("SelectAll"), b, v, &b);
    document->Release();
  }
}

DWORD IECtrl::connectEvents(IHTMLElement* pElem, IDispatch* pDisp) {
  IConnectionPointContainer* pCPC = NULL;
  IConnectionPoint* pCP = NULL;
  DWORD dwCookie;

  if (SUCCEEDED(pElem->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC))) {
    if (SUCCEEDED(pCPC->FindConnectionPoint(DIID_HTMLElementEvents2, &pCP))) {
      if (FAILED(pCP->Advise((IUnknown*)pDisp, &dwCookie))) {
        MessageBox(NULL, "Events advise failed", "IECtrl", MB_ICONERROR);
      }
      pCP->Release();
    }
    pCPC->Release();
  }
  return dwCookie;
}

void IECtrl::disconnectEvents(IHTMLElement* pElem, DWORD dwCookie) {
  IConnectionPointContainer* pCPC = NULL;
  IConnectionPoint* pCP = NULL;

  if (SUCCEEDED(pElem->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC))) {
    if (SUCCEEDED(pCPC->FindConnectionPoint(DIID_HTMLElementEvents2, &pCP))) {
      if (FAILED(pCP->Unadvise(dwCookie))) {
        MessageBox(NULL, "Events unadvise failed", "IECtrl", MB_ICONERROR);
      }
      pCP->Release();
    }
    pCPC->Release();
  }
} 

/*
 * EventSink
 */

IECtrl::EventSink::EventSink(IECtrl * pCtrl) {
  m_pCtrl = pCtrl;
  m_cRef = 0;
}

IECtrl::EventSink::~EventSink() {
  _ASSERT(m_cRef == 0);
}

STDMETHODIMP IECtrl::EventSink::QueryInterface(REFIID riid, PVOID *ppv) {
  *ppv = NULL;
  if (IID_IUnknown == riid) {
    *ppv = (IUnknown*) this;
  }
  if (IID_IDispatch == riid) {
    *ppv = (IDispatch*) this;
  }
  if (DIID_DWebBrowserEvents2 == riid) {
    *ppv = (DWebBrowserEvents2*) this;
  }
  if (NULL != *ppv) {
    ((LPUNKNOWN)*ppv)->AddRef();
    return NOERROR;
  }
  return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) IECtrl::EventSink::AddRef(void) {
  ++m_cRef;
  return m_cRef;
}

STDMETHODIMP_(ULONG) IECtrl::EventSink::Release(void) {
  --m_cRef;
  return m_cRef;
}

// IDispatch
STDMETHODIMP IECtrl::EventSink::GetTypeInfoCount(UINT *ptr) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::EventSink::GetTypeInfo(UINT iTInfo, LCID lcid, LPTYPEINFO* ppTInfo) { 
  return S_OK; 
}

STDMETHODIMP IECtrl::EventSink::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) { 
  return S_OK; 
}

STDMETHODIMP IECtrl::EventSink::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid , WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO*pExcepInfo, UINT*puArgErr) 
{
  if (!pDispParams) return E_INVALIDARG;

  switch (dispIdMember) {
    case DISPID_BEFORENAVIGATE2:
      BeforeNavigate2(pDispParams->rgvarg[6].pdispVal,
                      pDispParams->rgvarg[5].pvarVal,
                      pDispParams->rgvarg[4].pvarVal,
                      pDispParams->rgvarg[3].pvarVal,
                      pDispParams->rgvarg[2].pvarVal,
                      pDispParams->rgvarg[1].pvarVal,
                      pDispParams->rgvarg[0].pboolVal);
      return S_OK;
  }
  return DISP_E_MEMBERNOTFOUND;
}

// DWebBrowserEvents2
void IECtrl::EventSink::StatusTextChange(BSTR text) {
}

void IECtrl::EventSink::ProgressChange(long progress, long progressMax) {
}

void IECtrl::EventSink::CommandStateChange(long command, VARIANT_BOOL enable) {
}

void IECtrl::EventSink::DownloadBegin() {
}

void IECtrl::EventSink::DownloadComplete() {
}

void IECtrl::EventSink::TitleChange(BSTR text) {
}

void IECtrl::EventSink::PropertyChange(BSTR text) {
}

void IECtrl::EventSink::BeforeNavigate2(IDispatch* pDisp, VARIANT* url, VARIANT* flags, VARIANT* targetFrameName, VARIANT* postData, VARIANT* headers, VARIANT_BOOL* cancel) 
{
  int i = wcslen(url->bstrVal);
  char* tTemp = new char[i+1];
  WideCharToMultiByte(CP_ACP, 0, url->bstrVal, -1, tTemp, i+1, NULL, NULL);

  if (strcmp(tTemp, "about:blank")) {
    // sytuacja bezpieczenstwa gdy mouseClick nie przechwyci odnosnika
    if (m_pCtrl->m_pAnchorClickListener != NULL) {
      m_pCtrl->m_pAnchorClickListener->AnchorClicked(tTemp, m_pCtrl);
    }
    *cancel = VARIANT_TRUE;
  }
  delete tTemp;
}

void IECtrl::EventSink::NewWindow2(IDispatch** ppDisp, VARIANT_BOOL* cancel) {
}

void IECtrl::EventSink::NavigateComplete(IDispatch* pDisp, VARIANT* url) {
}

void IECtrl::EventSink::DocumentComplete(IDispatch* pDisp, VARIANT* url) {
}

void IECtrl::EventSink::OnQuit() {
}

void IECtrl::EventSink::OnVisible(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::OnToolBar(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::OnMenuBar(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::OnStatusBar(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::OnFullScreen(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::OnTheaterMode(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::WindowSetResizable(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::WindowSetLeft(long val) {
}

void IECtrl::EventSink::WindowSetTop(long val) {
}

void IECtrl::EventSink::WindowSetWidth(long val) {
}

void IECtrl::EventSink::WindowSetHeight(long val) {
}

void IECtrl::EventSink::WindowClosing(VARIANT_BOOL isChildWindow, VARIANT_BOOL* cancel) {
}

void IECtrl::EventSink::ClientToHostWindow(long *cx, long *cy) {
}

void IECtrl::EventSink::SetSecureLockIcon(long val) {
}

void IECtrl::EventSink::FileDownload(VARIANT_BOOL* cancel) {
}

/*
 * ClientSite
 */

IECtrl::ClientSite::ClientSite(IECtrl * pCtrl) {
  m_pCtrl = pCtrl;
  m_cRef = 0;
}

IECtrl::ClientSite::~ClientSite() {
  _ASSERT(m_cRef == 0);
}

// IUnknown
STDMETHODIMP IECtrl::ClientSite::QueryInterface(REFIID riid, PVOID *ppv) {
  *ppv = NULL;
  if (IID_IUnknown == riid) {
    *ppv = this;
  }
  if (IID_IOleClientSite == riid) {
    *ppv = (IOleClientSite*) this;
  }
  if (IID_IOleWindow == riid || IID_IOleInPlaceSite == riid) {
    *ppv = (IOleInPlaceSite*) this;
  }
  if (IID_IDocHostUIHandler == riid) {
    *ppv = (IDocHostUIHandler*) this;
  }
  /* if (IID_IDispatch == riid) {
    *ppv = (IDispatch*) this;
  } */
  if (IID_IDocHostShowUI == riid) {
    *ppv = (IDocHostShowUI*) this;
  }
  if (IID_IServiceProvider == riid) {
    *ppv = (IServiceProvider*) this;
  }
  if (IID_IInternetSecurityManager == riid) {
    *ppv = (IInternetSecurityManager*) this;
  }
  if (NULL != *ppv) {
    ((LPUNKNOWN)*ppv)->AddRef();
    return NOERROR;
  }
  return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) IECtrl::ClientSite::AddRef(void) {
  ++m_cRef;
  return m_cRef;
}

STDMETHODIMP_(ULONG) IECtrl::ClientSite::Release(void) {
  --m_cRef;
  return m_cRef;
}

// IOleWindow
STDMETHODIMP IECtrl::ClientSite::GetWindow(HWND *phwnd) {
  *phwnd = m_pCtrl->m_hParentWnd;
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::ContextSensitiveHelp(BOOL fEnterMode) {
  return E_NOTIMPL;
}

// IOleInPlace
STDMETHODIMP IECtrl::ClientSite::CanInPlaceActivate(void) {
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::OnInPlaceActivate(void) {
  m_pCtrl->m_bInPlaceActive = TRUE;
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::OnUIActivate(void) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo) 
{
  lprcPosRect->left = m_pCtrl->m_rcClient.left;
  lprcPosRect->top = m_pCtrl->m_rcClient.top;
  lprcPosRect->right = m_pCtrl->m_rcClient.right;
  lprcPosRect->bottom = m_pCtrl->m_rcClient.bottom;
  lprcClipRect->left = m_pCtrl->m_rcClient.left;
  lprcClipRect->top = m_pCtrl->m_rcClient.top;
  lprcClipRect->right = m_pCtrl->m_rcClient.right;
  lprcClipRect->bottom = m_pCtrl->m_rcClient.bottom;

  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::Scroll(SIZE scrollExtant) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::OnUIDeactivate(BOOL fUndoable) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::OnInPlaceDeactivate(void) {
  m_pCtrl->m_bInPlaceActive = FALSE;
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::DiscardUndoState(void) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::DeactivateAndUndo(void) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::OnPosRectChange(LPCRECT lprcPosRect) {
  return E_NOTIMPL;
}

// IOleClientSite
STDMETHODIMP IECtrl::ClientSite::SaveObject(void) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::GetContainer(IOleContainer **ppContainer) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::ShowObject(void) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::OnShowWindow(BOOL fShow) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::RequestNewObjectLayout(void) {
  return E_NOTIMPL;
}

// IDocHostUIHandler
STDMETHODIMP IECtrl::ClientSite::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdTarget, IDispatch *pdispElement) {
  IOleCommandTarget * pOleCommandTarget;
  IOleWindow * pOleWindow;
  HWND hSPWnd;
  
  if (SUCCEEDED(pcmdTarget->QueryInterface(IID_IOleCommandTarget, (void**)&pOleCommandTarget))) {
    if (SUCCEEDED(pOleCommandTarget->QueryInterface(IID_IOleWindow, (void**)&pOleWindow))) {
      pOleWindow->GetWindow(&hSPWnd);

      if (m_pCtrl->m_pPopupMenuListener != NULL) {
        PopupMenuListener::MenuType type = PopupMenuListener::MenuType::Unknown;
        if (dwID == 5) {
          type = PopupMenuListener::MenuType::Anchor;
        } else if (dwID == 4) {
          type = PopupMenuListener::MenuType::Selection;
        } else if (dwID == 1) {
          type = PopupMenuListener::MenuType::Image;
        }

        PopupMenuListener::MakeAction action = m_pCtrl->m_pPopupMenuListener->PopupMenu(type, *ppt, m_pCtrl);
        if (action == PopupMenuListener::MakeAction::OpenLink) {
          // Dalej ppt nie jest u¿ywane - nie robiê kopii przed konwersj¹. Jakby co to tu szukaæ b³êdu.
          ScreenToClient(hSPWnd, ppt);
          m_pCtrl->mouseClick(*ppt);
        } else if (action == PopupMenuListener::MakeAction::CopyLink) {
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)2262, (LPARAM) NULL);
        } else if (action == PopupMenuListener::MakeAction::CopySelection) {
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)15, (LPARAM) NULL);
        } else if (action == PopupMenuListener::MakeAction::Print) {
          // SendMessage(hSPWnd, WM_COMMAND, (WPARAM)27, (LPARAM) NULL); // print...
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)2003, (LPARAM) NULL); // print preview...
        } else if (action == PopupMenuListener::MakeAction::SaveImage) {
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)2270, (LPARAM) NULL);
        } else if (action == PopupMenuListener::MakeAction::SelectAll) {
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)31, (LPARAM) NULL);
        } else if (action == PopupMenuListener::MakeAction::ShowSource) {
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)2139, (LPARAM) NULL);
        }
      }

      /*
      if (dwID == 5) { // anchor 
        IHTMLElement *element;
        if (SUCCEEDED(pdispElement->QueryInterface(IID_IHTMLElement,(void**)&element)) && element != NULL) {
          BSTR url = m_pCtrl->getHrefFromAnchor(element);
          if (url != NULL) {
            int i = wcslen(url);
            char *tTemp = new char[i+1];
            WideCharToMultiByte(CP_ACP, 0, url, -1, tTemp, i+1, NULL, NULL);
            MessageBox(NULL, tTemp, "Anchor PopupMenu ", MB_OK); 
            delete tTemp;
            delete url;
          }
          element->Release();
        }
      }
      */
      pOleWindow->Release();
    }
    pOleCommandTarget->Release();
  }
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::GetHostInfo(DOCHOSTUIINFO *pInfo) {
  pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER; // | DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE;
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc) 
{
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::HideUI(void) {
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::UpdateUI(void) {
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::EnableModeless(BOOL fEnable) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::ClientSite::OnDocWindowActivate(BOOL fEnable) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::ClientSite::OnFrameWindowActivate(BOOL fEnable) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::ClientSite::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID) { 
  return S_FALSE;
}

STDMETHODIMP IECtrl::ClientSite::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::ClientSite::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget) { 
  *ppDropTarget = m_pCtrl->m_pDropTarget;
  m_pCtrl->m_pDropTarget->AddRef();
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::GetExternal(IDispatch **ppDispatch) {
  *ppDispatch = m_pCtrl->m_pExternal;
  m_pCtrl->m_pExternal->AddRef();
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::ClientSite::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet) { 
  return E_NOTIMPL; 
}

// IDocHostShowUI

HRESULT IECtrl::ClientSite::ShowHelp(HWND hwnd, LPOLESTR pszHelpFile, UINT nCommand, DWORD dwData, POINT ptMouse, IDispatch * pDispatchObjectHit) {
  return S_OK;
}

HRESULT IECtrl::ClientSite::ShowMessage(HWND hWnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT * plResult)
{
  const char * text = _com_util::ConvertBSTRToString(lpstrText);
  if (m_pCtrl->m_pScriptMessageListener != NULL) {
    *plResult = m_pCtrl->m_pScriptMessageListener->ShowMessage(hWnd, text, dwType);
  } else {
    *plResult = MessageBox(hWnd, text, "JScript Message", dwType);
  }
  delete [] text;
  return S_OK;
}

// IServiceProvider
STDMETHODIMP IECtrl::ClientSite::QueryService(REFGUID guidService, REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) {
  if (guidService == SID_SInternetSecurityManager && m_pCtrl->m_bSandbox) {
    if (riid == IID_IInternetSecurityManager) {
      return this->QueryInterface(riid, ppvObject);
    } else {
      return E_NOINTERFACE;
    }
  }
  return E_UNEXPECTED;
}

// IInternetSecurityManager
STDMETHODIMP IECtrl::ClientSite::SetSecuritySite(IInternetSecurityMgrSite *pSite) {
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::GetSecuritySite(IInternetSecurityMgrSite **ppSite) {
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags) {
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::GetSecurityId(LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved) {
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::ProcessUrlAction(LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved)
{
  if (cbPolicy != 4) return INET_E_DEFAULT_ACTION;
  PBOOL pBool = (PBOOL)pPolicy;

  if (dwAction == URLACTION_SCRIPT_RUN) {
    *pBool = URLPOLICY_DISALLOW; // URLPOLICY_ALLOW;
  } else if (dwAction == URLACTION_JAVA_PERMISSIONS) {
    *pBool = URLPOLICY_JAVA_PROHIBIT;
  } else {
    *pBool = URLPOLICY_DISALLOW;
  }
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::QueryCustomPolicy(LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::SetZoneMapping( DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags) {
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::GetZoneMappings(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags) {
  return INET_E_DEFAULT_ACTION;
}

/*
 * External
 */

IECtrl::External::External(IECtrl * pCtrl) {
  m_pCtrl = pCtrl;
  m_cRef = 0;
}

IECtrl::External::~External() {
  _ASSERT(m_cRef == 0);
}

// IUnknown
STDMETHODIMP IECtrl::External::QueryInterface(REFIID riid, PVOID *ppv) {
  *ppv = NULL;
  if (IID_IUnknown == riid) {
    *ppv = this;
  }
  if (IID_IDispatch == riid) {
    *ppv = (IDispatch*) this;
  }
  if (NULL != *ppv) {
    ((LPUNKNOWN)*ppv)->AddRef();
    return NOERROR;
  }
  return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) IECtrl::External::AddRef(void) {
  ++m_cRef;
  return m_cRef;
}

STDMETHODIMP_(ULONG) IECtrl::External::Release(void) {
  --m_cRef;
  return m_cRef;
}

// IDispatch
STDMETHODIMP IECtrl::External::GetTypeInfoCount(UINT *ptr) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::External::GetTypeInfo(UINT iTInfo, LCID lcid, LPTYPEINFO* ppTInfo) { 
  return S_OK; 
}

STDMETHODIMP IECtrl::External::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) { 
  HRESULT hr;
  UINT  i;
  hr = NOERROR;

  for (i = 0; i < cNames; i++) {
    const char * szName = _com_util::ConvertBSTRToString(rgszNames[i]);
    long id = 0;
    if (m_pCtrl->m_pExternalListener != NULL) {
      id = m_pCtrl->m_pExternalListener->GetMemberID(szName);
    }
    delete [] szName;
    if (id > 0) {
      rgDispId[i] = id;
    } else {
      hr = ResultFromScode(DISP_E_UNKNOWNNAME);
      rgDispId[i] = DISPID_UNKNOWN;
    }
  }
  return hr;
}

STDMETHODIMP IECtrl::External::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) 
{
  if (!pDispParams) return E_INVALIDARG;

  if (m_pCtrl->m_pExternalListener != NULL) {
    Var args;
    for (UINT i = 0; i < pDispParams->cArgs; i++) {
      args[-1] = pDispParams->rgvarg[pDispParams->cArgs-i-1];
    }
    Var ret = m_pCtrl->m_pExternalListener->Trigger(dispIdMember, args);
    ret.getVariant(pVarResult);

    return S_OK;
  }
  return DISP_E_MEMBERNOTFOUND;
}

/*
 * DropTarget
 */

IECtrl::DropTarget::DropTarget(IECtrl * pCtrl) {
  m_pCtrl = pCtrl;
  m_cRef = 0;
}

IECtrl::DropTarget::~DropTarget() {
  _ASSERT(m_cRef == 0);
}

// IUnknown
STDMETHODIMP IECtrl::DropTarget::QueryInterface(REFIID riid, PVOID *ppv) {
  *ppv = NULL;
  if (IID_IUnknown == riid) {
    *ppv = this;
  }
  if (IID_IDropTarget == riid) {
    *ppv = (IDropTarget*) this;
  }
  if (NULL != *ppv) {
    ((LPUNKNOWN)*ppv)->AddRef();
    return NOERROR;
  }
  return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) IECtrl::DropTarget::AddRef(void) {
  ++m_cRef;
  return m_cRef;
}

STDMETHODIMP_(ULONG) IECtrl::DropTarget::Release(void) {
  --m_cRef;
  return m_cRef;
}

// IDropTarget
STDMETHODIMP IECtrl::DropTarget::DragEnter(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
  m_fAllowDrop = (QueryDataObject(pDataObject) && (m_pCtrl->m_pDropListener != NULL));

  if (m_fAllowDrop) {
    *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
    SetFocus(m_pCtrl->m_hWnd);
    // PositionCursor(m_hWnd, pt);
  } else {
    *pdwEffect = DROPEFFECT_NONE;
  }
  return S_OK;
}

STDMETHODIMP IECtrl::DropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
  if (m_fAllowDrop) {
    *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
    // PositionCursor(m_hWnd, pt);
  } else {
    *pdwEffect = DROPEFFECT_NONE;
  }
  return S_OK;
}

STDMETHODIMP IECtrl::DropTarget::DragLeave(void) {
  return S_OK;
}

STDMETHODIMP IECtrl::DropTarget::Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
  // PositionCursor(m_hWnd, pt);
  if (m_fAllowDrop) {
    DropData(pDataObject);
    *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
  } else {
    *pdwEffect = DROPEFFECT_NONE;
  }
  return S_OK;
}

bool IECtrl::DropTarget::QueryDataObject(IDataObject *pDataObject) {
  // tylko drop plików CF_HDROP i TYMED_HGLOBAL
  FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
  return pDataObject->QueryGetData(&fmtetc) == S_OK ? true : false;
}

DWORD IECtrl::DropTarget::DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed) {
  // tu mozna zrobic rozroznienie po roznych parametrach
  return DROPEFFECT_COPY;
}

void IECtrl::DropTarget::DropData(IDataObject *pDataObject) {
  FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
  STGMEDIUM medium;

  if (SUCCEEDED(pDataObject->GetData(&fmtetc, &medium))) {
    if (medium.tymed == TYMED_HGLOBAL && medium.hGlobal) {
      DROPFILES * df = (DROPFILES *)GlobalLock(medium.hGlobal);
      SIZE_T lBufferSize = GlobalSize(medium.hGlobal);

      char * file = NULL;
      if (df->fWide) { 
        // pobieramy tylko pierwszy plik, reszta nas nie interesuje
        wchar_t * str = (wchar_t*)((BYTE*)df+df->pFiles);
        int len = wcslen(str)+1;
        file = new char[len];
        WideCharToMultiByte(CP_ACP, 0, str, len, file, len, NULL, FALSE);
      } else {
        file = new char[lBufferSize - df->pFiles];
        CopyMemory(file, (BYTE*)df + df->pFiles, lBufferSize - df->pFiles);
      }

      if (m_pCtrl->m_pDropListener != NULL) {
        m_pCtrl->m_pDropListener->FileDropped(file, m_pCtrl);
      }

      delete [] file;
      GlobalUnlock(medium.hGlobal);
    }
    ReleaseStgMedium(&medium);
  }
}