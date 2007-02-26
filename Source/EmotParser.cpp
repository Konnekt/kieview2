/**
  *  kIEview2 Emot parser class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @copyright    Copyright (c) 2006-2007 Micha³ "Dulek" Dulko
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 43 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-31 16:47:02 +0100 (N, 31 gru 2006) $
  */

#include "stdafx.h"
#include "EmotParser.h"

iPackage* JispParser::parse(const FindFile::Found& defFile) {
  // parsujemy xmla z definicjami
  xmlpp::DomParser parser;
  parser.set_substitute_entities();

  xmlpp::Node* rootNode;
  xmlpp::Node* metaNode;
  xmlpp::Node::NodeList icons;
  xmlpp::Node::NodeList nodes;
  xmlpp::Attribute* attrib;
  eMSet result;

  ifstream file(defFile.getFilePath().c_str());
  try {
    parser.parse_stream(file);
  } catch (const xmlpp::exception& e) {
    throw XMLParserException(e.what());
  }
  file.close();

  rootNode = parser.get_document()->get_root_node();
  if (rootNode->get_name() != "icondef") throw WrongFormat("Z³a nazwa korzenia dokumentu (z³y format ?)");

  nodes = rootNode->get_children("meta");
  if (nodes.size() != 1) throw WrongFormat("Nie ma dok³adnie jednego elementu 'meta'");
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
    eM emot(true, false);
    string mime;

    nodes = (*it)->get_children("object");
    for (xmlpp::Node::NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
      if (dynamic_cast<xmlpp::Element*>(*it) && dynamic_cast<xmlpp::Element*>(*it)->get_attribute("mime")) {
        mime = dynamic_cast<xmlpp::Element*>(*it)->get_attribute("mime")->get_value();
        if (mime == "image/png" || mime == "image/gif" || mime == "image/jpeg") {
          emot.setImgPath(defFile.getDirectory() + (string) dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content());
          emot.setMenuImgPath(emot.getImgPath());
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
  return new eMSet(result);
}

iPackage* GGParser::parse(const FindFile::Found& defFile) {
  ifstream file(defFile.getFilePath().c_str());
  string buff;

  if (!file.is_open()) {
    throw CannotOpen("Cannot open file " + defFile.getFilePath());
  }

  bool inMenu;
  eMSet result;
  RegEx reg;

  while (!file.eof()) {
    getline(file, buff);
    eMSet::tEmots emots;

    if (buff[0] == '*') {
      inMenu = false;
      buff.erase(0);
    } else {
      inMenu = true;
    }

    reg.setSubject(buff);
    reg.setPattern("/,?\"(.+?)\",?/");

    while (reg.match_global()) {
      eM emot(inMenu, reg[1][0] == '/');
      emot.setText(reg[1].c_str());
      emots.push_back(emot);

      if (reg.getSubject()[reg.getStart()] == ')' || buff[0] != '(') {
        string img_path;
        string menu_img_path;

        if (reg.match_global()) {
          img_path = reg[1];
          menu_img_path = reg.match_global() ? reg[1] : img_path;

          for (eMSet::tEmots::iterator it = emots.begin(); it != emots.end(); it++) {
            it->setMenuImgPath(defFile.getDirectory() + menu_img_path);
            it->setImgPath(defFile.getDirectory() + img_path);
          }
        } else {
          throw WrongFormat("Brak œcie¿ki do obrazka");
        }
        break;
      }
    }
    result.getEmots().insert(result.getEmots().end(), emots.begin(), emots.end());
  }
  file.close();

  return new eMSet(result);
}
