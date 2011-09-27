/*
*  Author(s):
*  - Chris  Kilner <ckilner@aldebaran-robotics.com>
*  - Cedric Gestes <gestes@aldebaran-robotics.com>
*
*  Copyright (C) 2010 Aldebaran Robotics
*/

#include "src/messaging/publisher_impl.hpp"
#include <qimessaging/transport/transport_publisher.hpp>
#include "src/messaging/network/master_endpoint.hpp"
#include "src/messaging/network/endpoints.hpp"
#include <qi/log.hpp>

namespace qi {
  namespace detail {

    PublisherImpl::PublisherImpl(const std::string& name, Context *ctx)
      : ImplBase(ctx),
        _publisherInitialized(false)
    {
      _endpointContext.type = PUBLISHER_ENDPOINT;
      _endpointContext.name = name;
      _endpointContext.contextID = _masterClient.getQiContextPtr()->getID();
    }

    void PublisherImpl::connect(const std::string& masterAddress) {
      _masterClient.connect(masterAddress);
      _masterClient.registerMachine(_machineContext);
      _isInitialized = _masterClient.isInitialized();
    }


      void PublisherImpl::advertiseTopic(const std::string& topicSignature,
        const bool& isManyToMany) {
        if (!_isInitialized) {
          qiLogError("qimessaging") << "Publisher::advertiseTopic \"" << topicSignature
            << "\" Attempt to use uninitialized publisher." << std::endl;
          return;
        }
        if (! _publisherInitialized) {
          if (! xInitOneToManyPublisher()) {
            qiLogError("qimessaging") << "Publisher::advertiseTopic \"" << topicSignature
              << "\" Failed to initialize oneToMany publisher." << std::endl;
            return;
          }
        }

        bool exists = _masterClient.topicExists(topicSignature);

        // Many to many
        if (isManyToMany) {
          if (! exists) {
            // this will create the forwarder on the master
            _masterClient.registerTopic(topicSignature, isManyToMany, _endpointContext);
          }
          // get the publish address of the forwarder
          std::string endpoint = _masterClient.locateTopic(topicSignature, _endpointContext);
          if (endpoint.empty()) {
            qiLogError("qimessaging") << "Publisher::advertiseTopic \"" << topicSignature
              << "\" Failed to locate endpoint for manyToMany publisher" << std::endl;
            // TODO throw
            return;
          }
          // This will thow if the endpoint is invalid
          if (! xCreateManyToManyPublisher(endpoint)) {
            qiLogError("qimessaging") << "Publisher::advertiseTopic \"" << topicSignature
              << "\" Failed to initialize manyToMany publisher" << std::endl;
            // TODO throw
            return;
          }
          _knownTopics.insert(topicSignature, endpoint);
          if (exists) {
            _masterClient.registerTopicParticipant(topicSignature, _endpointContext.endpointID);
          }
          return;
        }

        // One to Many
        if (exists) {
          qiLogError("qimessaging") << "Attempt to publish on an existing topic \""
            << topicSignature << "\" with isManyToMany = false: " << std::endl;
          return;
        }
        _masterClient.registerTopic(topicSignature, isManyToMany, _endpointContext);
        _knownTopics.insert(topicSignature, _endpointContext.endpointID);
      }

      void PublisherImpl::unadvertiseTopic(const std::string& topicSignature) {
        _masterClient.unregisterTopic(topicSignature, _endpointContext);
      }

    bool PublisherImpl::xInitOneToManyPublisher() {
      // prepare this publisher
      _endpointContext.port = _masterClient.getNewPort(_machineContext.machineID);
      std::vector<std::string> publishAddresses = getEndpoints(_endpointContext, _machineContext);

      // init the publisher that binds ports
      try {
        TPubPtr pub(new qi::transport::TransportPublisher(
          _masterClient.getQiContextPtr()->getTransportContext()));
        pub->init();
        pub->bind(publishAddresses);
        _transportPublishers.insert(_endpointContext.endpointID, pub);
        _masterClient.registerEndpoint(_endpointContext);
        _publisherInitialized = true;
      } catch(const std::exception & e) {
        _publisherInitialized = false;

        qiLogDebug("qimessaging") << "Publisher failed to create publisher for addresses" << std::endl;
        std::vector<std::string>::const_iterator it =  publishAddresses.begin();
        std::vector<std::string>::const_iterator end = publishAddresses.end();
        for(; it != end; ++it) {
          qiLogDebug("qimessaging") << *it << std::endl;
        }
        qiLogDebug("qimessaging") << "Reason: " << e.what() << std::endl;
      }
      return _publisherInitialized;
    }

    bool PublisherImpl::xCreateManyToManyPublisher(const std::string& connectEndpoint) {
      try {
        TPubPtr pub(new qi::transport::TransportPublisher(
          _masterClient.getQiContextPtr()->getTransportContext()));
        pub->init();
        // could throw
        pub->connect(connectEndpoint);
        _transportPublishers.insert(connectEndpoint, pub);
      } catch(const std::exception& e) {
        qiLogDebug("qimessaging") << "Publisher failed to connect to address: \"" <<
          connectEndpoint << "\" Reason: " << e.what() << std::endl;
        return false;
      }
      return true;
    }

    void PublisherImpl::publish(const std::string& topicSignature, const std::string& data)
    {
      if (! _isInitialized) {
        qiLogError("qimessaging") << "Publisher::publish Attempt to use an "
          << "uninitialized publisher." << std::endl;
        return;
      }

      // find the endpoint for this topic
      const std::string& endpointID = _knownTopics.get(topicSignature);
      if (endpointID.empty()) {
        qiLogError("qimessaging") << "Publisher::publish Attempt to publish to "
          << "unadvertised topic: " << topicSignature << std::endl;
        return;
      }

      // find the transport publisher for this endpoint
      const TPubPtr& pub = _transportPublishers.get(endpointID);
      if (pub == NULL) {
        qiLogError("qimessaging") << "Publisher::publish Unable to find transport for "
          << endpointID << std::endl;
        return;
      }

      pub->publish(data);
    }

    PublisherImpl::~PublisherImpl() {
      if (_publisherInitialized) {
        _masterClient.unregisterEndpoint(_endpointContext);
      }
    }
  }
}