#pragma once

#include "threadapi.h"

class Account
{
  public:
    typedef int DataType;

    Account() {m_Account.Data = 0;}
    void WithDraw(int amount);
    void Deposit(int amount);
    int GetBalance() {return m_Account.Data;}

  private:
    ThreadResource<DataType> m_Account;
};

class AccountStrategy
{
  public:
    virtual bool IsValid() = 0;
    virtual Account::DataType GetAmount() = 0;
    void Destroy() {delete this;}

    virtual AccountStrategy* Clone() = 0;

    static void SafeDelete(AccountStrategy* strategy);

  protected:
    virtual ~AccountStrategy() {}
};

class SimpleAccountStrategy : public AccountStrategy
{
  public:
    SimpleAccountStrategy(int iterations, int amount);
    virtual bool IsValid();
    virtual Account::DataType GetAmount();

    virtual SimpleAccountStrategy* Clone();

  protected:
    ~SimpleAccountStrategy()
    {

    }
  private:
    int m_Counter;
    int m_IterationLimit;
    int m_Amount;
};

class Deposit : public UniqueThreadable
{
  public:

    Deposit(Account& account, AccountStrategy* const &&  strategy);
    Deposit(Account& account, AccountStrategy*& strategy);
    ~Deposit();
    void* InThread(void* args);
    Deposit* Clone();
    Deposit* Move();

  private:
    Account& m_Account;
    AccountStrategy* m_Strategy;

    Deposit(Deposit&& obj);
};

class WithDraw : public UniqueThreadable
{
  public:

    WithDraw(Account& account, AccountStrategy* const &&  strategy);
    WithDraw(Account& account, AccountStrategy*& strategy);
    ~WithDraw();

    void* InThread(void* args);
    WithDraw* Clone();
    WithDraw* Move();

  private:
    Account& m_Account;
    AccountStrategy* m_Strategy;

    WithDraw(WithDraw&& obj);
};
