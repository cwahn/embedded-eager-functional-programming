#ifndef EFP_TRAIT_HPP_
#define EFP_TRAIT_HPP_

#include "efp/meta.hpp"

// Type-level function traits could be implemneted for external types
// Method requirements could be satisfied by actual implementation or inheritance

namespace efp {
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