/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**
** Copyright (C) 2012 Aldebaran Robotics
*/

#ifndef SERVICE_DIRECTORY_HPP_
# define SERVICE_DIRECTORY_HPP_

#include <iostream>
#include <vector>
#include <map>

#include <qimessaging/transport.hpp>
#include <qimessaging/session.hpp>
#include <qimessaging/datastream.hpp>

namespace qi
{
  class ServiceDirectory : public qi::TransportServerInterface, public qi::TransportSocketInterface
  {
  public:
    ServiceDirectory();
    virtual ~ServiceDirectory();

    void start(const std::string &address);

  protected:
    virtual void newConnection();
    virtual void onDisconnected(TransportSocket *client);
    virtual void onConnected(TransportSocket *client);
    virtual void onWriteDone(TransportSocket *client);
    virtual void onReadyRead(TransportSocket *client, const qi::Message &msg);

  private:
    void services(const qi::Message &msg, qi::Message &retval);
    void service(const qi::Message &msg, qi::Message &retval);

    void registerEndpoint(const qi::Message &msg, qi::Message &retval);
    void unregisterEndpoint(const qi::Message &msg, qi::Message &retval);

  private:
    qi::NetworkThread                     *nthd;
    qi::TransportServer                   *ts;
    std::map<std::string, qi::ServiceInfo> connectedServices;

  }; // !ServiceDirectory
}; // !qi

#endif // !SERVICE_DIRECTORY_HPP_