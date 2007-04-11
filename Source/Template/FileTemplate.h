#pragma once

#ifndef __FILETEMPLATE_H__
#define __FILETEMPLATE_H__

#include <fstream>
#include "iTemplate.h"

class FileTemplate : public iTemplate {
public:
  FileTemplate(const String& path, iVarManager* vm = NULL) : iTemplate(vm), isLoaded(false) { }

public:
  bool load();
  bool reload();
  bool isLoaded();

private:
  String _path;
  bool _isLoaded;
};

#endif //__FILETEMPLATE_H__