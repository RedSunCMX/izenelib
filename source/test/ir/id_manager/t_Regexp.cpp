#include <boost/test/unit_test.hpp>
#include "IDManagerTestUtil.h"

using namespace boost::unit_test;

BOOST_FIXTURE_TEST_SUITE( t_Regexp, IDManagerFixture )

typedef _IDManager< UString, uint32_t, izenelib::util::NullLock,
                    EmptyRegExpHandler<UString, uint32_t>,
                    HashIDGenerator<UString, uint32_t>,
                    SDBIDStorage<UString, uint32_t>,
                    UniqueIDGenerator<UString, uint32_t>,
                    SDBIDStorage<UString, uint32_t> >
IDManagerEmptyRegexpHandler;

typedef _IDManager< UString, uint32_t, izenelib::util::NullLock,
                    MemoryRegExpHandler<UString, uint32_t>,
                    HashIDGenerator<UString, uint32_t>,
                    SDBIDStorage<UString, uint32_t>,
                    UniqueIDGenerator<UString, uint32_t>,
                    SDBIDStorage<UString, uint32_t> >
IDManagerMemoryRegexpHandler;

typedef _IDManager< UString, uint32_t, izenelib::util::NullLock,
                    DiskRegExpHandler<UString, uint32_t>,
                    HashIDGenerator<UString, uint32_t>,
                    SDBIDStorage<UString, uint32_t>,
                    UniqueIDGenerator<UString, uint32_t>,
                    SDBIDStorage<UString, uint32_t> >
IDManagerDiskRegexpHandler;

BOOST_AUTO_TEST_CASE( EmptyRegexpHandler )
{
    IDManagerEmptyRegexpHandler idManager("regexp1");
    UString compare;

    // Build term index dictionary using getTermIdListByTermStringList() Interface.
    termIdList1_.resize(termUStringList1_.size());
    termIdList2_.resize(termUStringList2_.size());
    idManager.getTermIdListByTermStringList( termUStringList1_, termIdList1_ );
    idManager.getTermIdListByTermStringList( termUStringList2_, termIdList2_ );

    idManager.startWildcardProcess();
    idManager.joinWildcardProcess();

    // Get term id list using getTermIdListByWildcardPattern() Interface with pattern string "ate".
    termIdList1_.clear();

    std::string patternSource("ad");
    UString patternCheck(patternSource, UString::CP949);
    patternSource = "*";
    UString starChar(patternSource, UString::CP949);
    UString pattern;

    // ---------------------------------------------- pattern = "ad*"
    pattern.clear();
    pattern = patternCheck;
    pattern += starChar;
    idManager.getTermIdListByWildcardPattern(pattern, termIdList1_);
    BOOST_CHECK_EQUAL( termIdList1_.size() , static_cast<unsigned int>(0) );

    // ---------------------------------------------- pattern = "*ad"
    pattern.clear();
    termIdList1_.clear();
    pattern = starChar;
    pattern += patternCheck;

    idManager.getTermIdListByWildcardPattern(pattern, termIdList1_);
    BOOST_CHECK_EQUAL( termIdList1_.size() , static_cast<unsigned int>(0) );

    // ---------------------------------------------- pattern = "*ad*"
    pattern.clear();
    termIdList1_.clear();
    pattern += starChar;
    pattern += patternCheck;
    pattern += starChar;
    idManager.getTermIdListByWildcardPattern(pattern, termIdList1_);
    BOOST_CHECK_EQUAL( termIdList1_.size() , static_cast<unsigned int>(0));

}

BOOST_AUTO_TEST_CASE( MemoryRegexpHandler )
{
    IDManagerMemoryRegexpHandler idManager("regexp2");
    UString compare;

    // Build term index dictionary using getTermIdListByTermStringList() Interface.
    termIdList1_.resize(termUStringList1_.size());
    termIdList2_.resize(termUStringList2_.size());
    idManager.getTermIdListByTermStringList( termUStringList1_, termIdList1_ );
    idManager.getTermIdListByTermStringList( termUStringList2_, termIdList2_ );

    idManager.startWildcardProcess();
    idManager.joinWildcardProcess();

    // Get term id list using getTermIdListByWildcardPattern() Interface with pattern string "ate".
    termIdList1_.clear();

    std::string patternSource("ad");
    UString patternCheck(patternSource, UString::CP949);
    patternSource = "*";
    UString starChar(patternSource, UString::CP949);
    UString pattern;

    // ---------------------------------------------- pattern = "ad*"
    pattern.clear();
    pattern = patternCheck;
    pattern += starChar;
    idManager.getTermIdListByWildcardPattern(pattern, termIdList1_);
    BOOST_CHECK_EQUAL( termIdList1_.size() , static_cast<unsigned int>(35) );

    // ---------------------------------------------- pattern = "*ad"
    pattern.clear();
    termIdList1_.clear();
    pattern = starChar;
    pattern += patternCheck;

    idManager.getTermIdListByWildcardPattern(pattern, termIdList1_);
    BOOST_CHECK_EQUAL( termIdList1_.size() , static_cast<unsigned int>(4) );

    // ---------------------------------------------- pattern = "*ad*"
    pattern.clear();
    termIdList1_.clear();
    pattern += starChar;
    pattern += patternCheck;
    pattern += starChar;
    idManager.getTermIdListByWildcardPattern(pattern, termIdList1_);
    BOOST_CHECK_EQUAL( termIdList1_.size() , static_cast<unsigned int>(81));

} // end - BOOST_AUTO_TEST_CASE( TestCase3 )

BOOST_AUTO_TEST_CASE( DiskRegexpHandler )
{
    IDManagerDiskRegexpHandler idManager("regexp3");
    UString compare;

    // Build term index dictionary using getTermIdListByTermStringList() Interface.
    termIdList1_.resize(termUStringList1_.size());
    termIdList2_.resize(termUStringList2_.size());
    idManager.getTermIdListByTermStringList( termUStringList1_, termIdList1_ );
    idManager.getTermIdListByTermStringList( termUStringList2_, termIdList2_ );

    idManager.startWildcardProcess();
    idManager.joinWildcardProcess();

    // Get term id list using getTermIdListByWildcardPattern() Interface with pattern string "ate".
    termIdList1_.clear();

    std::string patternSource("ad");
    UString patternCheck(patternSource, UString::CP949);
    patternSource = "*";
    UString starChar(patternSource, UString::CP949);
    UString pattern;

    // ---------------------------------------------- pattern = "ad*"
    pattern.clear();
    pattern = patternCheck;
    pattern += starChar;
    idManager.getTermIdListByWildcardPattern(pattern, termIdList1_);
    BOOST_CHECK_EQUAL( termIdList1_.size() , static_cast<unsigned int>(35) );

    // ---------------------------------------------- pattern = "*ad"
    pattern.clear();
    termIdList1_.clear();
    pattern = starChar;
    pattern += patternCheck;

    idManager.getTermIdListByWildcardPattern(pattern, termIdList1_);
    BOOST_CHECK_EQUAL( termIdList1_.size() , static_cast<unsigned int>(4) );

    // ---------------------------------------------- pattern = "*ad*"
    pattern.clear();
    termIdList1_.clear();
    pattern += starChar;
    pattern += patternCheck;
    pattern += starChar;
    idManager.getTermIdListByWildcardPattern(pattern, termIdList1_);
    BOOST_CHECK_EQUAL( termIdList1_.size() , static_cast<unsigned int>(81));

} // end - BOOST_AUTO_TEST_CASE( TestCase3 )

BOOST_AUTO_TEST_SUITE_END()
