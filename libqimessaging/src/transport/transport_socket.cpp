/*
** transport-client.cpp
** Login : <hcuche@hcuche-de>
** Started on  Thu Jan  5 15:21:13 2012 Herve Cuche
** $Id$
**
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**
** Copyright (C) 2012 Herve Cuche
*/

#include <iostream>
#include <cstring>
#include <map>

#include <qi/log.hpp>

#include <event2/util.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <qimessaging/transport/transport_socket.hpp>
#include <qimessaging/transport/network_thread.hpp>

#define MAX_LINE 16384

namespace qi {


struct TransportSocketPrivate
{
  TransportSocketPrivate()
    : connected(false)
    , bev(NULL)
  {
  }

  ~TransportSocketPrivate()
  {
  }

  TransportSocketDelegate             *tcd;
  struct bufferevent                  *bev;
  bool                                 connected;
  std::map<unsigned int, qi::Message*> msgSend;

};



static void readcb(struct bufferevent *bev,
                   void *context)
{
  TransportSocket *tc = static_cast<TransportSocket*>(context);
  tc->readcb(bev, context);
}

static void writecb(struct bufferevent* bev,
                    void* context)
{
  TransportSocket *tc = static_cast<TransportSocket*>(context);
  tc->writecb(bev, context);
}


static void eventcb(struct bufferevent *bev,
                    short error,
                    void *context)
{
  TransportSocket *tc = static_cast<TransportSocket*>(context);
  tc->eventcb(bev, error, context);
}


void TransportSocket::readcb(struct bufferevent *bev,
                             void *context)
{
  char buf[1024];
  memset(buf, '\0', 1024);
  size_t n;
  struct evbuffer *input = bufferevent_get_input(bev);

  while ((n = evbuffer_remove(input, buf, sizeof(buf))) > 0)
  {
    std::string m(buf, n);
    qi::Message *ans = new qi::Message(m);
    _p->msgSend[ans->id()] = ans;
    _p->tcd->onRead(*ans);
  }
}

void TransportSocket::writecb(struct bufferevent* bev,
                              void* context)
{
  qi::Message msg;
  _p->tcd->onWrite(msg);
}

void TransportSocket::eventcb(struct bufferevent *bev,
                              short events,
                              void *context)
{
  if (events & BEV_EVENT_CONNECTED)
  {
    qi::Message msg;
    _p->tcd->onConnected(msg);
    _p->connected = true;
  }
  else if (events & BEV_EVENT_EOF)
  {
    // connection has been closed, do any clean up here
    qiLogError("qimessaging.TransportSocket") << "connection has been closed, do any clean up here" << std::endl;
  }
  else if (events & BEV_EVENT_ERROR)
  {
    bufferevent_free(_p->bev);
    // check errno to see what error occurred
    qiLogError("qimessaging.TransportSocket")  << "Cannnot connect" << std::endl;
  }
  else if (events & BEV_EVENT_TIMEOUT)
  {
    // must be a timeout event handle, handle it
    qiLogError("qimessaging.TransportSocket")  << "must be a timeout event handle, handle it" << std::endl;
  }
}

TransportSocket::TransportSocket()
{
  _p = new TransportSocketPrivate();
}


TransportSocket::~TransportSocket()
{
  disconnect();
  delete _p;
}

bool TransportSocket::connect(const std::string &address,
                              unsigned short port,
                              struct event_base *base)
{
  if (!_p->connected)
  {
    _p->bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(_p->bev, ::qi::readcb, ::qi::writecb, ::qi::eventcb, this);
    bufferevent_setwatermark(_p->bev, EV_WRITE, 0, MAX_LINE);
    bufferevent_enable(_p->bev, EV_READ|EV_WRITE);

    bufferevent_socket_connect_hostname(_p->bev, NULL, AF_INET, address.c_str(), port);

    if (_p->connected)
      return true;
  }

  return false;
}

bool TransportSocket::waitForConnected(int msecs)
{
  // no timeout
  if (msecs < 0)
  {
    while (!_p->connected)
      ;

    return true;
  }

  while (!_p->connected && msecs > 0)
  {
    qi::os::msleep(1);
    msecs--;
  }

  // timeout
  if (msecs == 0)
    return false;

  return true;
}

void TransportSocket::disconnect()
{
  if (_p->connected)
  {
    bufferevent_free(_p->bev);
    _p->bev = NULL;
    _p->connected = false;
  }
}

bool TransportSocket::waitForDisconnected(int msecs)
{
  // no timeout
  if (msecs < 0)
  {
    while (_p->connected)
      ;

    return true;
  }

  while (_p->connected && msecs > 0)
  {
    qi::os::msleep(1);
    msecs--;
  }

  // timeout
  if (msecs == 0)
    return false;

  return true;
}

bool TransportSocket::waitForId(int id, int msecs)
{
  std::map<unsigned int, qi::Message*>::iterator it;
  // no timeout
  if (msecs < 0)
  {
    do
    {
      it = _p->msgSend.find(id);
    }
    while (it == _p->msgSend.end());

    return true;
  }

  do
  {
    qi::os::msleep(1);
    msecs--;
    it = _p->msgSend.find(id);
  }
  while (it == _p->msgSend.end() && msecs > 0);

  // timeout
  if (msecs == 0)
    return false;

  return true;
}

void TransportSocket::read(int id, qi::Message *msg)
{
  std::map<unsigned int, qi::Message*>::iterator it;
  it = _p->msgSend.find(id);
  if (it != _p->msgSend.end())
  {
    qi::Message ans = *(it->second);
    *msg = ans;
    _p->msgSend.erase(it);
  }
}

bool TransportSocket::send(const qi::Message &msg)
{
  if (_p->connected && !bufferevent_write(_p->bev, msg.str().c_str(), msg.str().size()))
  {
    return true;
  }
  return false;
}

void TransportSocket::setDelegate(TransportSocketDelegate *delegate)
{
  _p->tcd = delegate;
}

}