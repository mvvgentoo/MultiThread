#pragma once

#include <pthread.h>
#include <threadapi.h>

class PthreadLockable : public LockableImplementation
{
  protected:
     pthread_mutex_t m_lock;
     pthread_cond_t m_cond;

  public:
     PthreadLockable()
     {

     }

     void Init()
     {
       pthread_mutex_init(&m_lock, NULL);
       //m_lock = PTHREAD_MUTEX_INITIALIZER;
     }

     ~PthreadLockable()
     {

     }

     void Destroy()
     {
        pthread_mutex_destroy(&m_lock);
     }

     void Lock()
     {
        pthread_mutex_lock(&m_lock);
     }
     void Unlock()
     {
        pthread_mutex_unlock(&m_lock);
     }

     void Wait()
     {
        pthread_cond_wait(&m_cond, &m_lock);
     }

     void WakeUp()
     {
        pthread_cond_signal(&m_cond);
     }
};

class PThreadAPI : public ThreadAPIImplementation
{
  public:
    void CreateThread( void* (*f)(void* args), void* data)
    {
       m_Status = pthread_create(&m_Thread, NULL, f, data);
    }
    void Join()
    {
        m_JoinStatus = pthread_join(m_Thread, (void**)&m_StatusAddr);
    }
  private:
    pthread_t m_Thread;
    int m_JoinStatus;
    int m_Status;    
    int m_StatusAddr;

};

class PThreadFactory : public ThreadAPIFactoryImplementation
{
  public:
    LockableImplementation* CreateLockable()
    {
      return new PthreadLockable();
    }

    ThreadAPIImplementation* CreateThreadAPIImplementation()
    {
      return new PThreadAPI();
    }
};
