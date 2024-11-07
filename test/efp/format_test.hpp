#ifndef FORMAT_TEST_HPP_
#define FORMAT_TEST_HPP_

#include "catch2/catch_test_macros.hpp"

#include "efp/format.hpp"
#include "test_common.hpp"

using namespace efp;

// todo Find the reason why std::format is also visible
TEST_CASE("format", "[format]") {
    SECTION("fundamental types") {
        CHECK(efp::format("{}", 42) == "42");

        CHECK(efp::format("{}", 42.0) == "42");
        CHECK(efp::format("{}", 42.1) == "42.1");

        CHECK(efp::format("{}", "hello") == "hello");
        CHECK(efp::format("{}", 'c') == "c");

        CHECK(efp::format("{}", 42u) == "42");
        CHECK(efp::format("{}", 42l) == "42");
        CHECK(efp::format("{}", 42ll) == "42");

        CHECK(efp::format("{}", 42.0f) == "42");
        CHECK(efp::format("{}", 42.1f) == "42.1");

        CHECK(efp::format("{}", 42.0l) == "42");
        CHECK(efp::format("{}", 42.1l) == "42.1");

        // format specifiers
        CHECK(efp::format("{:d}", 42) == "42");
        CHECK(efp::format("{:f}", 42.0) == "42.000000");
        CHECK(efp::format("{:f}", 42.1) == "42.100000");
        CHECK(efp::format("{:s}", "hello") == "hello");
        CHECK(efp::format("{:c}", 'c') == "c");
        // CHECK(efp::format("{:u}", 42u) == "42");
        // CHECK(efp::format("{:l}", 42l) == "42");
        // CHECK(efp::format("{:ll}", 42ll) == "42");
        CHECK(efp::format("{:f}", 42.0f) == "42.000000");
        CHECK(efp::format("{:f}", 42.1f) == "42.099998");
        CHECK(efp::format("{:f}", 42.0l) == "42.000000");
        CHECK(efp::format("{:f}", 42.1l) == "42.100000");
    }

    SECTION("efp::Array") {
        CHECK(efp::format("{}", array_3) == "[1, 2, 3]");
        CHECK(efp::format("{}", array_5) == "[1, 2, 3, 4, 5]");
    }

    SECTION("efp::ArrVec") {
        CHECK(efp::format("{}", arrvec_3) == "[1, 2, 3]");
        CHECK(efp::format("{}", arrvec_5) == "[1, 2, 3, 4, 5]");
    }

    SECTION("efp::Vector") {
        CHECK(efp::format("{}", vector_3) == "[1, 2, 3]");
        CHECK(efp::format("{}", vector_5) == "[1, 2, 3, 4, 5]");
    }

    SECTION("efp::String") {
        CHECK(efp::format("{}", String("hello")) == "hello");
    }

    SECTION("efp::Vcb") {
        Vcb<int, 3> vcb{};
        vcb.push_back(1);
        vcb.push_back(2);
        vcb.push_back(3);
        CHECK(efp::format("{}", vcb) == "[1, 2, 3]");
    }

    SECTION("efp::Vcq") {
        Vcq<int, 3> vcq{};
        vcq.push_back(1);
        vcq.push_back(2);
        CHECK(efp::format("{}", vcq) == "[1, 2]");
    }
}

#endif