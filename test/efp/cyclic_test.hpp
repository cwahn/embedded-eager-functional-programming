#ifndef CYCLIC_TEST_HPP_
#define CYCLIC_TEST_HPP_

#include "efp.hpp"
#include "test_common.hpp"

using namespace efp;

TEST_CASE("Vcb Rule of 5", "Vcb") {
    // use MockHW and MockRaii to check if the rule of 5 is followed
    SECTION("New Construction", "Vcb") {
        SECTION("Trivially Copiable") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable") {
            {
                MockHW::reset();
                Vcb<MockRaii, 3> vcb;
                CHECK(MockHW::remaining_resource_count() == 6);
                // CHECK(MockHW::resource_state_to_string() == "1, 2, 3, 4, 5, 6");
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Copy Construction", "Vcb") {
        SECTION("Trivially Copiable") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable") {
            {
                MockHW::reset();
                Vcb<MockRaii, 3> vcb;
                CHECK(MockHW::remaining_resource_count() == 6);
                CHECK(MockHW::resource_state_to_int() == 123456);

                Vcb<MockRaii, 3> vcb_copy = vcb;
                CHECK(MockHW::remaining_resource_count() == 12);
                // CHECK(MockHW::resource_state_to_string() == "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12");
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Copy Assignment", "Vcb") {
        SECTION("Trivially Copiable") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable") {
            {
                MockHW::reset();
                Vcb<MockRaii, 3> vcb;
                CHECK(MockHW::remaining_resource_count() == 6);

                Vcb<MockRaii, 3> vcb_copy;
                CHECK(MockHW::remaining_resource_count() == 12);

                vcb_copy = vcb;
                CHECK(MockHW::remaining_resource_count() == 12);
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Move Construction", "Vcb") {
        SECTION("Trivially Copiable") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable") {
            {
                MockHW::reset();
                Vcb<MockRaii, 3> vcb;
                CHECK(MockHW::remaining_resource_count() == 6);

                Vcb<MockRaii, 3> vcb_move = efp::move(vcb);
                CHECK(MockHW::remaining_resource_count() == 6);
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Move Assignment", "Vcb") {
        SECTION("Trivially Copiable") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable") {
            {
                MockHW::reset();
                Vcb<MockRaii, 3> vcb;
                CHECK(MockHW::remaining_resource_count() == 6);

                Vcb<MockRaii, 3> vcb_move;
                CHECK(MockHW::remaining_resource_count() == 12);

                vcb_move = efp::move(vcb);
                CHECK(MockHW::remaining_resource_count() == 6);
            }
            CHECK(MockHW::is_sound());
        }
    }
}

TEST_CASE("Vcb") {

    SECTION("Initializer List Construction", "Vcb") {
        const Vcb<int, 3> vcb{1, 2, 3};
        CHECK(vcb[0] == 1);
        CHECK(vcb[1] == 2);
        CHECK(vcb[2] == 3);
    }

    SECTION("0") {
        Vcb<int, 3> vcb;
        CHECK(IsSame<Element<decltype(vcb)>, int>::value == true);
        CHECK(vcb.size() == 3);
        CHECK(length(vcb) == 3);
        CHECK(IsCtConst<decltype(length(vcb))>::value == true);
        // DebugType<decltype(length(vcb))>{};
        CHECK(vcb.empty() == false);

        vcb.push_back(1);
        vcb.push_back(2);
        vcb.push_back(3);
        vcb.push_back(4);

        CHECK(vcb[0] == 2);
        CHECK(vcb[1] == 3);
        CHECK(vcb[2] == 4);
    }
}

TEST_CASE("Vcq Rule of 5", "Vcq") {
    // use MockHW and MockRaii to check if the rule of 5 is followed
    SECTION("New Construction", "Vcq") {
        SECTION("Trivially Copiable") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable") {
            {
                MockHW::reset();
                Vcq<MockRaii, 3> vcq;
                CHECK(MockHW::remaining_resource_count() == 0);
                // CHECK(MockHW::resource_state_to_string() == "1, 2, 3, 4, 5, 6");
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Copy Construction", "Vcq") {
        SECTION("Trivially Copiable") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable") {
            {
                MockHW::reset();
                Vcq<MockRaii, 3> vcq;
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});

                Vcq<MockRaii, 3> vcq_copy = vcq;
            }
            CHECK(MockHW::is_sound());
        }

        SECTION("Trivially Copiable Full") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable Full") {
            {
                MockHW::reset();
                Vcq<MockRaii, 3> vcq;
                vcq.push_back(MockRaii{});
                // CHECK(MockHW::resource_state_to_string() == "");
                vcq.push_back(MockRaii{});
                // CHECK(MockHW::resource_state_to_string() == "");
                vcq.push_back(MockRaii{});
                // CHECK(MockHW::resource_state_to_string() == "");
                vcq.push_back(MockRaii{});
                // CHECK(MockHW::resource_state_to_string() == "");

                Vcq<MockRaii, 3> vcq_copy = vcq;
                CHECK(vcq.size() == 3);
                // CHECK(MockHW::resource_state_to_string() == "");
            }
            CHECK(MockHW::is_sound());
            // CHECK(MockHW::resource_state_to_string() == "");
        }
    }

    SECTION("Copy Assignment", "Vcq") {
        SECTION("Trivially Copiable") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable") {
            {
                MockHW::reset();
                Vcq<MockRaii, 3> vcq;
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});

                Vcq<MockRaii, 3> vcq_copy;

                vcq_copy = vcq;
            }
            CHECK(MockHW::is_sound());
        }

        SECTION("Trivially Copiable Full") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable Full") {
            {
                MockHW::reset();
                Vcq<MockRaii, 3> vcq;
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});

                Vcq<MockRaii, 3> vcq_copy;

                vcq_copy = vcq;
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Move Construction", "Vcq") {
        SECTION("Trivially Copiable") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable") {
            {
                MockHW::reset();
                Vcq<MockRaii, 3> vcq;
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});

                Vcq<MockRaii, 3> vcq_move = efp::move(vcq);
            }
            CHECK(MockHW::is_sound());
        }

        SECTION("Trivially Copiable Full") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable Full") {
            {
                MockHW::reset();
                Vcq<MockRaii, 3> vcq;
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});

                Vcq<MockRaii, 3> vcq_move = efp::move(vcq);
            }
            CHECK(MockHW::is_sound());
        }
    }

    SECTION("Move Assignment", "Vcq") {
        SECTION("Trivially Copiable") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable") {
            {
                MockHW::reset();
                Vcq<MockRaii, 3> vcq;
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});

                Vcq<MockRaii, 3> vcq_move;

                vcq_move = efp::move(vcq);
            }
            CHECK(MockHW::is_sound());
        }

        SECTION("Trivially Copiable Full") {
            // todo with int
        }
        SECTION("Non-Trivially Copiable Full") {
            {
                MockHW::reset();
                Vcq<MockRaii, 3> vcq;
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});
                vcq.push_back(MockRaii{});

                Vcq<MockRaii, 3> vcq_move;

                vcq_move = efp::move(vcq);
            }
            CHECK(MockHW::is_sound());
        }
    }
}

TEST_CASE("Vcq") {
    SECTION("Initializer List Construction", "Vcq") {
        const Vcq<int, 3> vcq{1, 2, 3};
        CHECK(vcq[0] == 1);
        CHECK(vcq[1] == 2);
        CHECK(vcq[2] == 3);
    }

    SECTION("Array") {
        Vcq<int, 3> vcq;
        CHECK(IsSame<Element<decltype(vcq)>, int>::value == true);
        CHECK(vcq.size() == 0);
        CHECK(vcq.empty() == true);
        CHECK(length(vcq) == 0);
        CHECK(IsCtConst<decltype(length(vcq))>::value == false);

        vcq.push_back(1);

        CHECK(vcq.size() == 1);
        CHECK(vcq.empty() == false);
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
        CHECK(vcq[0] == 6);
        CHECK(vcq[1] == 7);
        CHECK(vcq[2] == 8);
        CHECK(vcq.pop_front() == 6);
        CHECK(vcq.pop_front() == 7);
        CHECK(vcq.pop_front() == 8);
    }
}

#endif