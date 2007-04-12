#pragma once

#ifndef __TEMPLATE_FACTORY_H__
#define __TEMPLATE_FACTORY_H__

class iTemplateToken;

class TemplateTokenFactory : public SharedObject<iSharedObject> {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(TemplateTokenFactory, iSharedObject, Version(1,0,0,0));

public:
  typedef iTemplateToken* (__stdcall fCallback*)(const string&);
  typedef map<String, fCallback*> tCallbacks;

private:
  TemplateTokenFactory() { }
  ~TemplateTokenFactory() { }

public:
  static TemplateTokenFactory* get() {
    if (!instance.isValid()) {
      instance = new TemplateTokenFactory;
    }
    return instance;
  }

public:
  iTemplateToken* getInstance(const string& name);
  bool isHandled(const string& name);

  bool bind(const string& name, fCallback* callback);
  bool unbind(const string& name);

protected:
  void clear();

protected:
  tCallbacks _callbacks;

private:
  static SharedPtr<TemplateTokenFactory> instance;
};

#endif //__TEMPLATE_FACTORY_H__