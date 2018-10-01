#pragma once

#include <memory>

class LockableImplementation
{
  public:
    virtual ~LockableImplementation() {}
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
    virtual void Destroy() = 0;
    virtual void Init() = 0;    
    virtual void Wait() = 0;
    virtual void WakeUp() = 0;
};

class ThreadAPIImplementation
{
  public:
    virtual ~ThreadAPIImplementation() {}
    virtual void CreateThread( void* (*f)(void* args), void* data) = 0;
    virtual void Join() = 0;
};

class Lockable
{
  protected:
     LockableImplementation* m_Impl;
  public:
     Lockable();
     virtual ~Lockable();
     virtual void Lock();
     virtual void Unlock();
     virtual void Wait();
     virtual void WakeUp();
};

class ThreadAPIFactoryImplementation
{
  public:
    virtual LockableImplementation* CreateLockable() = 0;
    virtual ThreadAPIImplementation* CreateThreadAPIImplementation() = 0;
};

class ThreadAPIFactory
{
  public:
    static LockableImplementation* CreateLockable();
    static ThreadAPIImplementation* CreateThreadAPIImplementation();

  private:
     static std::unique_ptr<ThreadAPIFactoryImplementation> m_Impl;
};

template <class T>
class ThreadResource : public Lockable
{
    public:
        T Data;
        ThreadResource() {}
        ~ThreadResource() {}
};

class AutoLock
{
  private:
     Lockable* m_lock;     
  public:
     AutoLock(Lockable* obj);
     ~AutoLock();
};

class ThreadableObject
{
  public:    
    virtual void* InThread(void* args) = 0;
    virtual bool IsImmutable()  = 0;

    virtual ~ThreadableObject() {}
};

class UniqueThreadable : public ThreadableObject
{
  public:
    virtual void* InThread(void* args) = 0;
    virtual UniqueThreadable* Clone() = 0;
    virtual UniqueThreadable* Move() = 0;

    virtual bool IsImmutable() {return false;}
};

class SharedThreadable : public ThreadableObject
{
  public:
    virtual void* InThread(void* args) = 0;

    virtual bool IsImmutable() {return true;}
};

class ThreadAPI
{
  public:
    ThreadAPI();
    virtual ~ThreadAPI();
    virtual void Start() = 0;
    virtual void Join() = 0;
  protected:
    ThreadAPIImplementation* m_Impl;
};

class ThreadWorker : public ThreadAPI
{
  public:
   ThreadWorker(SharedThreadable& obj);
   ThreadWorker(UniqueThreadable& obj);
   ThreadWorker(UniqueThreadable &&obj);
   ~ThreadWorker();

   void Start();
   void Join();

  private:
   ThreadableObject* m_Obj;

   static void* launch(void* args);
};

class ThreadPool
{ 
  public:
    ThreadPool(int n);
    ~ThreadPool();

    void AddWorker(SharedThreadable& obj);
    void AddWorker(UniqueThreadable& obj);
    void AddWorker(UniqueThreadable&& obj);
    void FillAllThreads(SharedThreadable& obj);
    void Start();
    void Join();


    int GetNumberOfWorkers() {return m_num;}

  private:
    int m_AllocNum;
    int m_num;
    ThreadAPI** m_workers;

    void Add(ThreadAPI* thread);
};
