#ifndef THREAD_H
#define THREAD_H
#ifndef __WIN32__
#include <pthread.h>
#else
#include <windows.h>
#endif
#include <iostream>
#include <map>
#include <stdlib.h>
#include <signal.h>
using namespace std;
class Thread{
  public:
#ifndef __WIN32__
  pthread_t tid;
#else
  HANDLE tid;
#endif
  Thread(void *(*f)(void *),void * arg);
  void Kill();
  void Join();
};
class Mutex{
  public:
#ifndef __WIN32__
  pthread_mutex_t mid;
#else
  CRITICAL_SECTION mid;
#endif
  Mutex();
  void Lock();
  void UnLock();
  void TryLock();
  ~Mutex();
};
#endif