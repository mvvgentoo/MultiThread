#include "mc_pi.h"

#include <time.h>
#include <thread>

#include "prng.h"

MonteCarloPICalculator::MonteCarloPICalculator(MC_Number &number, int maxIterations) : m_localPI(number), m_maxIterations(maxIterations)
{

}

void *MonteCarloPICalculator::InThread(void *args)
{
  double x(0.0), y(0.0);
  int counter(0), localCounter(0);

  auto pid=std::this_thread::get_id();  
  uint32_t* ptr=(uint32_t*) &pid;

  int id = (*ptr) % XorShift::GetMaximalIntValue();
  int timing = time(0) % XorShift::GetMaximalIntValue();

  int xs[4] = { timing, id, 787823 + 3 * id, 8978992 + (id + timing)};
  XorShift rnd(xs[0], xs[1], xs[2], xs[3]);

  while(counter < m_maxIterations)
  {
    x = rnd.GetRandomInRangeFast();
    y = rnd.GetRandomInRangeFast();

    if( !( x*x + y*y > 1.0f) )
    {
      ++localCounter;
    }

    ++counter;
  }

  AutoLock lock(&m_localPI);

  m_localPI.Data += localCounter;

  return NULL;
}
