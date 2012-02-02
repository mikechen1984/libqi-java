/*
*  Author(s):
*  - Cedric Gestes <gestes@aldebaran-robotics.com>
*  - Chris  Kilner <ckilner@aldebaran-robotics.com>
*
*  Copyright (C) 2010 Aldebaran Robotics
*/

#include <qimessaging/serialization/message.hpp>
#include <vector>

namespace qi {

#if 0
#define __QI_DEBUG_SERIALIZATION_DATA_R(x, d) {            \
  std::string sig = qi::signature< x >::value();           \
  std::cout << "read (" << sig << "): " << d << std::endl; \
}

#define __QI_DEBUG_SERIALIZATION_DATA_W(x, d) {            \
  std::string sig = qi::signature< x >::value();           \
  std::cout << "write(" << sig << "): " << d << std::endl; \
}
#else
# define __QI_DEBUG_SERIALIZATION_DATA_R(x, d)
# define __QI_DEBUG_SERIALIZATION_DATA_W(x, d)
#endif

#define QI_SIMPLE_SERIALIZER_IMPL(Name, Type)              \
  void DataStream::read##Name(Type& b)                        \
  {                                                        \
    b = *((Type *) (_data.data() + _index));               \
    _index += sizeof(Type);                                \
    __QI_DEBUG_SERIALIZATION_DATA_R(Type, b);              \
  }                                                        \
                                                           \
  void DataStream::write##Name(const Type& b)                 \
  {                                                        \
    _data.append((char *)&b, sizeof(b));                   \
    __QI_DEBUG_SERIALIZATION_DATA_W(Type, b);              \
  }

  QI_SIMPLE_SERIALIZER_IMPL(Bool,   bool);
  QI_SIMPLE_SERIALIZER_IMPL(Char,   char);
  QI_SIMPLE_SERIALIZER_IMPL(Int,    int);
  QI_SIMPLE_SERIALIZER_IMPL(Float,  float);
  QI_SIMPLE_SERIALIZER_IMPL(Double, double);

  // string
  const char *DataStream::readString(size_t &len)
  {
    int sz;
    readInt(sz);
    len = sz;
    return _data.data();
  }

  void DataStream::writeString(const char *str, size_t len)
  {
    writeInt(len);
    if (len) {
      _data.append(str, len);
      __QI_DEBUG_SERIALIZATION_DATA_W(std::string, str);
    }
  }

  // string
  void DataStream::readString(std::string& s)
  {
    int sz;
    readInt(sz);
    s.clear();
    if (sz) {
      s.append(_data.data() + _index, sz);
      _index += sz;
      __QI_DEBUG_SERIALIZATION_DATA_R(std::string, s);
    }
  }

  void DataStream::writeString(const std::string &s)
  {
    writeInt(s.size());
    if (!s.empty()) {
      _data.append(s.data(), s.size());
      __QI_DEBUG_SERIALIZATION_DATA_W(std::string, s);
    }
  }

}

