#include <iostream>

#include <time.h>
#include <unistd.h>
#include <list>

#include "threadapi.h"
#include "prng.h"
#include "account.h"
#include "mc_pi.h"

using namespace std;

void AccountManagment()
{
  Account account;

  WithDraw withdraw(account, new SimpleAccountStrategy(5,50));

  ThreadPool pool(2);
  pool.AddWorker(Deposit(account, new SimpleAccountStrategy(30,10)));
  pool.AddWorker(withdraw);

  pool.Start();
  pool.Join();

  cout << "Balance :: " << account.GetBalance() << endl;
}

void CalcPIbyMC()
{
  MC_Number mcPI;
  mcPI.Data = 0.0f;
  int iterNumber = 100000000;
  int threadsNumber = 6;

  MonteCarloPICalculator calculator(mcPI, iterNumber);

  ThreadPool pool(threadsNumber);

  pool.FillAllThreads(calculator);

  pool.Start();
  pool.Join();

  cout << "PI :: " << 4.0 * (mcPI.Data / (pool.GetNumberOfWorkers() * iterNumber) ) << endl;

}

int main()
{    
    AccountManagment();
    CalcPIbyMC();

    return 0;
}


