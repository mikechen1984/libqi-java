/*
** Author(s):
**  - Cedric GESTES      <gestes@aldebaran-robotics.com>
**  - Chris Kilner       <ckilner@aldebaran-robotics.com>
**
** Copyright (C) 2010 Aldebaran Robotics
*/

#ifndef QI_MESSAGING_I_GENERIC_MESSAGE_HANDLER_HPP_
#define QI_MESSAGING_I_GENERIC_MESSAGE_HANDLER_HPP_

namespace qi {
  namespace messaging {
    template<typename T, typename R>
    class IGenericMessageHandler {
    public:
      /// <summary> Generic message handler. </summary>
      /// <param name="requestMessage"> The request </param>
      /// <param name="responseMessage"> [in,out] The reply </param>
      virtual void messageHandler(const T &requestMessage, R& responseMessage) = 0;
    };
  }
}

#endif  // QI_MESSAGING_I_GENERIC_MESSAGE_HANDLER_HPP_
