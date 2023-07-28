#ifndef VCQ_TEST_HPP_
#define VCQ_TEST_HPP_

#include "efp.hpp"

using namespace efp;

TEST_CASE("Vcq")
{
    SECTION("Array")
    {
        Vcq<int, 3> vcq;
        CHECK(vcq.size() == 0);
        CHECK(vcq.is_empty() == true);
        vcq.push_back(1);
        CHECK(vcq.size() == 1);
        CHECK(vcq.is_empty() == false);
        CHECK(vcq.pop_front() == 1);
        vcq.push_back(2);
        vcq.push_back(3);
        vcq.push_back(4);
        CHECK(vcq.size() == 3);
        CHECK(vcq.pop_front() == 2);
        CHECK(vcq.pop_front() == 3);
        CHECK(vcq.pop_front() == 4);
        vcq.push_back(5);
        vcq.push_back(6);
        vcq.push_back(7);
        vcq.push_back(8);
        CHECK(vcq.size() == 3);
        CHECK(vcq.begin()[0] == 6);
        CHECK(vcq.begin()[1] == 7);
        CHECK(vcq.begin()[2] == 8);
        CHECK(vcq.pop_front() == 6);
        CHECK(vcq.pop_front() == 7);
        CHECK(vcq.pop_front() == 8);
    }
}

#endif