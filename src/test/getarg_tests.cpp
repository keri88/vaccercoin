#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-VAC");
    BOOST_CHECK(GetBoolArg("-VAC"));
    BOOST_CHECK(GetBoolArg("-VAC", false));
    BOOST_CHECK(GetBoolArg("-VAC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BARo"));
    BOOST_CHECK(!GetBoolArg("-BARo", false));
    BOOST_CHECK(GetBoolArg("-BARo", true));

    ResetArgs("-VAC=0");
    BOOST_CHECK(!GetBoolArg("-VAC"));
    BOOST_CHECK(!GetBoolArg("-VAC", false));
    BOOST_CHECK(!GetBoolArg("-VAC", true));

    ResetArgs("-VAC=1");
    BOOST_CHECK(GetBoolArg("-VAC"));
    BOOST_CHECK(GetBoolArg("-VAC", false));
    BOOST_CHECK(GetBoolArg("-VAC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-VAC"));
    BOOST_CHECK(!GetBoolArg("-VAC", false));
    BOOST_CHECK(!GetBoolArg("-VAC", true));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-VAC"));
    BOOST_CHECK(!GetBoolArg("-VAC", false));
    BOOST_CHECK(!GetBoolArg("-VAC", true));

    ResetArgs("-VAC -noBAR");  // -VAC should win
    BOOST_CHECK(GetBoolArg("-VAC"));
    BOOST_CHECK(GetBoolArg("-VAC", false));
    BOOST_CHECK(GetBoolArg("-VAC", true));

    ResetArgs("-VAC=1 -noBAR=1");  // -VAC should win
    BOOST_CHECK(GetBoolArg("-VAC"));
    BOOST_CHECK(GetBoolArg("-VAC", false));
    BOOST_CHECK(GetBoolArg("-VAC", true));

    ResetArgs("-VAC=0 -noBAR=0");  // -VAC should win
    BOOST_CHECK(!GetBoolArg("-VAC"));
    BOOST_CHECK(!GetBoolArg("-VAC", false));
    BOOST_CHECK(!GetBoolArg("-VAC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--VAC=1");
    BOOST_CHECK(GetBoolArg("-VAC"));
    BOOST_CHECK(GetBoolArg("-VAC", false));
    BOOST_CHECK(GetBoolArg("-VAC", true));

    ResetArgs("--noBAR=1");
    BOOST_CHECK(!GetBoolArg("-VAC"));
    BOOST_CHECK(!GetBoolArg("-VAC", false));
    BOOST_CHECK(!GetBoolArg("-VAC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-VAC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VAC", "eleven"), "eleven");

    ResetArgs("-VAC -bar");
    BOOST_CHECK_EQUAL(GetArg("-VAC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VAC", "eleven"), "");

    ResetArgs("-VAC=");
    BOOST_CHECK_EQUAL(GetArg("-VAC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VAC", "eleven"), "");

    ResetArgs("-VAC=11");
    BOOST_CHECK_EQUAL(GetArg("-VAC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-VAC", "eleven"), "11");

    ResetArgs("-VAC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-VAC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-VAC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-VAC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-VAC", 0), 0);

    ResetArgs("-VAC -bar");
    BOOST_CHECK_EQUAL(GetArg("-VAC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-VAC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-VAC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-VAC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-VAC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--VAC");
    BOOST_CHECK_EQUAL(GetBoolArg("-VAC"), true);

    ResetArgs("--VAC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-VAC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-VAC"));
    BOOST_CHECK(!GetBoolArg("-VAC", true));
    BOOST_CHECK(!GetBoolArg("-VAC", false));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-VAC"));
    BOOST_CHECK(!GetBoolArg("-VAC", true));
    BOOST_CHECK(!GetBoolArg("-VAC", false));

    ResetArgs("-noBAR=0");
    BOOST_CHECK(GetBoolArg("-VAC"));
    BOOST_CHECK(GetBoolArg("-VAC", true));
    BOOST_CHECK(GetBoolArg("-VAC", false));

    ResetArgs("-VAC --noBAR");
    BOOST_CHECK(GetBoolArg("-VAC"));

    ResetArgs("-noBAR -VAC"); // VAC always wins:
    BOOST_CHECK(GetBoolArg("-VAC"));
}

BOOST_AUTO_TEST_SUITE_END()
