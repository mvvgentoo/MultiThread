#include <stdint.h>
#include "prng.h"

#ifdef TEST_MODULE
  #include <iostream>
  using std::cout;
  using std::endl;
#endif

template <typename FloatValueType, typename intValueType>
class ConverterCompability
{
    enum{ value = false };
};

template <typename FloatValueType, typename intValueType>
class Converter
{
    public:
        Converter()
        {
            static_assert(ConverterCompability<FloatValueType, intValueType>::value, "Pair of this types is unsupported");
        }

        static FloatValueType toUniRange(intValueType val)
        {
            static_assert(ConverterCompability<FloatValueType, intValueType>::value, "Pair of this types is unsupported");
        }
};

template <>
class Converter<float, uint32_t>
{
	public:
		static float toUniRange(uint32_t val)
		{
			const union {float f; uint32_t i;} u = { .i = UINT32_C(0x7f) << 23 | val >> 9};
			return u.f - 1.0f;
		}
};

template <>
class Converter<double, uint64_t>
{
	public:	
		static double toUniRange(uint64_t val)
		{
			const union {double d; uint64_t i;} u = { .i = UINT64_C(0x3ff) << 52 | val >> 12};
			return u.d - 1.0f;
		}
};

template <>
class Converter<double, uint32_t>
{
	public:	
		static double toUniRange(uint32_t val)
		{
			return static_cast<double>(Converter<float,uint32_t>::toUniRange(val));
		}
};


const XorShift::IntValueType XorShift::ms_maxIVal = 0xffffffff;
const XorShift::IntValueType XorShift::ms_invMax = 1.0 / ms_maxIVal;

XorShift::XorShift(XorShift::IntValueType x, XorShift::IntValueType y, XorShift::IntValueType z, XorShift::IntValueType w) :
  m_x(x), m_y(y), m_z(z), m_w(w)
{

}

XorShift::FloatValueType XorShift::GetRandomInRange(XorShift::FloatValueType low, XorShift::FloatValueType high)
{
  return low + (high - low) * GetRandomInRangeFast();
}

XorShift::FloatValueType XorShift::GetRandomInRangeFast()
{
  return Converter<FloatValueType, IntValueType>::toUniRange( GetRandomInteger() );
}

XorShift::IntValueType XorShift::GetRandomInteger()
{

  uint32_t t = m_x;
  t ^= t << 11;
  t ^= t >> 8;
  m_x = m_y;
  m_y = m_z;
  m_z = m_w;
  m_w ^= m_w >> 19;
  m_w ^= t;
  return m_w;
}

XorShift::IntValueType XorShift::GetRandomNumberUpperLimit()
{
  return ms_maxIVal;
}

#ifdef TEST_MODULE
int main()
{
    XorShift sh(123133, 112481, 98723441, 8978992);

    for (int i = 0; i < 1000; i++)
        cout << sh.GetRandomInRangeFast() << endl;

    return 0;
}
#endif
