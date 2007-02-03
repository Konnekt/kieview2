/**
  *  kIEview2 Emot parsers
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @copyright    Copyright (c) 2006-2007 Micha³ "Dulek" Dulko
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 19 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-15 17:31:48 +0100 (Pt, 15 gru 2006) $
  */

#include "stdafx.h"
#include "EmotHandler.h"
#include "Helpers.h"

EmotSet JispParser::parse(const string& filePath, const string& fileDir) {
  ZIPENTRY entry;
  OFSTRUCT ofs;
  int index;

  HZIP file = OpenZip((void*) OpenFile(filePath.c_str(), &ofs, OF_READ), 0, ZIP_HANDLE);
  if (!(FindZipItem(file, (fileDir + "/icondef.xml").c_str(), true, &index, &entry) == ZR_OK)) {
    throw CannotOpen("Cannot open file " + filePath);
  }

  String code;
  char* buff = new char[entry.unc_size];
  UnzipItem(file, index, buff, entry.unc_size, ZIP_MEMORY);
  code = buff;
  delete buff;

  xmlpp::DomParser parser;
  parser.set_substitute_entities();

  xmlpp::Node* rootNode;
  xmlpp::Node* metaNode;
  xmlpp::Node::NodeList icons;
  xmlpp::Node::NodeList nodes;
  EmotSet result;

  try {
    parser.parse_memory(code.c_str());
  } catch (xmlpp::exception ex) {
    throw XMLParserException(ex.what());
  }

  rootNode = parser.get_document()->get_root_node();
  if (rootNode->get_name() != "icondef") throw WrongFormat("Z³a nazwa korzenia dokumentu (z³y format ?)");

  nodes = rootNode->get_children("meta");
  if (nodes.size() != 1) throw WrongFormat("Nie ma dok³adnie jednego elementu 'meta'");
  metaNode = *nodes.begin();
  if (!metaNode) throw WrongFormat("Element 'meta' nie zawiera dzieci");

  nodes = metaNode->get_children("name");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.name = dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content().c_str();
  }

  nodes = metaNode->get_children("version");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.version = dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content().c_str();
  }

  nodes = metaNode->get_children("description");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.description = dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content().c_str();
  }

  nodes = metaNode->get_children("author");
  for (xmlpp::Node::NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
    if (dynamic_cast<xmlpp::Element*>(*it)) {
      result.authors.push_back(EmotAuthor(dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content().c_str(),
        dynamic_cast<xmlpp::Element*>(*it)->get_attribute("jid") ? dynamic_cast<xmlpp::Element*>(*it)->get_attribute("jid")->get_value().c_str() : ""));
    }
  }

  nodes = metaNode->get_children("creation");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.creationTime = dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content().c_str();
  }

  nodes = metaNode->get_children("home");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.url = dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content().c_str();
  }
  
  icons = rootNode->get_children("icon");
  for (xmlpp::Node::NodeList::iterator it = icons.begin(); it != icons.end(); it++) {
    string mime;
    Emot emot;

    nodes = (*it)->get_children("object");
    for (xmlpp::Node::NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
      if (dynamic_cast<xmlpp::Element*>(*it) && dynamic_cast<xmlpp::Element*>(*it)->get_attribute("mime")) {
        mime = dynamic_cast<xmlpp::Element*>(*it)->get_attribute("mime")->get_value();
        if (mime == "image/png" || mime == "image/gif" || mime == "image/jpeg") {
          emot.menu_img_path = emot.img_path = dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content();
          break;
        }
        mime.clear();
      }
    }

    if (mime.empty()) continue;

    nodes = (*it)->get_children("text");
    if(nodes.empty()) throw WrongFormat("Brak tekstu do zamiany");
    for (xmlpp::Node::NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
      if (dynamic_cast<xmlpp::Element*>(*it)) {
        emot.text = dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content().c_str();
        result.emots.push_back(emot);
      }
    }
  }
  result.dir = fileDir;
  return result;
}

EmotSet GGEmotParser::parse(const string& filePath, const string& fileDir) {
  ifstream file(filePath.c_str());
  string code, buff;

  if (!file.is_open()) {
    throw CannotOpen("Cannot open file " + filePath);
  }

  while (!file.eof()) {
    getline(file, buff);
    code += buff + "\n";
  }
  file.close();

  EmotSet result;
  tStringVector strings;
  bool inMenu;

  Stamina::split(code, "\n", strings);
  RegEx reg;

  for (tStringVector::iterator it = strings.begin(); it != strings.end(); it++) {
    EmotSet::tEmots emots;

    if ((*it)[0] == '*') {
      inMenu = false;
      (*it).erase(0);
    } else {
      inMenu = true;
    }

    reg.setSubject(*it);
    reg.setPattern("/,?\"(.+?)\",?/");

    while (reg.match_global()) {
      emots.push_back(Emot(reg[1].c_str(), "", "", reg[1][0] == '/', inMenu));

      if (reg.getSubject()[reg.getStart()] == ')' || (*it)[0] != '(') {
        string img_path;
        string menu_img_path;

        if (reg.match_global()) {
          img_path = reg[1];
          menu_img_path = reg.match_global() ? reg[1] : img_path;

          for (EmotSet::tEmots::iterator it = emots.begin(); it != emots.end(); it++) {
            it->img_path = img_path;
            it->menu_img_path = menu_img_path;
          }
        } else {
          throw WrongFormat("Brak œcie¿ki do obrazka");
        }
        break;
      }
    }
    result.emots.insert(result.emots.end(), emots.begin(), emots.end());
  }

  result.name = fileDir;
  result.dir = fileDir;

  return result;
}

string __stdcall EmotHandler::replaceEmot(RegEx* reg, void* param) {
  EmotHandler* handler = static_cast<EmotHandler*>(param);

  IMLOG("tag = %s, path = %s", reg->getSub(0).c_str(), reg->getSub(1).c_str());
  return "<img src=\"" + unifyPath(handler->getEmotDir() + "/" + reg->getSub(1)) + "\" class=\"emoticon\" alt=\"\" />";
}

String EmotHandler::parse(const StringRef& body, int net) {
  RegEx reg;
  reg.setSubject(body);

  for (tEmotSets::iterator it = emotSets.begin(); it != emotSets.end(); it++) {
    // if (emotSetsByNet.find(net) != emotSetsByNet.end() && emotSetsByNet[net].size()) 

    for (EmotSet::tEmots::iterator it2 = it->emots.begin(); it2 != it->emots.end(); it2++) {
      IMLOG("emot_set = %s, emot_text = %s, emot_preg = %i", it->name.c_str(), it2->text.c_str(), it2->preg);
      try {
        reg.setPattern(!it2->preg ? "/" + reg.addSlashes(it2->text) + "/i" : it2->text);
        reg.replaceItself(("<kiev2:emot path=\"" + it->dir + "/" + it2->img_path + "\" />").c_str());
      } catch (const RegEx::CompileException& e) {
        IMLOG("B³¹d definicji emotikony: %s, %i", e.error, e.pos);
      }
    }
  }
  reg.replaceItself("#<kiev2:emot path=\"([^\"]+)\" />#", &EmotHandler::replaceEmot, 0, (void*) this);
  return reg.getSubject();
}

void EmotHandler::loadPackages() {
  if (emotSets.size()) emotSets.clear();

  string emotDir = getEmotDir();
  Dir::tItems emotDirs;

  try {
    emotDirs = Dir::getDirs(emotDir + "\\*");
  } catch (const Exception& e) {
    IMLOG("Nie znaleziono katalogu z emotami (%s) !", e.getReason().a_str());
    return;
  }
  if (!emotDirs.size()) {
    IMLOG("Brak katalogów z pakietami");
    return;
  }

  for (Dir::tItems::iterator it = emotDirs.begin(); it != emotDirs.end(); it++) {
    for (tParsers::iterator it2 = parsers.begin(); it2 != parsers.end(); it2++) {

      string fileName = unifyPath(emotDir + "/" + it->cFileName + "/" + (*it2)->getDefFileName(it->cFileName));
      if (!fileExists(fileName.c_str())) continue;

      try {
        emotSets.push_back((*it2)->parse(fileName, it->cFileName));
      } catch (const EmotParserException& e) {
        IMLOG("dupsko podczas parsowania emotSeta: %s", e.getReason().a_str());
      }
      break;
    }
  }
}