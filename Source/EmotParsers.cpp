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
#include "Emots.h"
#include "EmotParsers.h"

EmotSet JispParser::parse(string str) {
  xmlpp::Node* rootNode;
  xmlpp::Node* metaNode;
  xmlpp::Node::NodeList icons;
  xmlpp::Node::NodeList nodes;
  EmotSet result;

  try {
    this->parser.parse_memory(str.c_str());
  } catch(xmlpp::exception ex) {
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

EmotSet GGEmotParser::parse(string str) {
  EmotSet result;
  parser.setSubject(str);
  parser.setPattern("/\\((\".+?\",?)+\\),\".+?\"(,\".+?\")?/");
  while (parser.match_global()) {
    EmotSet::tEmots emots;
    subParser.setSubject(parser[0]);
    subParser.setPattern("/,?\"(.+?)\",?/");
    while (subParser.match_global()) {
      emots.push_back(Emot(subParser[1].c_str(), "", "", subParser[1][0] == '/'));
      if (subParser.getSubject()[subParser.getStart()] == ')') {
        string img_path;
        string menu_img_path;
        if (subParser.match_global()) {
          img_path = subParser[1];
          menu_img_path = subParser.match_global() ? subParser[1] : img_path;
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