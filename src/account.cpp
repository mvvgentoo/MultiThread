#include "account.h"
#include "threadapi.h"
#include "prng.h"

#include <assert.h>

#include <iostream>
using std::cout;
using std::endl;

void Account::WithDraw(int amount)
{
  AutoLock lock(&m_Account);

  while(m_Account.Data < amount)
  {
     cout << "Not enough resources" << endl;
     m_Account.Wait();
  }

  {
    m_Account.Data -= amount;
    cout << "WithDraw:: Account now = " << m_Account.Data << endl;
  }
}

void Account::Deposit(int amount)
{
  AutoLock lock(&m_Account);

  m_Account.Data += amount;
  cout << "Deposit:: Account now = " << m_Account.Data << endl;

  m_Account.WakeUp();
}

Deposit::Deposit(Account &account, AccountStrategy * const &&strategy) :
  m_Account(account), m_Strategy(strategy)
{
  assert(m_Strategy!=nullptr);
}

Deposit::Deposit(Account &account, AccountStrategy *&strategy) :
  m_Account(account), m_Strategy(nullptr)
{
  m_Strategy = strategy;
  strategy = nullptr;

  assert(m_Strategy!=nullptr);
}

Deposit::~Deposit()
{
  AccountStrategy::SafeDelete(m_Strategy);
}

void *Deposit::InThread(void *args)
{
  while(m_Strategy->IsValid() )
  {
    m_Account.Deposit(m_Strategy->GetAmount() );
  }

  cout << "ended Deposit" << endl;

  return NULL;
}

Deposit *Deposit::Clone()
{ 
  return new Deposit(m_Account, m_Strategy->Clone() );
}

Deposit *Deposit::Move()
{ 
  return new Deposit(std::move(*this));
}

Deposit::Deposit(Deposit &&obj) : m_Account(obj.m_Account), m_Strategy(obj.m_Strategy)
{
  obj.m_Strategy = nullptr;
}

WithDraw::WithDraw(Account &account, AccountStrategy * const &&strategy) :
  m_Account(account), m_Strategy(strategy)
{
  assert(m_Strategy!=nullptr);
}

WithDraw::WithDraw(Account &account, AccountStrategy *&strategy) :
   m_Account(account), m_Strategy(nullptr)
{
  m_Strategy = strategy;
  strategy = nullptr;

  assert(m_Strategy!=nullptr);
}

WithDraw::~WithDraw()
{
  AccountStrategy::SafeDelete(m_Strategy);
}

void *WithDraw::InThread(void *args)
{
  while(m_Strategy->IsValid() )
  {
    m_Account.WithDraw(m_Strategy->GetAmount());
  }

  cout << "ended WithDraw" << endl;

  return NULL;
}

WithDraw *WithDraw::Clone()
{ 
  return new WithDraw(m_Account, m_Strategy->Clone());
}

WithDraw *WithDraw::Move()
{
  return new WithDraw(std::move(*this));
}

WithDraw::WithDraw(WithDraw &&obj) : m_Account(obj.m_Account), m_Strategy(obj.m_Strategy)
{
  obj.m_Strategy = nullptr;
}

SimpleAccountStrategy::SimpleAccountStrategy(int iterations, int amount) : m_Counter(0), m_IterationLimit(iterations), m_Amount(amount)
{

}

bool SimpleAccountStrategy::IsValid()
{
  return m_Counter++ < m_IterationLimit;
}

Account::DataType SimpleAccountStrategy::GetAmount()
{
  return m_Amount;
}

SimpleAccountStrategy *SimpleAccountStrategy::Clone()
{
  return new SimpleAccountStrategy(m_IterationLimit, m_Amount);
}


void AccountStrategy::SafeDelete(AccountStrategy *strategy)
{
  if(strategy)
    strategy->Destroy();
}
