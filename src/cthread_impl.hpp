#pragma once

#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <assert.h>
#include <iostream>

#include <threadapi.h>

class CThreadLockable : public LockableImplementation
{
  protected:
     std::mutex m_lock;
     std::condition_variable m_cond;
     std::unique_lock<std::mutex> m_ulock;

  public:
     CThreadLockable() : m_ulock(m_lock, std::defer_lock)
     {

     }

     void Init()
     {

     }

     ~CThreadLockable()
     {

     }

     void Destroy()
     {

     }

     void Lock()
     {
        m_lock.lock();
     }
     void Unlock()
     {
        m_lock.unlock();
     }

     void Wait()
     {
        m_cond.wait(m_ulock);
     }

     void WakeUp()
     {
        m_cond.notify_one();
     }   
};

class CThreadAPI : public ThreadAPIImplementation
{
  public:
    void CreateThread( void* (*f)(void* args), void* data)
    {
       m_Thread = new std::thread(f, data);
    }
    void Join()
    {
        assert(m_Thread != nullptr);
        m_Thread->join();
    }

    CThreadAPI() : m_Thread(nullptr) {}

    virtual ~CThreadAPI()
    {
      delete m_Thread;
      m_Thread = nullptr;
    }

  private:
    std::thread* m_Thread;
    int m_JoinStatus;
    int m_Status;
    int m_StatusAddr;

};


class CThreadFactory : public ThreadAPIFactoryImplementation
{
  public:
    LockableImplementation* CreateLockable()
    {
      return new CThreadLockable();
    }

    ThreadAPIImplementation* CreateThreadAPIImplementation()
    {
      return new CThreadAPI();
    }
};
