#pragma once

#ifndef __FILETEMPLATE_H__
#define __FILETEMPLATE_H__

#include <fstream>
#include "iTemplate.h"

class FileTemplate : public iTemplate {
public:
  FileTemplate(const StringRef& path, iVarManager* vm = NULL) : iTemplate(vm), isLoaded(false) { }

public:
  bool load();
  bool reload();
  bool loaded();

private:
  String _path;
  bool _loaded;
};

#endif //__FILETEMPLATE_H__