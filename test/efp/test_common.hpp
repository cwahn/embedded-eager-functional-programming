#ifndef TEST_COMMON_HPP_
#define TEST_COMMON_HPP_

#include <algorithm>
#include <array>
#include <vector>

#include "catch2/catch_test_macros.hpp"
#include "efp.hpp"

using namespace efp;

const Array<double, 3> array_3{1., 2., 3.};
const Array<double, 5> array_5{1., 2., 3., 4., 5.};

const ArrVec<double, 3> arrvec_3{1., 2., 3.};
const ArrVec<double, 5> arrvec_5{1., 2., 3., 4., 5.};

const Vector<double> vector_3{1., 2., 3.};
const Vector<double> vector_5{1., 2., 3., 4., 5.};

const ArrayView<const double, 3> array_view_3{data(array_3)};
const ArrayView<const double, 5> array_view_5{data(array_5)};

const ArrVecView<const double, 3> arrvec_view_3{data(vector_3), 3};
const ArrVecView<const double, 5> arrvec_view_5{data(vector_5), 5};

const VectorView<const double> vector_view_3{data(vector_3), 3};
const VectorView<const double> vector_view_5{data(vector_5), 5};

const double c_array_3[3] = {1., 2., 3.};
const double c_array_5[5] = {1., 2., 3., 4., 5.};

const std::array<double, 3> stl_array_3{1., 2., 3.};
const std::array<double, 5> stl_array_5{1., 2., 3., 4., 5.};

const std::vector<double> stl_vector_3{1., 2., 3.};
const std::vector<double> stl_vector_5{1., 2., 3., 4., 5.};

// Thread local side effectful

class MockHW {
public:
    struct Resource {
        int id;
        int value;
    };

private:
    static thread_local int _next_id;
    static thread_local std::vector<Resource> _resources;
    static thread_local int _double_free_cnt;

public:
    MockHW() {}

    // Optional: Method to reset the manager for reuse or testing
    static void reset() {
        _resources.clear();
        _next_id = 1;
        _double_free_cnt = 0;
    }

    static Resource acquire() {
        Resource resource = {_next_id++, 0};
        _resources.push_back(resource);
        return resource;
    }

    static void release(Resource resource) {
        auto it = std::find_if(_resources.begin(), _resources.end(), [resource](const Resource& r) {
            return r.id == resource.id;
        });

        if (it != _resources.end()) {
            _resources.erase(it);
        } else {
            // This resource has already been released, incrementing double-free
            // counter
            ++_double_free_cnt;
        }
    }

    static int double_free_count() {
        return _double_free_cnt;
    }

    static int remaining_resource_count() {
        return _resources.size();
    }

    static bool is_leak_free() {
        return _resources.empty();
    }

    // Concatenate the values of all resources into a decimal int
    static int resource_state_to_int() {
        int result = 0;
        for (const auto& resource : _resources) {
            result = result * 10 + resource.id;
        }
        return result;
    }

    static String resource_state_to_string() {
        const auto to_str = [](const Resource& resource) { return std::to_string(resource.id); };
        const auto strings = map(to_str, _resources);
        return intercalate(String{", "}, strings);
    }

    static bool is_sound() {
        return _double_free_cnt == 0 && _resources.empty();
    }
};

thread_local int MockHW::_next_id = 1;
thread_local std::vector<MockHW::Resource> MockHW::_resources;
thread_local int MockHW::_double_free_cnt = 0;

// Class managing a resource of Resource Manager following RAII and rule of 5
class MockRaii {
    MockHW::Resource _resource;

public:
    MockRaii()
        : _resource(MockHW::acquire()) {}

    ~MockRaii() {
        if (_resource.id != 0)
            MockHW::release(_resource);
    }

    MockRaii(const MockRaii& other)
        : _resource(MockHW::acquire()) {
        // Copy the value from the other resource
        _resource.value = other._resource.value;
    }

    MockRaii& operator=(const MockRaii& other) {
        if (this != &other) {
            // Release the current resource
            if (_resource.id != 0)
                MockHW::release(_resource);

            // Acquire a new resource and copy the value from the other resource
            _resource = MockHW::acquire();
            _resource.value = other._resource.value;
        }

        return *this;
    }

    MockRaii(MockRaii&& other)
        : _resource(other._resource) {
        other._resource.id = 0;
    }

    MockRaii& operator=(MockRaii&& other) {
        if (this != &other) {
            if (_resource.id != 0)
                MockHW::release(_resource);

            _resource = other._resource;
            other._resource.id = 0;
        }

        return *this;
    }

    MockHW::Resource resource() const {
        return _resource;
    }
};

TEST_CASE("MockHW and MockRaii", "MockHW") {
    SECTION("Sanity") {
        // Test with MockRaii and MockHW
        MockHW::reset();
        {
            Enum<int, MockRaii> a = MockRaii{};
            CHECK(MockHW::remaining_resource_count() == 1);
            CHECK(MockHW::resource_state_to_int() == 1);

            Enum<int, MockRaii> b = a;
            CHECK(MockHW::remaining_resource_count() == 2);
            CHECK(MockHW::resource_state_to_int() == 12);

            Enum<int, MockRaii> c = MockRaii{};
            CHECK(MockHW::remaining_resource_count() == 3);
            CHECK(MockHW::resource_state_to_int() == 123);

            a = c;
            CHECK(MockHW::remaining_resource_count() == 3);
            CHECK(MockHW::resource_state_to_int() == 234);

            b = efp::move(c);
            CHECK(MockHW::remaining_resource_count() == 2);
            CHECK(MockHW::resource_state_to_int() == 34);
        }
        CHECK(MockHW::is_leak_free() == true);
        CHECK(MockHW::double_free_count() == 0);
        CHECK(MockHW::resource_state_to_int() == 0);
        MockHW::reset();
    }
}

#endif