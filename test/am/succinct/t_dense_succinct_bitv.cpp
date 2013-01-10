#include <boost/test/unit_test.hpp>

#include <am/succinct/dbitv/SuccinctBitVector.hpp>

using namespace std;

#define BITV_SZ 262144LL

BOOST_AUTO_TEST_SUITE( t_dense_succinct_bitv_suite )

BOOST_AUTO_TEST_CASE(rank)
{
    izenelib::am::succinct::dense::SuccinctBitVector bv;
    bv.init(BITV_SZ);
    for (uint64_t i = 0; i < BITV_SZ; i++)
        if (i % 2 == 0) bv.set_bit(i);
    bv.build();

    uint32_t nrank0 = 0;
    uint32_t nrank1 = 0;

    for (int i = 0; i < BITV_SZ; i++) {
        BOOST_CHECK_EQUAL(nrank0, bv.rank0(i));
        BOOST_CHECK_EQUAL(nrank1, bv.rank1(i));

        if (bv.lookup(i))
            nrank1++;
        else
          nrank0++;
    }
}

BOOST_AUTO_TEST_CASE(select)
{
    izenelib::am::succinct::dense::SuccinctBitVector bv;
    bv.init(BITV_SZ);
    for (uint64_t i = 0; i < BITV_SZ; i++)
        if (i % 2 == 0) bv.set_bit(i);
    bv.build();

    uint32_t nrank0 = 0;
    uint32_t nrank1 = 0;

    for (int i = 0; i < BITV_SZ; i++)
    {
        if (bv.lookup(i))
        {
//          BOOST_CHECK_EQUAL(i, bv.select1(nrank1));
            nrank1++;
        }
        else
        {
//          BOOST_CHECK_EQUAL(i, bv.select0(nrank0));
            nrank0++;
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
