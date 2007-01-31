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

EmotSet EmotHandler::JispParser::parse(string str) {
  xmlpp::Node* rootNode;
  xmlpp::Node* metaNode;
  xmlpp::Node::NodeList icons;
  xmlpp::Node::NodeList nodes;
  EmotSet result;

  try {
    this->parser.parse_memory(str.c_str());
  } catch (xmlpp::exception ex) {
    throw XMLParserException(ex.what());
  }

  rootNode = this->parser.get_document()->get_root_node();
  if (rootNode->get_name() != "icondef") throw JispParser::WrongFormat("Z³a nazwa korzenia dokumentu (z³y format ?)");

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
    if(nodes.empty()) throw JispParser::WrongFormat("Brak tekstu do zamiany");
    for (xmlpp::Node::NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
      if (dynamic_cast<xmlpp::Element*>(*it)) {
        emot.text = dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content().c_str();
        result.emots.push_back(emot);
      }
    }
  }
  return result;
}

EmotSet EmotHandler::GGEmotParser::parse(string str) {
  EmotSet result;
  EmotSet::tEmots emots;
  tStringVector strings;
  bool inMenu;

  split(str, "\n", strings);

  for (tStringVector::iterator it = strings.begin(); it != strings.end(); it++) {
    if ((*it)[0] == '*') {
      inMenu = false;
      (*it).erase(0);
    } else {
      inMenu = true;
    }

    parser.setSubject(*it);
    parser.setPattern("/,?\"(.+?)\",?/");

    while (parser.match_global()) {
      emots.push_back(Emot(parser[1].c_str(), "", "", parser[1][0] == '/', inMenu));

      if (parser.getSubject()[parser.getStart()] == ')' || (*it)[0] != '(') {
        string img_path;
        string menu_img_path;
        if (parser.match_global()) {
          img_path = parser[1];
          menu_img_path = parser.match_global() ? parser[1] : img_path;
          for (EmotSet::tEmots::iterator it = emots.begin(); it != emots.end(); it++) {
            it->img_path = img_path;
            it->menu_img_path = menu_img_path;
          }
        } else {
          throw GGEmotParser::WrongFormat("Brak œcie¿ki do obrazka");
        }
        break;
      }
    }
    result.emots.merge(emots);
  }
  return result;
}

String EmotHandler::EmotReplacer::replace(StringRef &str, EmotSet &set) {
  parser.setSubject(str.a_string());

  for (EmotSet::tEmots::iterator it = set.emots.begin(); it != set.emots.end(); it++) {
    if (it->preg) {
      parser.setPattern(it->text);
    } else {
      parser.setPattern(parser.addSlashes(it->text));
    }
    parser.replaceItself(((string)"<img src=\"" + it->img_path + "\" />").c_str());
  }

  return parser.getSubject();
}

EmotHandler::EmotHandler(const string& emotDir) {
  setEmotDir(emotDir);
}

void EmotHandler::addEmotDir(const string& dir) {
  if (!dir.size()) return;

  string emotDir = dir;
  emotDir = unifyPath(emotDir);
  emotDir = Helpers::ltrim(emotDir, ".\\");

  emotDirs.push_back(emotDir);
}

void EmotHandler::loadPackages() {
  try {
    filesystem::path path(kPath + emotDir, filesystem::native);
    for (filesystem::directory_iterator it(path); it != filesystem::directory_iterator(); it++) {
      if (filesystem::is_directory(*it)) {
        addEmotDir(it->string());
      }
    }
  } catch (const std::exception& ex) {
    //@todo nie ma katalogu emots - robimy coœ?
  }

  for (tEmotDirs::iterator it = emotDirs.begin(); it != emotDirs.end(); it++) {
    string title = it->substr(it->find_last_of("\\") + 1);

    if(fileExists((*it + "\\" + title + ".jisp").c_str())) {
      HZIP file;
      OFSTRUCT ofs;
      ZIPENTRY entry;
      int index;
      file = OpenZip((void*)OpenFile((*it + "\\" + title + ".jisp").c_str(), &ofs, OF_READ), 0, ZIP_HANDLE);
      if (FindZipItem(file, (title + "/icondef.xml").c_str(), true, &index, &entry) == ZR_OK) {
        string code;
        char* buff = new char[entry.unc_size];
        UnzipItem(file, index, buff, entry.unc_size, ZIP_MEMORY);
        code = buff;
        delete buff;

        try {
          emotSets.push_back(jispParser.parse(code));
        } catch (JispParser::WrongFormat& ex) {
          //@todo z³y format pliku - robimy coœ?
        } catch (JispParser::XMLParserException& ex) {
          //@todo libXML++ rzuci³ wyj¹tek - robimy coœ?
        }
      } else {
        //@todo brak g³ównego pliku JISP - robimy coœ?
      }
    }

    if(fileExists((*it + "\\" + title + ".txt").c_str())) {
      ifstream file((*it + "\\" + title + ".txt").c_str());
      string code;
      string buff;
      while (!file.eof()) {
        getline(file, buff);
        code += buff + "\n";
      }
      file.close();

      try {
        emotSets.push_back(ggEmotParser.parse(code));
      } catch (GGEmotParser::WrongFormat& ex) {
        //@todo z³y format pliku - robimy coœ?
      }
    }
  }
}