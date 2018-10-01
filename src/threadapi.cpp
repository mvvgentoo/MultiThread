#include "threadapi.h"

#include <assert.h>
#include <unistd.h>

#define PTHREAD (HAVE_PTHREAD & USE_PTHREAD)

#if PTHREAD
#include "pthread_impl.hpp"
#else
#include "cthread_impl.hpp"
#endif

static ThreadAPIFactoryImplementation* UseProperThreadImplementation()
{
#ifdef PTHREAD
  return new PThreadFactory();
#else
  return new CThreadFactory();
#endif
}

std::unique_ptr<ThreadAPIFactoryImplementation> ThreadAPIFactory::m_Impl = nullptr;

LockableImplementation *ThreadAPIFactory::CreateLockable()
{
  if(m_Impl == nullptr)
    m_Impl.reset(UseProperThreadImplementation() );

  return m_Impl->CreateLockable();
}

ThreadAPIImplementation *ThreadAPIFactory::CreateThreadAPIImplementation()
{
  if(m_Impl == nullptr)
    m_Impl.reset(UseProperThreadImplementation() );

  return m_Impl->CreateThreadAPIImplementation();
}

Lockable::Lockable()
{
  m_Impl = nullptr;
  m_Impl = ThreadAPIFactory::CreateLockable();
  assert(m_Impl != nullptr);
}

Lockable::~Lockable()
{
  m_Impl->Destroy();
  delete m_Impl;
}

void Lockable::Lock()
{
  m_Impl->Lock();
}
void Lockable::Unlock()
{
  m_Impl->Unlock();
}

void Lockable::Wait()
{
  m_Impl->Wait();
}

void Lockable::WakeUp()
{
  m_Impl->WakeUp();
}


ThreadAPI::ThreadAPI()
{
  m_Impl = nullptr;
  m_Impl = ThreadAPIFactory::CreateThreadAPIImplementation();
  assert(m_Impl != nullptr);
}

ThreadAPI::~ThreadAPI()
{
  delete m_Impl;
}

AutoLock::AutoLock(Lockable *obj)
{
  m_lock = obj;
  m_lock->Lock();
}

AutoLock::~AutoLock()
{
  m_lock->Unlock();
}

ThreadWorker::ThreadWorker(SharedThreadable &obj) : m_Obj(&obj)
{

}

ThreadWorker::ThreadWorker(UniqueThreadable &obj) : m_Obj(obj.Clone())
{

}

ThreadWorker::ThreadWorker(UniqueThreadable &&obj) : m_Obj(obj.Move())
{

}

ThreadWorker::~ThreadWorker()
{
  if(!m_Obj->IsImmutable())
  {
    delete m_Obj;
  }
}

void ThreadWorker::Start()
{
  void* args = (void*)(m_Obj);
  m_Impl->CreateThread(ThreadWorker::launch, args);
}

void ThreadWorker::Join()
{
  m_Impl->Join();
}

void *ThreadWorker::launch(void *args)
{
  ThreadableObject* obj = (ThreadableObject*)(args);
  return obj->InThread(args);
}

ThreadPool::ThreadPool(int n) :m_AllocNum(n),  m_num(0), m_workers(NULL)
{
  m_workers = new ThreadAPI*[m_AllocNum];
  for(int i = 0; i < m_AllocNum; i++)
  {
    m_workers[i] = NULL;
  }
}

ThreadPool::~ThreadPool()
{
  for(int i = 0; i < m_num; i++)
  {
    delete m_workers[i];
  }

  delete m_workers;
}

void ThreadPool::AddWorker(UniqueThreadable &obj)
{
  Add(new ThreadWorker(obj));
}

void ThreadPool::AddWorker(UniqueThreadable &&obj)
{
  Add(new ThreadWorker(std::move(obj)));
}

void ThreadPool::FillAllThreads(SharedThreadable &obj)
{
  for(int i = 0; i < m_AllocNum; i++)
  {
     Add(new ThreadWorker(obj));
  }
}

void ThreadPool::AddWorker(SharedThreadable &obj)
{
  Add(new ThreadWorker(obj));
}


void ThreadPool::Start()
{
  for(int i = 0; i < m_num; i++)
  {
    m_workers[i]->Start();
  }
}

void ThreadPool::Join()
{
  for(int i = 0; i < m_num; i++)
  {
    m_workers[i]->Join();
  }
}

void ThreadPool::Add(ThreadAPI *thread)
{
  if(m_num < m_AllocNum)
  {
    m_workers[m_num] = thread;
    ++m_num;
  }
}


