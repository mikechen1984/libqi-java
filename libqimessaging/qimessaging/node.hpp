#pragma once
/*
*  Author(s):
*  - Chris  Kilner <ckilner@aldebaran-robotics.com>
*  - Cedric Gestes <gestes@aldebaran-robotics.com>
*
*  Copyright (C) 2010 Aldebaran Robotics
*/


#ifndef _QIMESSAGING_NODE_HPP_
#define _QIMESSAGING_NODE_HPP_

#include <qimessaging/server.hpp>
#include <qimessaging/client.hpp>
#include <qimessaging/publisher.hpp>
#include <qimessaging/subscriber.hpp>
#include <qimessaging/api.hpp>

namespace qi {
  /// <summary>
  /// A combination of Server, Client, Publisher and Subscriber that
  /// exposes all their methods.
  /// </summary>
  /// \ingroup Messaging
  class QIMESSAGING_API Node :
    public Server,
    public Client,
    public Publisher,
    public Subscriber
  {
  public:
    /// <summary> Finaliser. </summary>
    virtual ~Node();

    /// <summary> Creates a Node </summary>
    /// <param name="name"> The name you want to give to the node. </param>
    /// <param name="context">
    /// An optional context that can be used to group or separate
    /// transport resources.
    /// </param>
    Node(const std::string& name = "", qi::Context *context = 0);

    /// <summary> Connect to masterAddress. If no address is specified
    /// the default 127.0.0.1:5555 is used </summary>
    /// <param name="masterAddress"> The master address. </param>
    void connect(const std::string &masterAddress = "127.0.0.1:5555");

  };
}

#endif  // _QIMESSAGING_NODE_HPP_
