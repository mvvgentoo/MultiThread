#pragma once


template <typename _FloatValueType = double, typename _IntValueType = uint32_t>
class IRandomGenerator
{
    public:
        typedef _FloatValueType FloatValueType;
        typedef _IntValueType IntValueType;

        // return random in range [low,high]
        virtual FloatValueType GetRandomInRange(FloatValueType low, FloatValueType high) = 0;
        // return random in range [0,1]
        virtual	FloatValueType GetRandomInRangeFast() = 0;
        // return integer random in range [0,MAX(ival)]
        virtual IntValueType GetRandomInteger() = 0;
        //auxullary functions
        virtual IntValueType GetRandomNumberUpperLimit() = 0;
};

typedef IRandomGenerator<double,uint32_t> ClassicPRNG;

class XorShift : public ClassicPRNG
{
    public:

        typedef ClassicPRNG::FloatValueType FloatValueType;
        typedef ClassicPRNG::IntValueType IntValueType;

        XorShift( IntValueType x, IntValueType y, IntValueType z, IntValueType w);
        virtual FloatValueType GetRandomInRange(FloatValueType low, FloatValueType high);
        virtual	FloatValueType GetRandomInRangeFast();
        virtual IntValueType GetRandomInteger();
        virtual IntValueType GetRandomNumberUpperLimit();

        static IntValueType GetMaximalIntValue() {return ms_maxIVal;}

    private:
        union
        {
            IntValueType m_states[4];
            struct
            {
                IntValueType m_x,m_y,m_z,m_w;
            };
        };

        static const IntValueType ms_maxIVal; // = 0xffffffff;
        static const IntValueType ms_invMax; // = 1.0f / ms_maxIVal;
};
