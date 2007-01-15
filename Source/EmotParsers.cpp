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
  if (rootNode->get_name() != "icondef") throw JispParser::WrongTagException("Z³a nazwa korzenia dokumentu (z³y format ?)");

  nodes = rootNode->get_children("meta");
  if (nodes.size() != 1) throw WrongTagException("Nie ma dok³adnie jednego elementu 'meta'");
  metaNode = *nodes.begin();
  if (!metaNode) throw WrongTagException("Element 'meta' nie zawiera dzieci");

  nodes = metaNode->get_children("name");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.name = dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content();
  }

  nodes = metaNode->get_children("version");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.version = dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content();
  }

  nodes = metaNode->get_children("description");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.description = dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content();
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
    result.creationTime = dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content();
  }

  nodes = metaNode->get_children("home");
  if (nodes.size() == 1 && dynamic_cast<xmlpp::Element*>(*nodes.begin())) {
    result.url = dynamic_cast<xmlpp::Element*>(*nodes.begin())->get_child_text()->get_content();
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
          emot.imgPath = dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content();
          break;
        }
        mime.clear();
      }
    }

    if (mime.empty()) continue;

    nodes = (*it)->get_children("text");
    for (xmlpp::Node::NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
      if (dynamic_cast<xmlpp::Element*>(*it)) {
        emot.text = dynamic_cast<xmlpp::Element*>(*it)->get_child_text()->get_content();
        result.emots.push_back(emot);
      }
    }
  }
  return result;
}