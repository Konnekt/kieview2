/**
  *  kIEview2 Emot parsers
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @copyright    Copyright (c) 2006-2007 Micha� "Dulek" Dulko
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 19 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-15 17:31:48 +0100 (Pt, 15 gru 2006) $
  */

#include "stdafx.h"
#include "EmotHandler.h"
#include "Helpers.h"
#include "Controller.h"

eMSet JispParser::parse(const string& filePath, const string& fileDir) {
  String code;
  Zip zip;

  try {
    zip.open(filePath);
    code = zip.getFile(fileDir + "/icondef.xml");
  } catch(const Exception& e) {
    throw CannotOpen(e.getReason());
  }

  xmlpp::DomParser parser;
  parser.set_substitute_entities();

  xmlpp::Node* rootNode;
  xmlpp::Node* metaNode;
  xmlpp::Node::NodeList icons;
  xmlpp::Node::NodeList nodes;
  xmlpp::Attribute* attrib;
  eMSet result;

  try {
    parser.parse_memory(code.c_str());
  } catch (const xmlpp::exception& e) {
    throw XMLParserException(e.what());
  }

  rootNode = parser.get_document()->get_root_node();
  if (rootNode->get_name() != "icondef") throw WrongFormat("Z�a nazwa korzenia dokumentu (z�y format ?)");

  nodes = rootNode->get_children("meta");
  if (nodes.size() != 1) throw WrongFormat("Nie ma dok�adnie jednego elementu 'meta'");
  metaNode = *nodes.begin();
  if (!metaNode) throw WrongFormat("Element 'meta' nie zawiera dzieci");

  nodes = metaNode->get_children("name");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.setName(dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content().c_str());
  }

  nodes = metaNode->get_children("version");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.setVersion(dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content().c_str());
  }

  nodes = metaNode->get_children("description");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.setDescription(dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content().c_str());
  }

  nodes = metaNode->get_children("author");
  for (xmlpp::Node::NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
    if (dynamic_cast<xmlpp::Element*>(*it)) {
      attrib = dynamic_cast<xmlpp::Element*>(*it)->get_attribute("jid");
      result.addAuthor(eMAuthor(dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content().c_str(),
        attrib ? attrib->get_value().c_str() : ""));
    }
  }

  nodes = metaNode->get_children("creation");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    // result.setCTime(dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content().c_str());
  }

  nodes = metaNode->get_children("home");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.setUrl(dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content().c_str());
  }
  
  icons = rootNode->get_children("icon");
  for (xmlpp::Node::NodeList::iterator it = icons.begin(); it != icons.end(); it++) {
    eM emot(true, false, true);
    string mime;

    nodes = (*it)->get_children("object");
    for (xmlpp::Node::NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
      if (dynamic_cast<xmlpp::Element*>(*it) && dynamic_cast<xmlpp::Element*>(*it)->get_attribute("mime")) {
        mime = dynamic_cast<xmlpp::Element*>(*it)->get_attribute("mime")->get_value();
        if (mime == "image/png" || mime == "image/gif" || mime == "image/jpeg") {
          // emot.menu_img_path = emot.img_path = dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content();

          try {
            emot.setRawData(zip.getBinaryFile(fileDir + "/" + (string) dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content()));
          } catch(const Exception& e) {
            throw CannotOpen(e.getReason());
          }
          break;
        }
        mime.clear();
      }
    }

    if (mime.empty()) continue;

    nodes = (*it)->get_children("text");
    if (nodes.empty()) throw WrongFormat("Brak tekstu do zamiany");

    for (xmlpp::Node::NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
      if (dynamic_cast<xmlpp::Element*>(*it)) {
        emot.setText(dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content().c_str());
        attrib = dynamic_cast<xmlpp::Element*>(*it)->get_attribute("regexp");
        if (attrib) {
          emot.setPreg((bool) atoi(attrib->get_value().c_str()));
        }
        result.addEmot(emot);
      }
    }
  }

  result.setDir(fileDir);
  zip.close();
  return result;
}

eMSet GGParser::parse(const string& filePath, const string& fileDir) {
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

  eMSet result;
  tStringVector strings;
  bool inMenu;

  Stamina::split(code, "\n", strings);
  RegEx reg;

  for (tStringVector::iterator it = strings.begin(); it != strings.end(); it++) {
    eMSet::tEmots emots;

    if ((*it)[0] == '*') {
      inMenu = false;
      (*it).erase(0);
    } else {
      inMenu = true;
    }

    reg.setSubject(*it);
    reg.setPattern("/,?\"(.+?)\",?/");

    while (reg.match_global()) {
      eM emot(inMenu, reg[1][0] == '/');
      emot.setText(reg[1].c_str());
      emots.push_back(emot);

      if (reg.getSubject()[reg.getStart()] == ')' || (*it)[0] != '(') {
        string img_path;
        string menu_img_path;

        if (reg.match_global()) {
          img_path = reg[1];
          menu_img_path = reg.match_global() ? reg[1] : img_path;

          for (eMSet::tEmots::iterator it = emots.begin(); it != emots.end(); it++) {
            it->setMenuImgPath(menu_img_path);
            it->setImgPath(img_path);
          }
        } else {
          throw WrongFormat("Brak �cie�ki do obrazka");
        }
        break;
      }
    }
    result.getEmots().insert(result.getEmots().end(), emots.begin(), emots.end());
  }

  result.setName(fileDir);
  result.setDir(fileDir);

  return result;
}

void EmotHandler::loadSettings() {
  string data = Controller::getConfig()->getChar(cfg::emotPacks);
  data = trim(data, "\n");

  if (!data.length()) return;

  tStringVector sets;
  Stamina::split(data, "\n", sets);

  for (tStringVector::iterator it = sets.begin(); it != sets.end(); it++) {
    tStringVector rows;
    Stamina::split(*it, "|", rows);

    if (rows.size() != 3) continue;
    for (tEmotSets::iterator it2 = emotSets.begin(); it2 != emotSets.end(); it2++) {
      if (it2->getDir() == rows[2]) {
        it2->setEnabled(atoi(rows[1].c_str()));
        it2->setPos(atoi(rows[0].c_str()));
        break;
      }
    }
  }
  emotSets.sort();
}

void EmotHandler::saveSettings() {
  if (!emotSets.size()) return;
  String result;

  for (tEmotSets::iterator it = emotSets.begin(); it != emotSets.end(); it++) {
    result += stringf("%d|%d|%s\n", it->getPos(), (int) it->isEnabled(), it->getDir().c_str());
  }
  Controller::getConfig()->set(cfg::emotPacks, result);
}

void EmotHandler::fillLV(EmotLV* lv) {
  oImage img;

  for (tEmotSets::iterator it = emotSets.begin(); it != emotSets.end(); it++) {
    if (!it->getEmots()[0].isVirtual()) {
      img = loadImageFromFile((getEmotDir() + "\\" + it->getDir() + "\\" + it->getEmots()[0].getMenuImgPath()).c_str());
    } else {
      img = new Icon((HICON) Ctrl->ICMessage(IMI_ICONGET, kIEview2::ico::emots, IML_16), false);
    }
    lv->addItem(new EmotLV::sEmotPackInfo(it->isEnabled(), &*it, img));
  }
}

string EmotHandler::getKonnektPath() {
  return Controller::getInstance()->kPath;
}

string EmotHandler::getEmotDir() {
  string emotDir = Controller::getConfig()->getChar(kIEview2::cfg::emotsDir);

  if (emotDir.find(':') == emotDir.npos) {
    emotDir = getKonnektPath() + emotDir;
  }
  return emotDir;
}

String EmotHandler::prepareBody(const StringRef& body, bool encode, bool html) {
  RegEx reg;
  reg.setSubject(body);

  if (encode) {
    reg.replaceItself(html ? "/&amp;/" : "/&/", "\1");
    reg.replaceItself(html ? "/&lt;/" : "/</", "\2");
    reg.replaceItself(html ? "/&gt;/" : "/>/", "\3");
    reg.replaceItself(html ? "/&quot;/" : "/\"/", "\4");
    reg.replaceItself(html ? "/&(apos|#0?39);/" : "/'/", "\5");
  } else {
    reg.replaceItself("/\1/", html ? "&amp;" : "/&/");
    reg.replaceItself("/\2/", html ? "&lt;" : "/</");
    reg.replaceItself("/\3/", html ? "&gt;" : "/>/");
    reg.replaceItself("/\4/", html ? "&quot;" : "/\"/");
    reg.replaceItself("/\5/", html ? "&#039;" : "/'/");
  }
  return reg.getSubject();
}

string __stdcall EmotHandler::emotInsertion(RegEx* reg, void* param) {
  sEmotInsertion* ei = static_cast<sEmotInsertion*>(param);
  ei->match = reg->getSub(0).c_str();

  return "<kiev2:emot:insertion id=\"" + inttostr(ei->id) + "\" />";
}

string __stdcall EmotHandler::replaceEmot(RegEx* reg, void* param) {
  EmotHandler* handler = static_cast<EmotHandler*>(param);
  sEmotInsertion* ei = &handler->emotInsertions.at(atoi(reg->getSub(1).c_str()));

  IMLOG("[EmotHandler::replaceEmot()] match = %s, img_path = %s, emot = %s, set = %s", ei->match.c_str(), ei->emot->getImgPath().c_str(),
    ei->emot->getText().c_str(), ei->emotSet->getName().c_str());

  return "<img src=\"" + 
    (ei->emot->isVirtual() ? 
      "javascript:window.external.oController.getEmot(" + inttostr(ei->emot->getID()) + ");" :
      unifyPath(handler->getEmotDir() + "/" + ei->emotSet->getDir() + "/" + ei->emot->getImgPath())
    ) + "\" class=\"emoticon\" alt=\"" + ei->match + "\" />";
}

void EmotHandler::parseSet(RegEx& reg, eMSet& set) {
  if (!set.isEnabled()) return;

  for (eMSet::tEmots::iterator it = set.getEmots().begin(); it != set.getEmots().end(); it++) {
    sEmotInsertion ei(emotInsertions.size(), &*it, &set);
    try {
      reg.setPattern(prepareBody(!it->isPreg() ? "/" + reg.addSlashes(it->getText()) + "/i" : it->getText(), true, false));
      reg.replaceItself(&EmotHandler::emotInsertion, 0, (void*) &ei);
    } catch (const RegEx::CompileException& e) {
      IMLOG("[EmotHandler::parseSet()] B��d definicji emotikony: %s, pos %i", e.error, e.pos);
      continue;
    }
    emotInsertions.push_back(ei);
  }
}

String EmotHandler::parse(const StringRef& body, int net) {
  RegEx reg;
  reg.setSubject(prepareBody(body));

  /*
  if (emotSetsByNet.find(net) != emotSetsByNet.end() && emotSetsByNet[net].size()) {
    for (list<eMSet*>::iterator it = emotSetsByNet[net].begin(); it != emotSetsByNet[net].end(); it++) {
      parseSet(reg, **it);
    }
  } */
  for (tEmotSets::iterator it = emotSets.begin(); it != emotSets.end(); it++) {
    parseSet(reg, *it);
  }

  reg.replaceItself("#<kiev2:emot:insertion id=\"([0-9]+)\" />#", &EmotHandler::replaceEmot, 0, (void*) this);
  emotInsertions.clear();

  return prepareBody(reg.getSubject(), false);
}

void EmotHandler::loadPackages() {
  clearPackages();

  string emotDir = getEmotDir();
  Dir::tItems emotDirs;

  try {
    emotDirs = Dir::getDirs(emotDir + "\\*");
  } catch (const Exception& e) {
    IMLOG("[EmotHandler::loadPackages()] Nie znaleziono katalogu z emotami (%s) !", e.getReason().a_str());
    return;
  }
  if (!emotDirs.size()) {
    IMLOG("[EmotHandler::loadPackages()] Brak katalog�w z pakietami !");
    return;
  }

  for (Dir::tItems::iterator it = emotDirs.begin(); it != emotDirs.end(); it++) {
    for (tParsers::iterator it2 = parsers.begin(); it2 != parsers.end(); it2++) {

      string fileName = unifyPath(emotDir + "/" + it->cFileName + "/" + (*it2)->getDefFileName(it->cFileName));
      if (!fileExists(fileName.c_str())) continue;

      try {
        emotSets.push_back((*it2)->parse(fileName, it->cFileName));
      } catch (const EmotParserException& e) {
        IMLOG("[EmotHandler::loadPackages()] b��d podczas parsowania paczki emot (%s): %s", it->cFileName, e.getReason().a_str());
      }
      break;
    }
  }
}
