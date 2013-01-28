
#include "Thread.h"
#include <stdio.h>
Thread::Thread(void *(*f)(void *),void * arg)
{
#ifndef __WIN32__
  printf("Spawning new thread");
  int err = pthread_create(&tid,NULL,f,arg);
  if ( err != 0)
  {
    printf("FATAL: Thread failed to spawn");
    abort();
  }
#else
  tid = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)f,arg,0x0,NULL);

#endif
}
void Thread::Join()
{
#ifndef __WIN32__
  pthread_join(tid,NULL);
#else
  WaitForSingleObject(tid,INFINITE);
#endif
}
void Thread::Kill()
{
 // pthread_kill(tid,SIGHUP); Epic fail
}

Mutex::Mutex()
{
#ifndef __WIN32__
  if (pthread_mutex_init(&mid,NULL) != 0)
  {
    printf("Cannot create mutex");
  }
#else
  InitializeCriticalSection(&mid);
#endif
}
void Mutex::Lock()
{
#ifndef __WIN32__
  pthread_mutex_lock(&mid);
#else
  EnterCriticalSection(&mid);
#endif
}
void Mutex::UnLock()
{
#ifndef __WIN32__
  pthread_mutex_unlock(&mid);
#else
  LeaveCriticalSection(&mid);
#endif
}
void Mutex::TryLock()
{
#ifndef __WIN32__
   pthread_mutex_trylock(&mid);
#else
   Lock();
#endif
}
Mutex::~Mutex()
{
#ifdef __WIN32__
  DeleteCriticalSection(&mid);
#endif
}
  
