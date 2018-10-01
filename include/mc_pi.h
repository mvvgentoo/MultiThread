#pragma once

#include "threadapi.h"

typedef ThreadResource<double> MC_Number;

class MonteCarloPICalculator : public SharedThreadable
{
  public:
    MonteCarloPICalculator(MC_Number& number, int maxIterations);

    void* InThread(void *args);

  private:
    MC_Number& m_localPI;
    const int m_maxIterations;
};
