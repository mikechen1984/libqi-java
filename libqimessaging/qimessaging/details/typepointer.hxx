#pragma once
/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/

#ifndef _QIMESSAGING_TYPEPOINTER_HXX_
#define _QIMESSAGING_TYPEPOINTER_HXX_
namespace qi
{
  template<typename T> class TypePointerImpl: public TypePointer
  {
  public:
    Type* pointedType() const
    {
      static Type* result = 0;
      if (!result)
        result = typeOf<T>();
      return result;
    }

    GenericValue dereference(void* storage)
    {
      GenericValue result;
      result.type = pointedType();
      // We are in DirectAccess mode, so storage is a T*.
      result.value = result.type->initializeStorage(storage);
      return result;
    }

    typedef DefaultTypeImplMethods<T*,
                                     TypeDirectAccess<T*>,
                                     TypeNoClone<TypeDirectAccess<T*> >,
                                     TypeNoValue<TypeDirectAccess<T*> >,
                                     TypeNoSerialize<TypeDirectAccess<T*> >
                                     > TypeMethodsImpl;
    _QI_BOUNCE_TYPE_METHODS(TypeMethodsImpl);
  };

  template<typename T> class TypeImpl<T*>: public TypePointerImpl<T>{};
}

#endif