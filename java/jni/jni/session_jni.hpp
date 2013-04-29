/*
**
** Author(s):
**  - Pierre ROULLON <proullon@aldebaran-robotics.com>
**
** Copyright (C) 2012 Aldebaran Robotics
*/

#ifndef _JAVA_JNI_SESSION_HPP_
#define _JAVA_JNI_SESSION_HPP_

#include <qimessaging/api.hpp>
#include <jni.h>

extern "C"
{
  QIMESSAGING_API jlong     Java_com_aldebaran_qimessaging_Session_qiSessionCreate();
  QIMESSAGING_API void      Java_com_aldebaran_qimessaging_Session_qiSessionDestroy(jlong pSession);
  QIMESSAGING_API jboolean  Java_com_aldebaran_qimessaging_Session_qiSessionIsConnected(JNIEnv *env, jobject obj, jlong pSession);
  QIMESSAGING_API jboolean  Java_com_aldebaran_qimessaging_Session_qiSessionConnect(JNIEnv *env, jobject obj, jlong pSession, jstring jurl);
  QIMESSAGING_API void      Java_com_aldebaran_qimessaging_Session_qiSessionClose(jlong pSession);
  QIMESSAGING_API jlong     Java_com_aldebaran_qimessaging_Session_qiSessionService(JNIEnv *env, jobject obj, jlong pSession, jstring jname);
  QIMESSAGING_API jlong     Java_com_aldebaran_qimessaging_Session_qiSessionRegisterService(JNIEnv *env, jobject obj, jlong pSession, jlong pObject, jstring name);
} // !extern "C"

#endif // !_JAVA_JNI_SESSION_HPP_
