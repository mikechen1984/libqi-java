/*
*  Author(s):
*  - Cedric Gestes <gestes@aldebaran-robotics.com>
*  - Chris  Kilner <ckilner@aldebaran-robotics.com>
*
*  Copyright (C) 2010 Aldebaran Robotics
*/

#include <qi/messaging/node.hpp>
#include <qi/messaging/context.hpp>

namespace qi {
  Node::~Node() {}

  Node::Node(const std::string& name, Context *ctx)
    : Server(name, ctx),
      Client(name, ctx),
      Publisher(name, ctx),
      Subscriber(name, ctx)
  {
  }


}