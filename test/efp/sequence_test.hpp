#ifndef SEQUENCE_TEST_HPP_
#define SEQUENCE_TEST_HPP_

#include "catch2/catch_test_macros.hpp"
#include "efp.hpp"

using namespace efp;

// Default implementation static assess message works as expected.
// class NonSequence {};

// const NonSequence non_sequence {};
// const auto _ = map([](int x) { return x; }, non_sequence);

TEST_CASE("Array Rule of five", "Array") {
    SECTION("New Constructor") {
        {
            MockHW::reset();
            Array<MockRaii, 3> a;
            CHECK(MockHW::remaining_resource_count() == 3);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Copy Constructor") {
        {
            MockHW::reset();
            Array<MockRaii, 3> a;
            Array<MockRaii, 3> b = a;
            CHECK(MockHW::remaining_resource_count() == 6);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Copy Assignment") {
        {
            MockHW::reset();
            Array<MockRaii, 3> a;
            Array<MockRaii, 3> b;
            b = a;
            CHECK(MockHW::remaining_resource_count() == 6);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Move Constructor") {
        {
            MockHW::reset();
            Array<MockRaii, 3> a;
            Array<MockRaii, 3> b = efp::move(a);
            CHECK(MockHW::remaining_resource_count() == 3);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Move Assignment") {
        {
            MockHW::reset();
            Array<MockRaii, 3> a;
            Array<MockRaii, 3> b;
            b = efp::move(a);
            CHECK(MockHW::remaining_resource_count() == 3);
        }
        CHECK(MockHW::is_sound());
    }
}

TEST_CASE("ArrVec Rule of 5", "ArrVec") {
    SECTION("New Constructor") {
        {
            MockHW::reset();
            ArrVec<MockRaii, 3> a;
            CHECK(MockHW::remaining_resource_count() == 0);
            a.push_back(MockRaii{});
            a.push_back(MockRaii{});
            CHECK(MockHW::remaining_resource_count() == 2);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Copy Constructor") {
        {
            MockHW::reset();
            ArrVec<MockRaii, 3> a;
            a.push_back(MockRaii{});
            a.push_back(MockRaii{});
            ArrVec<MockRaii, 3> b = a;
            CHECK(MockHW::remaining_resource_count() == 4);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Copy Assignment") {
        {
            MockHW::reset();
            ArrVec<MockRaii, 3> a;
            a.push_back(MockRaii{});
            a.push_back(MockRaii{});
            CHECK(MockHW::remaining_resource_count() == 2);
            ArrVec<MockRaii, 3> b;
            CHECK(MockHW::remaining_resource_count() == 2);
            b = a;
            CHECK(MockHW::remaining_resource_count() == 4);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Move Constructor") {
        {
            MockHW::reset();
            ArrVec<MockRaii, 3> a;
            a.push_back(MockRaii{});
            a.push_back(MockRaii{});
            CHECK(MockHW::remaining_resource_count() == 2);
            ArrVec<MockRaii, 3> b = efp::move(a);
            CHECK(MockHW::remaining_resource_count() == 2);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Move Assignment") {
        {
            MockHW::reset();
            ArrVec<MockRaii, 3> a;
            a.push_back(MockRaii{});
            a.push_back(MockRaii{});
            CHECK(MockHW::remaining_resource_count() == 2);
            ArrVec<MockRaii, 3> b;
            CHECK(MockHW::remaining_resource_count() == 2);
            b = efp::move(a);
            CHECK(MockHW::remaining_resource_count() == 2);
        }
        CHECK(MockHW::is_sound());
    }
}

TEST_CASE("Vector Rule of five", "Vector") {
    SECTION("New Constructor") {
        {
            MockHW::reset();
            Vector<MockRaii> a;
            CHECK(MockHW::remaining_resource_count() == 0);
            a.push_back(MockRaii{});
            a.push_back(MockRaii{});
            CHECK(MockHW::remaining_resource_count() == 2);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Copy Constructor") {
        {
            MockHW::reset();
            Vector<MockRaii> a;
            a.push_back(MockRaii{});
            a.push_back(MockRaii{});
            Vector<MockRaii> b = a;
            CHECK(MockHW::remaining_resource_count() == 4);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Copy Assignment") {
        {
            MockHW::reset();
            Vector<MockRaii> a;
            a.push_back(MockRaii{});
            a.push_back(MockRaii{});
            Vector<MockRaii> b;
            CHECK(MockHW::remaining_resource_count() == 2);
            b = a;
            CHECK(MockHW::remaining_resource_count() == 4);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Move Constructor") {
        {
            MockHW::reset();
            Vector<MockRaii> a;
            a.push_back(MockRaii{});
            a.push_back(MockRaii{});
            Vector<MockRaii> b = efp::move(a);
            CHECK(MockHW::remaining_resource_count() == 2);
        }
        CHECK(MockHW::is_sound());
    }

    SECTION("Move Assignment") {
        {
            MockHW::reset();
            Vector<MockRaii> a;
            a.push_back(MockRaii{});
            a.push_back(MockRaii{});
            Vector<MockRaii> b;
            CHECK(MockHW::remaining_resource_count() == 2);
            b = efp::move(a);
            CHECK(MockHW::remaining_resource_count() == 2);
        }
        CHECK(MockHW::is_sound());
    }
}

TEST_CASE("Initialization") {
    SECTION("Array Default Constructor") {
        Array<int, 5> a;
        CHECK(a.size() == 5); // Assuming default constructor initializes size to 0
    }

    SECTION("ArrVec Default Constructor") {
        ArrVec<int, 5> a;
        CHECK(a.size() == 0);
        CHECK(a.capacity() == 5);
    }

    SECTION("Vector Default Constructor") {
        Vector<int> a;
        CHECK(a.size() == 0);
        CHECK(a.capacity() == 0); // Assuming default capacity is 0
    }

    SECTION("Array Initialization") {
        Array<int, 3> a{1, 2, 3};
        CHECK(a == Array<int, 3>{1, 2, 3});
        CHECK(a.size() == 3);
    }

    SECTION("ArrVec Initialization") {
        ArrVec<int, 4> a = {1, 2, 3};
        CHECK(a.size() == 3);
        CHECK(a.capacity() == 4);
    }

    SECTION("Vector Initialization") {
        Vector<int> a = {1, 2, 3};
        CHECK(a.size() == 3);
        CHECK(a.capacity() == 4);
    }

    SECTION("Array Copy Constructor") {
        Array<int, 3> a{1, 2, 3};
        Array<int, 3> b = a;
        CHECK(b == Array<int, 3>{1, 2, 3});
    }

    SECTION("ArrVec Copy Constructor") {
        ArrVec<int, 4> a = {1, 2, 3};
        ArrVec<int, 4> b = a;
        CHECK(b.size() == 3);
        CHECK(b.capacity() == 4);
    }

    SECTION("Vector Copy Constructor") {
        Vector<int> a = {1, 2, 3};
        Vector<int> b = a;
        CHECK(b.size() == 3);
        CHECK(b.capacity() == 4);
    }

    SECTION("Array Assignment") {
        Array<int, 3> a{1, 2, 3};
        Array<int, 3> b;
        b = a;
        CHECK(b == Array<int, 3>{1, 2, 3});
    }

    SECTION("ArrVec Assignment") {
        ArrVec<int, 4> a = {1, 2, 3};
        ArrVec<int, 4> b;
        b = a;
        CHECK(b.size() == 3);
        CHECK(b.capacity() == 4);
    }

    SECTION("Vector Assignment") {
        Vector<int> a = {1, 2, 3};
        Vector<int> b;
        b = a;
        CHECK(b.size() == 3);
        CHECK(b.capacity() == 4);
    }
}

bool runtime_con = true;

Array<int, 3>* array_nrvo_p = nullptr;
ArrVec<int, 3>* arrvec_nrvo_p = nullptr;
Vector<int>* vector_nrvo_p = nullptr;

Array<int, 3>* n_array_nrvo_p = nullptr;
ArrVec<int, 3>* n_arrvec_nrvo_p = nullptr;
Vector<int>* n_vector_nrvo_p = nullptr;

Array<int, 3> array_rvo() {
    return Array<int, 3>{1, 2, 3};
};

Array<int, 3> array_nrvo() {
    Array<int, 3> result{0, 0, 0};

    result[0] = 1;
    result[1] = 2;
    result[2] = 3;
    array_nrvo_p = &result;

    return result;
};

Array<int, 3> n_array_nrvo() {
    Array<int, 3> result{0, 0, 0};
    Array<int, 3> n_result{1, 2, 3};

    result[0] = 1;
    result[1] = 2;
    result[2] = 3;
    n_array_nrvo_p = &result;

    return runtime_con ? result : n_result;
};

ArrVec<int, 3> array_vector_rvo() {
    return ArrVec<int, 3>{1, 2, 3};
};

ArrVec<int, 3> array_vector_nrvo() {
    ArrVec<int, 3> result{0, 0, 0};

    result[0] = 1;
    result[1] = 2;
    result[2] = 3;
    arrvec_nrvo_p = &result;

    return result;
};

ArrVec<int, 3> n_array_vector_nrvo() {
    ArrVec<int, 3> result{0, 0, 0};
    ArrVec<int, 3> n_result{1, 2, 3};

    result[0] = 1;
    result[1] = 2;
    result[2] = 3;
    n_arrvec_nrvo_p = &result;

    return runtime_con ? result : n_result;
};

Vector<int> vector_rvo() {
    return Vector<int>{1, 2, 3};
};

Vector<int> vector_nrvo() {
    Vector<int> result{0, 0, 0};

    result[0] = 1;
    result[1] = 2;
    result[2] = 3;
    vector_nrvo_p = &result;

    return result;
};

Vector<int> n_vector_nrvo() {
    Vector<int> result{0, 0, 0};
    Vector<int> n_result{1, 2, 3};

    result[0] = 1;
    result[1] = 2;
    result[2] = 3;
    n_vector_nrvo_p = &result;

    return runtime_con ? result : n_result;
};

TEST_CASE("Copy Elision") {
    SECTION("Array NRVO") {
        Array<int, 3> a_nrvo = array_nrvo();
        CHECK(array_nrvo_p == &a_nrvo); // NRVO should have occurred;

        Array<int, 3> a_n_nrvo = n_array_nrvo();
        CHECK(
            n_array_nrvo_p != &a_n_nrvo); // Conditional NRVO should have occurred; pointers should differ
    }

    SECTION("ArrVec NRVO") {
        ArrVec<int, 3> av_nrvo = array_vector_nrvo();
        CHECK(arrvec_nrvo_p == &av_nrvo); // NRVO should have occurred;

        ArrVec<int, 3> av_n_nrvo = n_array_vector_nrvo();
        CHECK(
            n_arrvec_nrvo_p != &av_n_nrvo); // Conditional NRVO should have occurred; pointers should differ
    }

    SECTION("Vector NRVO") {
        Vector<int> v_nrvo = vector_nrvo();
        CHECK(vector_nrvo_p == &v_nrvo); // NRVO should have occurred;

        Vector<int> v_n_nrvo = n_vector_nrvo();
        CHECK(
            n_vector_nrvo_p != &v_n_nrvo); // Conditional NRVO should have occurred; pointers should differ
    }
}

TEST_CASE("erase") {
    SECTION("ArrVec::erase") {
        ArrVec<int, 5> arrvec{1, 2, 3, 4, 5};

        // Erasing a middle element
        arrvec.erase(2);
        CHECK(arrvec == ArrVec<int, 5>{1, 2, 4, 5});

        // Erasing the first element
        arrvec.erase(0);
        CHECK(arrvec == ArrVec<int, 5>{2, 4, 5});

        // Erasing the last element
        arrvec.erase(arrvec.size() - 1);
        CHECK(arrvec == ArrVec<int, 5>{2, 4});

        // Multiple erasures
        arrvec.erase(1);
        arrvec.erase(0);
        CHECK(arrvec.empty());

        // Erasing with invalid index (should assert or throw depending on implementation)
        // CHECK_THROWS(arrvec.erase(5));
    }

    SECTION("Vector::erase") {
        Vector<int> vec{1, 2, 3, 4, 5};

        // Erasing a middle element
        vec.erase(2);
        CHECK(vec == Vector<int>{1, 2, 4, 5});

        // Erasing the first element
        vec.erase(0);
        CHECK(vec == Vector<int>{2, 4, 5});

        // Erasing the last element
        vec.erase(vec.size() - 1);
        CHECK(vec == Vector<int>{2, 4});

        // Multiple erasures
        vec.erase(1);
        vec.erase(0);
        CHECK(vec.empty());

        // Erasing with invalid index (should assert or throw depending on implementation)
        // CHECK_THROWS(vec.erase(5));
    }
}

TEST_CASE("insert") {
    SECTION("ArrVec::insert") {
        ArrVec<int, 5> arrvec{1, 3, 4};

        // Insert at the beginning
        arrvec.insert(0, 0);
        CHECK(arrvec == ArrVec<int, 5>{0, 1, 3, 4});

        // Insert in the middle
        arrvec.insert(2, 2);
        CHECK(arrvec == ArrVec<int, 5>{0, 1, 2, 3, 4});

        // Insert into an empty container
        ArrVec<int, 5> empty_arrvec;
        empty_arrvec.insert(0, 1);
        CHECK(empty_arrvec == ArrVec<int, 5>{1});
    }

    SECTION("Vector::insert") {
        Vector<int> vec{1, 3, 4};

        // Insert at the beginning
        vec.insert(0, 0);
        CHECK(vec == Vector<int>{0, 1, 3, 4});

        // Insert in the middle
        vec.insert(2, 2);
        CHECK(vec == Vector<int>{0, 1, 2, 3, 4});

        // Insert at the end
        vec.insert(vec.size(), 5);
        CHECK(vec == Vector<int>{0, 1, 2, 3, 4, 5});

        // Insert into an empty container
        Vector<int> empty_vec;
        empty_vec.insert(0, 1);
        CHECK(empty_vec == Vector<int>{1});
    }
}

TEST_CASE("push_back") {
    SECTION("ArrVec::push_back") {
        ArrVec<int, 5> arrvec;

        // Push_back into an empty container
        arrvec.push_back(1);
        CHECK(arrvec == ArrVec<int, 5>{1});

        // Push_back to add more elements
        arrvec.push_back(2);
        arrvec.push_back(3);
        CHECK(arrvec == ArrVec<int, 5>{1, 2, 3});
    }

    SECTION("Vector::push_back") {
        Vector<int> vec;

        // Push_back into an empty container
        vec.push_back(1);
        CHECK(vec == Vector<int>{1});

        // Push_back to add more elements
        vec.push_back(2);
        vec.push_back(3);
        CHECK(vec == Vector<int>{1, 2, 3});

        // Verify capacity grows as expected (assuming starting capacity is less than 3)
        CHECK(vec.capacity() >= 3);
    }
}

TEST_CASE("pop_back") {
    SECTION("ArrVec::pop_back") {
        ArrVec<int, 5> arrvec{1, 2, 3, 4, 5};

        // Pop_back from a container with multiple elements
        arrvec.pop_back();
        CHECK(arrvec == ArrVec<int, 5>{1, 2, 3, 4});

        // Pop_back to remove the last remaining element
        arrvec.clear(); // Clearing first to simulate single element condition
        arrvec.push_back(1);
        arrvec.pop_back();
        CHECK(arrvec.empty());

        //! pop_back on an empty container is undefined behavior
        // arrvec.pop_back();
        // CHECK(arrvec.empty()); // Expect no change
    }

    SECTION("Vector::pop_back") {
        Vector<int> vec{1, 2, 3, 4, 5};

        // Pop_back from a container with multiple elements
        vec.pop_back();
        CHECK(vec == Vector<int>{1, 2, 3, 4});

        // Pop_back to remove the last remaining element
        vec.clear(); // Clearing first to simulate single element condition
        vec.push_back(1);
        vec.pop_back();
        CHECK(vec.empty());

        //! pop_back on an empty container is undefined behavior
        // vec.pop_back();
        // CHECK(vec.empty()); // Expect no change
    }
}

TEST_CASE("clear") {
    SECTION("ArrVec::clear") {
        ArrVec<int, 5> arrvec{1, 2, 3, 4, 5};

        // Clear a container with multiple elements
        arrvec.clear();
        CHECK(arrvec.empty());

        // Clear an already empty container
        arrvec.clear();
        CHECK(arrvec.empty()); // Expect no change
    }

    SECTION("Vector::clear") {
        Vector<int> vec{1, 2, 3, 4, 5};

        // Clear a container with multiple elements
        vec.clear();
        CHECK(vec.empty());

        // Clear an already empty container
        vec.clear();
        CHECK(vec.empty()); // Expect no change
    }
}

TEST_CASE("ArrayView") {
    SECTION("construction") {
        int a = 0;
        const int a_const = 0;

        // const pointer will create read-only view.

        ArrayView<int, 1> a_view0{&a};
        ArrayView<const int, 1> a_view1{&a_const};
        const ArrayView<int, 1> a_view2{&a};
        const ArrayView<const int, 1> a_view3{&a_const};
        // ArrayView<int, 1> a_view4{&a_const};
        ArrayView<const int, 1> a_view5{&a};
        // const ArrayView<int, 1> a_view6{&a_const};
        const ArrayView<const int, 1> a_view7{&a};
    }

    SECTION("copy construction") {
        ArrayView<const double, 3> view_1{data(array_3)};
        auto view_2 = view_1;
        CHECK(view_1 == view_2);
    }
}

TEST_CASE("VectorView") {
    // ! constexpr ArrayView does not works
    // SECTION("constexpr construction")
    // {
    //     constexpr double ct_data[3] = {1., 2., 3.};
    //     constexpr VectorView<const double> view_1{ct_data, 3};
    //     auto view_2 = view_1;
    //     CHECK(view_1 == view_2);
    // }

    SECTION("copy construction") {
        VectorView<const double> view_1{data(array_3), 3};
        auto view_2 = view_1;
        CHECK(view_1 == view_2);
    }
}

TEST_CASE("Vector Copy") {
    SECTION("Vector copy") {
        Vector<double> view_1{3., 3., 3.};
        auto view_2 = view_1;
        CHECK(view_1 == view_2);
    }
}

// Vector allocation startegy test
TEST_CASE("Vector Allocation Strategy", "Vector") {
    SECTION("Default Constructor") {
        Vector<int> vec{};
        CHECK(vec.size() == 0);
        CHECK(vec.capacity() == 0);
    }

    SECTION("Copy Constructor") {
        Vector<int> vec{1, 2, 3};
        Vector<int> vec_copy = vec;
        CHECK(vec_copy.size() == 3);
        CHECK(vec_copy.capacity() == 4);
    }

    SECTION("Copy Assignment") {
        Vector<int> vec{1, 2, 3};
        Vector<int> vec_copy;
        vec_copy = vec;
        CHECK(vec_copy.size() == 3);
        CHECK(vec_copy.capacity() == 4);
    }

    SECTION("Move Constructor") {
        Vector<int> vec{1, 2, 3};
        Vector<int> vec_move = efp::move(vec);
        CHECK(vec_move.size() == 3);
        CHECK(vec_move.capacity() == 4);
    }

    SECTION("Move Assignment") {
        Vector<int> vec{1, 2, 3};
        Vector<int> vec_move;
        vec_move = efp::move(vec);
        CHECK(vec_move.size() == 3);
        CHECK(vec_move.capacity() == 4);
    }

    SECTION("Vector::reserve") {
        Vector<int> vec{1, 2, 3};
        vec.reserve(10);
        CHECK(vec.size() == 3);
        CHECK(vec.capacity() == 10);
    }

    SECTION("Vector::shrink_to_fit") {
        Vector<int> vec{1, 2, 3};
        vec.reserve(10);
        vec.shrink_to_fit();
        CHECK(vec.size() == 3);
        CHECK(vec.capacity() == 4);
    }

    SECTION("Vector::resize") {
        Vector<int> vec{1, 2, 3};
        vec.resize(5);
        CHECK(vec.size() == 5);
        CHECK(vec.capacity() == 6);
    }

    SECTION("Vector::push_back") {
        Vector<int> vec{};

        vec.push_back(1);
        CHECK(vec.size() == 1);
        CHECK(vec.capacity() == 2);

        vec.push_back(2);
        CHECK(vec.size() == 2);
        CHECK(vec.capacity() == 3);

        vec.push_back(3);
        vec.push_back(4);
        vec.push_back(5);

        CHECK(vec.size() == 5);
        CHECK(vec.capacity() == 9);
    }

    SECTION("Vector::emplace_back") {
        // todo
    }

    SECTION("Vector::pop_back") {
        Vector<int> vec{1, 2, 3, 4, 5};

        vec.pop_back();
        CHECK(vec.size() == 4);
        CHECK(vec.capacity() == 6);

        vec.pop_back();
        vec.pop_back();
        vec.pop_back();
        vec.pop_back();

        CHECK(vec.size() == 0);
        CHECK(vec.capacity() == 6);
    }

    SECTION("Vector::insert") {
        Vector<int> vec{1, 2, 3};

        vec.insert(0, 0);
        CHECK(vec.size() == 4);
        CHECK(vec.capacity() == 8);

        vec.insert(2, 2);
        CHECK(vec.size() == 5);
        CHECK(vec.capacity() == 8);

        vec.insert(5, 5);
        CHECK(vec.size() == 6);
        CHECK(vec.capacity() == 8);
    }

    SECTION("Vector::erase") {
        Vector<int> vec{1, 2, 3, 4, 5};

        vec.erase(2);
        CHECK(vec.size() == 4);
        CHECK(vec.capacity() == 6);

        vec.erase(0);
        CHECK(vec.size() == 3);
        CHECK(vec.capacity() == 6);

        vec.erase(2);
        CHECK(vec.size() == 2);
        CHECK(vec.capacity() == 6);
    }

    SECTION("Vector::clear") {
        Vector<int> vec{1, 2, 3, 4, 5};

        vec.clear();
        CHECK(vec.size() == 0);
        CHECK(vec.capacity() == 6);
    }
}

#endif