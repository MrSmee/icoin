//
// Unit tests for block-chain checkpoints
//
#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

#include "../checkpoints.h"
#include "../util.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(Checkpoints_tests)

BOOST_AUTO_TEST_CASE(sanity)
{
    uint256 p1000 = uint256("0xd7340ba3046b16eacef10ceec5de73e8f3ff55c33e33fff6b6d4e8bc311b07fc");
    uint256 p5199 = uint256("0x6ce060aa8c0fa463dc360ee1e193227151f20ebeb0a94ad85f67c8094961caeb");
    BOOST_CHECK(Checkpoints::CheckBlock(1000, p1000));
    BOOST_CHECK(Checkpoints::CheckBlock(5199, p5199));

    
    // Wrong hashes at checkpoints should fail:
    BOOST_CHECK(!Checkpoints::CheckBlock(1000, p5199));
    BOOST_CHECK(!Checkpoints::CheckBlock(5199, p1000));

    // ... but any hash not at a checkpoint should succeed:
    BOOST_CHECK(Checkpoints::CheckBlock(1000+1, p5199));
    BOOST_CHECK(Checkpoints::CheckBlock(5199+1, p1000));

    BOOST_CHECK(Checkpoints::GetTotalBlocksEstimate() >= 5199);
}    

BOOST_AUTO_TEST_SUITE_END()
