#ifndef NUMERIC_HPP_
#define NUMERIC_HPP_

#include "efp/prelude.hpp"

namespace efp {

    template <typename A>
    constexpr A square(const A& a) {
        return a * a;
    }

    template <typename A>
    constexpr A abs(const A& a) {
        return a < 0 ? -a : a;
    }

    template <typename A, typename B>
    constexpr A is_approx(const A& lhs, const B& rhs) {
        using MorePrecise = Conditional<(sizeof(A) > sizeof(B)), A, B>;

        return abs(static_cast<MorePrecise>(lhs) - static_cast<MorePrecise>(rhs)) <= NumericLimits<MorePrecise>::epsilon();
    }
} // namespace efp

#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1

#include <cmath>
#include <complex>

namespace efp {

    template <typename A>
    constexpr A sqrt(const A& a) {
        return std::sqrt(a);
    }

    template <typename A>
    constexpr A sin(const A& a) {
        return std::sin(a);
    }

    template <typename A>
    constexpr A cos(const A& a) {
        return std::cos(a);
    }

    template <typename A>
    constexpr A tan(const A& a) {
        return std::tan(a);
    }

    template <typename A>
    using Complex = typename std::complex<A>;

    template <typename A>
    struct IsComplex : False {};

    template <typename A>
    struct IsComplex<Complex<A>> : True {};

    template <typename A>
    struct ComplexBaseType {};

    template <typename A>
    struct ComplexBaseType<Complex<A>> {
        using Type = A;
    };

    template <typename A>
    using ComplexBase_t = typename ComplexBaseType<A>::Type;

    template <typename A>
    struct AssertComplex {
        using Type = Complex<A>;
    };

    template <typename A>
    struct AssertComplex<Complex<A>> {
        using Type = Complex<A>;
    };

    template <typename A>
    using AssertComplex_t = typename AssertComplex<A>::Type;

    template <bool to_complex, typename A>
    auto complex_cast(const A& a) -> EnableIf<!to_complex && IsComplex<A>::value, ComplexBase_t<A>> {
        return a.real();
    }

    template <bool to_complex, typename A>
    auto complex_cast(const A& a) -> EnableIf<to_complex && !IsComplex<A>::value, Complex<A>> {
        return Complex<A>{a, 0};
    }

    template <bool to_complex, typename A>
    auto complex_cast(const A& a) -> EnableIf<to_complex == IsComplex<A>::value, A> {
        return a;
    }

    // template<typename A>
    // constexpr Maybe<A> real_from_complex(const Complex<A>& a) {
    //     return is_approx(abs(a.imag()), 0) ? Maybe<A> {a.real()} : Maybe<A> {nothing};
    // }

} // namespace efp

#endif

namespace efp {
    // Reducings

    template <typename As>
    constexpr Element<As> max_elem(const As& as) {
        return foldl(max, NumericLimits<Element<As>>::min(), as);
    }

    template <typename As>
    constexpr Element<As> min_elem(const As& as) {
        return foldl(min, NumericLimits<Element<As>>::max(), as);
    }

    template <typename As>
    constexpr Element<As> max_min(const As& as) {
        return max_elem(as) - min_elem(as);
    }

    template <typename As>
    constexpr Element<As> sum(const As& as) {
        return foldl(op_add, static_cast<Element<As>>(0), as);
    }

    template <typename As>
    constexpr Element<As> product(const As& as) {
        return foldl(op_mul, static_cast<Element<As>>(1), as);
    }

} // namespace efp

#endif