/*
 ** Author(s):
 **  - Herve Cuche <hcuche@aldebaran-robotics.com>
 **
 ** Copyright (C) 2012 Aldebaran Robotics
 */

#pragma once
#ifndef _QIMESSAGING_TRANSPORT_SERVER_DUMMY_P_HPP_
#define _QIMESSAGING_TRANSPORT_SERVER_DUMMY_P_HPP_

# include <qimessaging/api.hpp>
# include <qimessaging/url.hpp>
# include <qimessaging/session.hpp>
# include "src/transport_server_p.hpp"

# include <string>
# include <queue>

namespace qi
{
  class TransportServerDummyPrivate : public TransportServerPrivate
  {
  public:
    TransportServerDummyPrivate(qi::Session *session,
                                const qi::Url &url);
    virtual ~TransportServerDummyPrivate();
    virtual bool start();

  private:
    TransportServerDummyPrivate() {};
  };
}

#endif  // _QIMESSAGING_TRANSPORT_SERVER_DUMMY_P_HPP_