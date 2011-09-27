/*
*  Author(s):
*  - Chris  Kilner <ckilner@aldebaran-robotics.com>
*  - Cedric Gestes <gestes@aldebaran-robotics.com>
*
*  Copyright (C) 2010 Aldebaran Robotics
*/

#include "src/transport/zmq/zmq_poll_client.hpp"
#include "src/transport/zmq/zmq_client_backend.hpp"
#include <qimessaging/exceptions.hpp>
#include <iostream>
#include <qi/log.hpp>
#include <qi/os.hpp>

namespace qi {
  namespace transport {
    namespace detail {
      /// <summary> Constructor. </summary>
      /// <param name="serverAddress"> The server address. </param>
      ZMQClientBackend::ZMQClientBackend(const std::string &serverAddress, zmq::context_t &context)
        : ClientBackend(serverAddress),
          _zcontext(context),
          _zsocket(context, ZMQ_REQ),
          _poller(_zsocket)
      {
        int linger = 0;
#ifdef ZMQ_LINGER
        _zsocket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
#endif
        connect();
      }

      /// <summary> Connects to the server </summary>
      /// this is a little bit tricky, zmq does asynchronous connect/bind. Inproc will fail if
      /// bind is not ready
      void ZMQClientBackend::connect()
      {
        int i = 3;
        while (i > 0)
        {
          try {
            _zsocket.connect(_serverAddress.c_str());
            return;
          } catch(const std::exception& e) {
            qiLogDebug("qimessaging") << "ZMQClientBackend failed to create client for address \""
                << _serverAddress << "\" Reason: " << e.what() << std::endl;
            qiLogDebug("qimessaging") << "retrying.." << std::endl;
          }
          ++i;
          qi::os::msleep(100);
        }
        throw qi::transport::Exception("ZMQClientBackend cant connect.");
      }


      /// <summary> Sends. </summary>
      /// <param name="tosend"> The data to send. </param>
      /// <param name="result"> [in,out] The result. </param>
      void ZMQClientBackend::send(const std::string &tosend, std::string &result)
      {
        // TODO optimise this
        // Could we copy from the serialized stream without calling
        // stream.str() before sending to this method?
        //TODO: could we avoid more copy?
        zmq::message_t msg(tosend.size());
        memcpy(msg.data(), tosend.data(), tosend.size());
        _zsocket.send(msg);

        //we leave the possibility to timeout, pollRecv will throw and avoid the call to recv
        _poller.recv(&msg, 60 * 1000 * 1000);

        // TODO optimize this
        // boost could serialize from msg.data() and size,
        // without making a string
        result.assign((char *)msg.data(), msg.size());
      }
    }
  }
}
