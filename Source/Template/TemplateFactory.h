#pragma once

#ifndef __TEMPLATE_FACTORY_H__
#define __TEMPLATE_FACTORY_H__

class iTemplateToken;

class TemplateFactory : public SharedObject<iSharedObject> {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(TemplateFactory, iSharedObject, Version(1,0,0,0));

public:
  typedef iTemplateToken* (__stdcall fFactoryTokensCallback*)(const String&);

  typedef map<String, fFactoryTokensCallback*> tFactoryTokensCallbacks;

private:
  TemplateFactory() { }
  ~TemplateFactory() { }

public:
  static TemplateFactory* get() {
    if (!instance.isValid()) {
      instance = new TemplateFactory;
    }
    return instance;
  }

public:
  bool registerToken(const String& name, fFactoryTokensCallback* callback);
  bool unregisterToken(const String& name);
  bool isHandlingToken(const String& name);
  iTemplateToken* getInstanceOfToken(const String& name);

private:
  void clearTokens();

private:
  tFactoryCallbacks _callbacks;

private:
  static SharedPtr<TemplateFactory> instance;
};

#endif //__TEMPLATE_FACTORY_H__