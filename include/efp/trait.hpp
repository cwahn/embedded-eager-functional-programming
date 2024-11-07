#ifndef EFP_TRAIT_HPP_
#define EFP_TRAIT_HPP_

#include "efp/meta.hpp"

// Type-level function traits could be implemneted for external types
// Method requirements could be satisfied by actual implementation or inheritance

namespace efp {

// Functor
template<typename F, template<typename...> class Functor, typename A>
auto fmap(const F& f, const Functor<A>& a) -> Functor<decltype(f(a))> {
    static_assert(AlwaysFalse<F>::value, "fmap is not implemented for the type");
    return {};
}

// Implement Functor for std::array
template<typename F, typename A, size_t n>
auto fmap(const F& f, const std::array<A, n>& a) -> std::array<decltype(f(a[0])), n> {
    std::array<A, n> result;

    for (size_t i = 0; i < n; ++i) {
        result[i] = f(a[i]);
    }

    return result;
}

// Implement Functor for std::vector
template<typename F, typename A>
// auto fmap(const std::function<A(const A&)>& f, const std::vector<A>& a) -> std::vector<A> {
auto fmap(const F& f, const std::vector<A>& a) -> std::vector<decltype(f(a[0]))> {
    std::vector<A> result;
    result.reserve(a.size());

    for (const auto& x : a) {
        result.push_back(f(x));
    }

    return result;
}

// Applicative
template<template<typename...> class Applicative, typename A>
auto pure(const A& a) -> Applicative<A> {
    static_assert(AlwaysFalse<A>::value, "pure is not implemented for the type");
    return {};
}

template<typename F, template<typename...> class Applicative, typename A>
auto apply(const Applicative<F>& f, const Applicative<A>& a) -> Applicative<decltype(f(a))> {
    static_assert(AlwaysFalse<F>::value, "apply is not implemented for the type");
    return {};
}

// Monad
template<template<typename...> class Monad, typename A>
auto pure(const A& a) -> Monad<A> {
    static_assert(AlwaysFalse<A>::value, "pure is not implemented for the type");
    return {};
}

template<typename F, template<typename...> class Monad, typename A>
auto bind(const F& f, const Monad<A>& a) -> Monad<decltype(f(a))> {
    static_assert(AlwaysFalse<F>::value, "bind is not implemented for the type");
    return {};
}

// Implement Monad for std::vector
template<typename A>
auto pure(const A& a) -> std::vector<A> {
    return {a};
}

template<typename F, typename A>
auto bind(const F& f, const std::vector<A>& a) -> std::vector<decltype(f(a[0]))> {
    std::vector<A> result;

    for (const auto& x : a) {
        auto y = f(x);
        result.insert(result.end(), y.begin(), y.end());
    }

    return result;
}

// Io action trait
template<typename A>
auto run() -> A {
    static_assert(AlwaysFalse<A>::value, "run is not implemented for the type");
    return {};
}





//////////////////////////////////////

// Element trait
// Should be the type of the elements of the sequence
template<typename A>
struct ElementImpl {
    static_assert(AlwaysFalse<A>::value, "Element trait is not implemented for the type");
};

template<typename A>
using Element = typename ElementImpl<CVRefRemoved<A>>::Type;

// Sequence traits
// dyn
constexpr size_t dyn = std::numeric_limits<size_t>::max();
using Dyn = Size<dyn>;

// CtSize
// Should be IntegralConstant<size_t> with compile time size.
// dyn for dynamic size
template<typename A>
struct CtSizeImpl {
    static_assert(AlwaysFalse<A>::value, "CtSize trait is not implemented for the type");
};

template<typename A>
using CtSize = typename CtSizeImpl<CVRefRemoved<A>>::Type;

// CtCapacity
// Should be IntegralConstant<size_t> with compile time capcity.
// dyn for dynamic size
template<typename A>
struct CtCapacityImpl {
    static_assert(AlwaysFalse<A>::value, "CtCapacity trait is not implemented for the type");
};

template<typename A>
using CtCapacity = typename CtCapacityImpl<CVRefRemoved<A>>::Type;

template<typename As, size_t n, typename = Void<CtSize<As>>>
constexpr auto length(const As&) -> CtSize<As> {
    static_assert(AlwaysFalse<As>::value, "length is not implemented for the type");
    return {};
}

template<typename As>
constexpr auto length(const As&) -> size_t {
    static_assert(AlwaysFalse<As>::value, "length is not implemented for the type");
    return {};
}

template<typename As, typename = Void<Element<As>>>
constexpr auto nth(size_t, const As&) -> const Element<As>& {
    static_assert(AlwaysFalse<As>::value, "nth is not implemented for the type");
    return {};
}

template<typename As, typename = Void<Element<As>>>
constexpr auto nth(size_t, As&) -> Element<As>& {
    static_assert(AlwaysFalse<As>::value, "nth is not implemented for the type");
    return {};
}

template<typename As, typename = Void<Element<As>>>
constexpr auto data(const As&) -> const Element<As>* {
    static_assert(AlwaysFalse<As>::value, "data is not implemented for the type");
    return {};
}

template<typename As, typename = Void<Element<As>>>
constexpr auto data(As&) -> Element<As>* {
    static_assert(AlwaysFalse<As>::value, "data is not implemented for the type");
    return {};
}

// IsStaticSize

template<typename A>
using IsStaticSize = Bool<CtSize<A>::value != dyn>;

// IsStaticCapacity

template<typename A>
using IsStaticCapacity = Bool<CtCapacity<A>::value != dyn>;

}  // namespace efp

#endif