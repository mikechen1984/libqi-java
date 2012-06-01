/*
**
** Copyright (C) 2012 Aldebaran Robotics
*/

#include "src/metamethod_p.hpp"
#include <qimessaging/metamethod.hpp>

namespace qi {

  MetaMethod::MetaMethod()
  {
    _p = new MetaMethodPrivate();
  }

  MetaMethod::MetaMethod(const std::string &sig, const qi::Functor *functor)
  {
    _p = new MetaMethodPrivate(sig, functor);
  }

  MetaMethod::MetaMethod(const MetaMethod &other)
  {
    _p = new MetaMethodPrivate();
    *_p = *(other._p);
  }

  MetaMethod& MetaMethod::operator=(const MetaMethod &other)
  {
    _p = new MetaMethodPrivate();
    *_p = *(other._p);
    return (*this);
  }

  MetaMethod::~MetaMethod()
  {
    delete _p;
  }

  const std::string &MetaMethod::signature() const
  {
    return _p->signature();
  }

  const qi::Functor *MetaMethod::functor() const
  {
    return _p->_functor;
  }

  unsigned int       MetaMethod::index() const
  {
    return _p->index();
  }

};