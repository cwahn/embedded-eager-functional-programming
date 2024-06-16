#ifndef PRELUDE_HPP_
#define PRELUDE_HPP_

#include "efp/meta.hpp"
#include "efp/enum.hpp"
#include "efp/maybe.hpp"
#include "efp/sequence.hpp"
#include "efp/trait.hpp"

// Trust compiler for length calculation

namespace efp {

// todo concat, concat_map

// Fold and Traversals

// foldl :: (A -> B -> A) -> A -> [B] -> A
template<typename A, typename Bs, typename F = A (*)(const A&, const Element<Bs>&)>
auto foldl(const F& f, const A& init, const Bs& bs) -> A {
    A res = init;
    const auto res_length = length(bs);

    for (size_t i = 0; i < res_length; ++i) {
        res = f(res, nth(i, bs));
    }

    return res;
}

// foldr :: (A -> B -> B) -> B -> [A] -> B
template<typename As, typename B, typename F = B (*)(const Element<As>&, const B&)>
auto foldr(const F& f, const B& init, const As& as) -> B {
    B res = init;

    for (size_t i = length(as); i > 0; i--) {
        res = f(nth(i - 1, as), res);
    }

    return res;
}

// elem_index

template<typename As>
Maybe<size_t> elem_index(const Element<As>& a, const As& as) {
    const auto as_len = length(as);

    for (size_t i = 0; i < as_len; ++i) {
        if (nth(i, as) == a) {
            return i;
        }
    }

    return nothing;
}

// ElemIndicesReturn

template<typename As>
using ElemIndicesReturn =
    Conditional<IsStaticCapacity<As>::value, ArrVec<size_t, CtCapacity<As>::value>, Vector<size_t>>;

// elem_indices

template<typename As>
auto elem_indices(const Element<As>& a, const As& as) -> ElemIndicesReturn<As> {
    ElemIndicesReturn<As> res {};
    const auto as_len = length(as);

    for (size_t i = 0; i < as_len; ++i) {
        if (a == nth(i, as)) {
            res.push_back(i);
        }
    }

    return res;
}

// todo maximum, minimum, sum, product

// Miscellanius functions

// id :: A -> A
template<typename A>
constexpr A id(const A& a) {
    return a;
}

// Composed

template<typename... Fs>
struct Composed {};

template<typename F>
struct Composed<F> {
    const F& f;

    Composed(const F& f) : f(f) {}

    template<typename... Arg>
    inline auto operator()(Arg&&... arg) const -> decltype(f(forward<Arg>(arg)...)) {
        return f(forward<Arg>(arg)...);
    }
};

template<typename F, typename... Fs>
struct Composed<F, Fs...> {
    const F& f;
    const Composed<Fs...> fs;

    Composed(const F& f, const Fs&... fs) : f(f), fs(Composed<Fs...>(fs...)) {}

    template<typename... Arg>
    inline auto operator()(Arg&&... arg) const -> decltype(f(fs(forward<Arg>(arg)...))) {
        return f(fs(forward<Arg>(arg)...));
    }
};

// compose :: (B -> C) -> (A -> B) -> A -> C
template<typename F, typename G>
auto compose(const F& f, const G& g) -> Composed<F, G> {
    return Composed<F, G>(f, g);
}

template<typename F, typename... Fs>
auto compose(const F& f, const Fs&... fs) -> Composed<F, Fs...> {
    return Composed<F, Fs...>(f, fs...);
}

// _min_length

template<typename As, typename... Ass>
size_t _min_length(const As& as, const Ass&... ass) {
    return _minimum({static_cast<size_t>(length(as)), static_cast<size_t>(length(ass))...});
}

// for_each :: (A -> void) -> [A] -> void
template<typename... Ass, typename F = void (*)(const Element<Ass>&...)>
void for_each(const F& f, const Ass&... ass) {
    const size_t res_length = _min_length(ass...);

    for (size_t i = 0; i < res_length; ++i) {
        f(nth(i, ass)...);
    }
}

// for_each_mut :: (A -> void) -> [A] -> void
template<typename... Ass, typename F = void (*)(Element<Ass>&...)>
void for_each_mut(const F& f, Ass&... ass) {
    const size_t res_length = _min_length(ass...);

    for (size_t i = 0; i < res_length; ++i) {
        f(nth(i, ass)...);
    }
}

template<typename R, typename... Ass>
using NAryReturn = Conditional<
    _and({IsStaticSize<Ass>::value...}),
    Array<R, _minimum({CtSize<Ass>::value...})>,
    Conditional<
        _and({IsStaticCapacity<Ass>::value...}),
        ArrVec<R, _minimum({CtCapacity<Ass>::value...})>,
        Vector<R>>>;

// MapReturn

template<typename F, typename... Ass>
using MapReturn = NAryReturn<InvokeResult<F, Element<Ass>...>, Ass...>;

// map :: (A -> B) -> [A] -> [B]
template<typename F, typename... Ass>
auto map(const F& f, const Ass&... ass) -> MapReturn<F, Ass...> {
    MapReturn<F, Ass...> res {};
    const size_t res_len = _min_length(ass...);

    if (CtSize<MapReturn<F, Ass...>>::value == dyn) {
        res.resize(res_len);
    }

    for (size_t i = 0; i < res_len; ++i) {
        nth(i, res) = f(nth(i, ass)...);
    }

    return res;
}

// ConcatReturn

template<typename As, typename... Ass>
struct ConcatReturnImpl {};

template<typename Ass>
using ConcatReturn = Conditional<
    IsStaticSize<Ass>::value && IsStaticSize<Element<Ass>>::value,
    Array<Element<Element<Ass>>, CtSize<Ass>::value * CtSize<Element<Ass>>::value>,
    Conditional<
        IsStaticCapacity<Ass>::value && IsStaticCapacity<Element<Ass>>::value,
        Conditional<
            IsStaticSize<Ass>::value,
            ArrVec<Element<Element<Ass>>, CtSize<Ass>::value * CtCapacity<Element<Ass>>::value>,
            Conditional<
                IsStaticSize<Element<Ass>>::value,
                ArrVec<Element<Element<Ass>>, CtCapacity<Ass>::value * CtSize<Element<Ass>>::value>,
                ArrVec<
                    Element<Element<Ass>>,
                    CtCapacity<Ass>::value * CtCapacity<Element<Ass>>::value>>>,
        Vector<Element<Element<Ass>>>>>;

// concat :: [[A]] -> [A]
template<typename Ass>
auto concat(const Ass& ass) -> ConcatReturn<Ass> {
    ConcatReturn<Ass> res {};

    const auto ass_len = length(ass);

    if (!IsStaticSize<ConcatReturn<Ass>>::value) {
        size_t res_len = 0;

        for (size_t i = 0; i < ass_len; ++i) {
            res_len += length(nth(i, ass));
        }

        res.resize(res_len);
    }

    size_t idx = 0;
    for (size_t i = 0; i < ass_len; ++i) {
        const auto& as = nth(i, ass);  // Access each as
        const auto as_len = length(as);

        for (size_t j = 0; j < as_len; ++j) {
            nth(idx++, res) = nth(j, as);
        }
    }

    return res;
}

// FilterReturn

template<typename As>
using FilterReturn = Conditional<
    CtCapacity<As>::value != dyn,
    ArrVec<Element<As>, CtCapacity<As>::value>,
    Vector<Element<As>>>;

// filter :: (A -> Bool) -> [A] -> [A]
template<typename As, typename F = bool (*)(const Element<As>&)>
auto filter(const F& f, const As& as) -> FilterReturn<As> {
    FilterReturn<As> res {};
    const auto res_len = length(as);

    for (size_t i = 0; i < res_len; ++i) {
        const auto& a = nth(i, as);

        if (f(a)) {
            res.push_back(a);
        }
    }

    return res;
}

// head

// ! Partial function. Make sure non-empty

template<typename As>
auto head(const As& as) -> const Element<As>& {
    if (as.empty()) {
        throw RuntimeError("Sequence should not be empty");
    }

    return nth(0, as);
}

// last

// ! Partial function. Make sure non

template<typename As>
auto last(const As& as) -> const Element<As>& {
    if (length(as) == 0) {
        throw RuntimeError("Sequence should not be empty");
    }

    return nth(length(as) - 1, as);
}

// TailReturn

template<typename As, bool is_const>
using TailReturn = EnableIf<
    CtSize<As>::value != 0 && CtCapacity<As>::value != 0,
    Conditional<
        IsStaticSize<As>::value,
        Conditional<
            is_const,
            ArrayView<const Element<As>, CtSize<As>::value - 1>,
            ArrayView<Element<As>, CtSize<As>::value - 1>>,
        Conditional<
            IsStaticCapacity<As>::value,
            Conditional<
                is_const,
                ArrVecView<const Element<As>, CtCapacity<As>::value - 1>,
                ArrVecView<Element<As>, CtCapacity<As>::value - 1>>,
            Conditional<is_const, VectorView<const Element<As>>, VectorView<Element<As>>>>>>;

// tail
// ! Partial function. Application on empty list is abortion.

template<typename A>
auto tail(const A& as) -> TailReturn<A, true> {
    if (length(as) == 0) {
        throw RuntimeError("Sequence should not be empty");
    }

    return {data(as) + 1, length(as) - 1};
}

template<typename A>
auto tail(A& as) -> TailReturn<A, false> {
    if (length(as) == 0) {
        throw RuntimeError("Sequence should not be empty");
    }

    return {data(as) + 1, length(as) - 1};
}

// InitReturn

template<typename As, bool is_const>
using InitReturn = EnableIf<
    CtSize<As>::value != 0 && CtCapacity<As>::value != 0,
    Conditional<
        IsStaticSize<As>::value,
        Conditional<
            is_const,
            ArrayView<const Element<As>, CtSize<As>::value - 1>,
            ArrayView<Element<As>, CtSize<As>::value - 1>>,
        Conditional<
            IsStaticCapacity<As>::value,
            Conditional<
                is_const,
                ArrVecView<const Element<As>, CtCapacity<As>::value - 1>,
                ArrVecView<Element<As>, CtCapacity<As>::value - 1>>,
            Conditional<is_const, VectorView<const Element<As>>, VectorView<Element<As>>>>>>;

// init

// ! Zero length input will envoke abort.÷

template<typename As>
auto init(const As& as) -> InitReturn<As, true> {
    if (length(as) == 0) {
        throw RuntimeError("Sequence should not be empty");
    }

    return {data(as), length(as) - 1};
}

template<typename As>
auto init(As& as) -> InitReturn<As, false> {
    if (length(as) == 0) {
        throw RuntimeError("Sequence should not be empty");
    }

    return {data(as), length(as) - 1};
}

// cf nth is part of sequence trait

// is_null

template<typename As>
bool is_null(const As& as) {
    return length(as) == 0;
}

// cf length is part of sequence trait

// todo reverse

// Special folds

// and :: [Bool] -> Bool

// or :: [Bool] -> Bool

// any :: (A -> Bool) -> [A] -> Bool

// all :: (A -> Bool) -> [A] -> Bool

// FromFunctionReturnImpl

namespace detail {
    template<typename N, typename F>
    struct FromFunctionReturnImpl {
        using Type = Vector<InvokeResult<F, N>>;
    };

    template<size_t n, typename F>
    struct FromFunctionReturnImpl<Size<n>, F> {
        using Type = Array<InvokeResult<F, size_t>, n>;
    };
}  // namespace detail

// FromFunctionReturn

template<typename N, typename F>
using FromFunctionReturn = typename detail::FromFunctionReturnImpl<N, F>::Type;

// from_function :: (Size -> A) -> Size -> [A]
template<typename N, typename F>
auto from_function(const N& length, const F& f) -> FromFunctionReturn<N, F> {
    FromFunctionReturn<N, F> res {};

    if (!IsStaticSize<FromFunctionReturn<N, F>>::value) {
        res.resize(length);
    }

    for (size_t i = 0; i < static_cast<size_t>(length); ++i) {
        nth(i, res) = f(i);
    }

    return res;
}

// execute_pack

template<typename... Args>
void execute_pack(Args... args) {}

template<typename... Ass>
using AppendReturn = Conditional<
    _and({IsStaticSize<Ass>::value...}),
    Array<Common<Element<Ass>...>, _sum({CtSize<Ass>::value...})>,
    Conditional<
        _and({IsStaticCapacity<Ass>::value...}),
        ArrVec<Common<Element<Ass>...>, _sum({CtCapacity<Ass>::value...})>,
        Vector<Common<Element<Ass>...>>>>;

namespace detail {
    template<typename As, typename Bs>
    Unit append_impl(size_t& idx, As& as, const Bs& bs) {
        const auto seq_length = length(bs);

        for (size_t i = 0; i < seq_length; ++i) {
            nth(idx, as) = nth(i, bs);
            idx++;
        }

        return unit;
    }
}  // namespace detail

// append :: [A] -> [A] ... -> [A]
template<typename As, typename... Ass>
auto append(const As& as, const Ass&... ass) -> AppendReturn<As, Ass...> {
    AppendReturn<As, Ass...> res {};

    if (CtSize<AppendReturn<As, Ass...>>::value == dyn) {
        res.resize(_sum({static_cast<size_t>(length(as)), static_cast<size_t>(length(ass))...}));
    }

    size_t idx = 0;
    execute_pack(detail::append_impl(idx, res, as), detail::append_impl(idx, res, ass)...);

    return res;
}

// IntercalateReturn

template<typename As, typename Ass>
using IntercalateReturn = Conditional<
    _and({IsStaticSize<As>::value, IsStaticSize<Ass>::value, IsStaticSize<Element<Ass>>::value}),
    Array<
        Element<Element<Ass>>,
        CtSize<Ass>::value*(CtSize<Element<Ass>>::value + CtSize<As>::value) - CtSize<As>::value>,
    Conditional<
        _and(
            {IsStaticCapacity<As>::value,
             IsStaticCapacity<Ass>::value,
             IsStaticCapacity<Element<Ass>>::value}
        ),
        ArrVec<
            Element<Element<Ass>>,
            CtCapacity<Ass>::value*(CtCapacity<Element<Ass>>::value + CtCapacity<As>::value)
                - CtCapacity<As>::value>,
        Vector<Element<Element<Ass>>>>>;

// intercalate :: [A] -> [[A]] -> [A]
template<typename As, typename Ass>
auto intercalate(const As& delimeter, const Ass& ass) -> IntercalateReturn<As, Ass> {
    IntercalateReturn<As, Ass> result {};
    const size_t delimeter_len = length(delimeter);
    const size_t ass_len = length(ass);

    if (ass_len == 0) {
        return result;
    }

    if (CtSize<IntercalateReturn<As, Ass>>::value == dyn) {
        size_t result_len = 0;

        for (size_t i = 0; i < ass_len; ++i) {
            result_len += length(nth(i, ass));
        }

        if (ass_len != 0) {
            result_len = result_len + (ass_len - 1) * delimeter_len;
        }

        result.resize(result_len);
    }

    size_t idx = 0;
    for (size_t i = 0; i < ass_len - 1; ++i) {
        const auto& as = nth(i, ass);  // Access each as
        const auto as_len = length(as);

        for (size_t j = 0; j < as_len; ++j) {
            nth(idx++, result) = nth(j, as);
        }

        for (size_t j = 0; j < delimeter_len; ++j) {
            nth(idx++, result) = nth(j, delimeter);
        }
    }

    const auto& as = nth(ass_len - 1, ass);  // Last of ass
    const auto as_len = length(as);

    for (size_t j = 0; j < as_len; ++j) {
        nth(idx++, result) = nth(j, as);
    }

    return result;
}

// for_index :: (size_t -> A) -> size_t -> void
template<typename F = void (*)(size_t)>
void for_index(const F& f, const size_t n) {
    for (size_t i = 0; i < n; ++i) {
        f(i);
    }
}

// for_each_with_index :: (size_t -> A -> void) -> [A] ... -> void
template<typename... Ass, typename F = void (*)(size_t, const Element<Ass>&...)>
void for_each_with_index(const F& f, const Ass&... seqs) {
    const auto min_len = _min_length(seqs...);

    for (size_t i = 0; i < min_len; ++i) {
        f(i, nth(i, seqs)...);
    }
}

template<typename... Ass, typename F = void (*)(size_t, Element<Ass>&...)>
void for_each_with_index_mut(const F& f, Ass&... seqs) {
    const size_t min_len = _min_length(seqs...);

    for (size_t i = 0; i < min_len; ++i) {
        f(i, nth(i, seqs)...);
    }
}

// cartesian_for_each
template<typename As, typename F = void (*)(const Element<As>&)>
void cartesian_for_each(const F& f, const As& as) {
    for_each(f, as);
}

template<
    typename As,
    typename... Ass,
    typename F = void (*)(const Element<As>&, const Element<Ass>&...)>
void cartesian_for_each(const F& f, const As& as, const Ass&... ass) {
    const auto as_len = length(as);

    for (size_t i = 0; i < as_len; ++i) {
        const auto a = nth(i, as);
        const auto inner = [&](const Element<Ass>&... xs) { f(a, xs...); };

        cartesian_for_each(inner, ass...);
    }
}

template<typename As, typename F = void (*)(Element<As>&)>
void cartesian_for_each_mut(const F& f, As& as) {
    for_each_mut(f, as);
}

template<typename As, typename... Ass, typename F = void (*)(Element<As>&, Element<Ass>&...)>
void cartesian_for_each_mut(const F& f, As& as, Ass&... ass) {
    const auto as_len = length(as);

    for (size_t i = 0; i < as_len; ++i) {
        const auto a = nth(i, as);
        const auto inner = [&](Element<Ass>&... xs) { f(a, xs...); };

        cartesian_for_each_mut(inner, ass...);
    }
}

// MapWithIndexRetrun

template<typename F, typename... Ass>
using MapWithIndexReturn = Conditional<
    _and({IsStaticSize<Ass>::value...}),
    Array<InvokeResult<F, size_t, Element<Ass>...>, _minimum({CtSize<Ass>::value...})>,
    Conditional<
        _and({IsStaticCapacity<Ass>::value...}),
        ArrVec<InvokeResult<F, size_t, Element<Ass>...>, _minimum({CtCapacity<Ass>::value...})>,
        Vector<InvokeResult<F, size_t, Element<Ass>...>>>>;

// map_with_index

template<typename... Ass, typename F = void (*)(size_t, const Element<Ass>&...)>
auto map_with_index(const F& f, const Ass&... ass) -> MapWithIndexReturn<F, Ass...> {
    auto res = MapWithIndexReturn<F, Ass...> {};
    const auto res_len = _min_length(ass...);

    if (CtSize<MapWithIndexReturn<F, Ass...>>::value == dyn) {
        res.resize(res_len);
    }

    for (size_t i = 0; i < res_len; ++i) {
        nth(i, res) = f(i, nth(i, ass)...);
    }

    return res;
}

// CartesianMapReturn

template<typename F, typename... Ass>
using CartesianMapReturn = Conditional<
    _and({IsStaticSize<Ass>::value...}),
    Array<InvokeResult<F, Element<Ass>...>, _product({CtSize<Ass>::value...})>,
    Conditional<
        _and({IsStaticCapacity<Ass>::value...}),
        ArrVec<InvokeResult<F, Element<Ass>...>, _product({CtCapacity<Ass>::value...})>,
        Vector<InvokeResult<F, Element<Ass>...>>>>;

// cartesian_map

// ! Maybe need to bechmark and optimize

template<typename... Ass, typename F = void (*)(const Element<Ass>&...)>
auto cartesian_map(const F& f, const Ass&... ass) -> CartesianMapReturn<F, Ass...> {
    auto res = CartesianMapReturn<F, Ass...> {};

    if (CtSize<CartesianMapReturn<F, Ass...>>::value == dyn) {
        res.resize(_product({static_cast<size_t>(length(ass))...}));
    }

    size_t i = 0;
    const auto inner = [&](const Element<Ass>&... xss) { nth(i++, res) = f(xss...); };

    cartesian_for_each(inner, ass...);

    return res;
}

// cartesian_for_index

template<typename F = void (*)(size_t)>
void cartesian_for_index(const F& f, size_t n) {
    for_index(f, n);
}

template<typename... Ints, typename F = void (*)(size_t)>
void cartesian_for_index(const F& f, size_t n, const Ints&... is) {
    for (size_t i = 0; i < n; ++i) {
        const auto inner = [&](const Ints&... is) { f(i, is...); };
        cartesian_for_index(inner, is...);
    }
}

// TakeUnsafeReturnImpl
// ! Size must be valid. It must be smaller than compile time and runtime size.
// ! Invalid size will be undifined behavior
namespace detail {
    template<typename N, typename As, bool is_const>
    struct TakeUnsafeReturnImpl {
        using Type = Conditional<
            IsStaticCapacity<As>::value,
            Conditional<
                is_const,
                ArrVecView<const Element<As>, CtCapacity<As>::value>,
                ArrVecView<Element<As>, CtCapacity<As>::value>>,
            Conditional<is_const, VectorView<const Element<As>>, VectorView<Element<As>>>>;
    };

    template<size_t n, typename As, bool is_const>
    struct TakeUnsafeReturnImpl<Size<n>, As, is_const> {
        using Type =
            Conditional<is_const, ArrayView<const Element<As>, n>, ArrayView<Element<As>, n>>;
    };
}  // namespace detail

// TakeUnsafeReturn

template<typename N, typename As, bool is_const>
using TakeUnsafeReturn = typename detail::TakeUnsafeReturnImpl<N, As, is_const>::Type;

// take_unsafe

// ! Should not put n longer than the length. Check should be done by the caller

template<typename N, typename As>
auto take_unsafe(N n, const As& as) -> TakeUnsafeReturn<N, As, true> {
    return TakeUnsafeReturn<N, As, true>(data(as), n);
}

template<typename N, typename As>
auto take_unsafe(N n, As& as) -> TakeUnsafeReturn<N, As, false> {
    return TakeUnsafeReturn<N, As, false>(data(as), n);
}

// TakeReturnImpl

namespace detail {
    template<typename N, typename As, bool is_const>
    struct TakeReturnImpl {
        using Type = Conditional<
            IsStaticCapacity<As>::value,
            Conditional<
                is_const,
                ArrVecView<const Element<As>, CtCapacity<As>::value>,
                ArrVecView<Element<As>, CtCapacity<As>::value>>,
            Conditional<is_const, VectorView<const Element<As>>, VectorView<Element<As>>>>;
    };

    template<size_t n, typename As, bool is_const>
    struct TakeReturnImpl<Size<n>, As, is_const> {
        static constexpr size_t bound_size = min(n, CtSize<As>::value);
        static constexpr size_t bound_capacity = min(n, CtCapacity<As>::value);

        using Type = Conditional<
            IsStaticSize<As>::value,
            Conditional<
                is_const,
                ArrayView<const Element<As>, bound_size>,
                ArrayView<Element<As>, bound_size>>,

            Conditional<
                is_const,
                ArrVecView<const Element<As>, bound_capacity>,
                ArrVecView<Element<As>, bound_capacity>>>;
    };
}  // namespace detail

// TakeReturn

template<typename N, typename As, bool is_const>
using TakeReturn = typename detail::TakeReturnImpl<N, As, is_const>::Type;

// take

template<typename N, typename As>
auto take(N n, const As& as) -> TakeReturn<N, As, true> {
    return TakeReturn<N, As, true>(
        data(as),
        min(static_cast<size_t>(n), static_cast<size_t>(length(as)))
    );  // Safeguarding against n > length(as)
}

template<typename N, typename As>
auto take(N n, As& as) -> TakeReturn<N, As, false> {
    return TakeReturn<N, As, false>(
        data(as),
        min(static_cast<size_t>(n), static_cast<size_t>(length(as)))
    );  // Safeguarding against n > length(as)
}

// DropUnsafeReturnImpl

namespace detail {
    template<typename N, typename As, bool is_const>
    struct DropUnsafeReturnImpl {
        using Type = Conditional<
            IsStaticCapacity<As>::value,
            Conditional<
                is_const,
                ArrVecView<const Element<As>, CtCapacity<As>::value>,
                ArrVecView<Element<As>, CtCapacity<As>::value>>,
            Conditional<is_const, VectorView<const Element<As>>, VectorView<Element<As>>>>;
    };

    template<size_t n, typename As, bool is_const>
    struct DropUnsafeReturnImpl<Size<n>, As, is_const> {
        using Type = Conditional<
            IsStaticSize<As>::value,
            Conditional<
                is_const,
                ArrayView<const Element<As>, CtSize<As>::value - n>,
                ArrayView<Element<As>, CtSize<As>::value - n>>,
            Conditional<
                IsStaticCapacity<As>::value,
                Conditional<
                    is_const,
                    ArrVecView<const Element<As>, CtCapacity<As>::value - n>,
                    ArrVecView<Element<As>, CtCapacity<As>::value - n>>,
                Conditional<is_const, VectorView<const Element<As>>, VectorView<Element<As>>>>>;
    };
}  // namespace detail

// DropUnsafeReturn

template<typename N, typename As, bool is_const>
using DropUnsafeReturn = typename detail::DropUnsafeReturnImpl<N, As, is_const>::Type;

// drop_unsafe

template<typename N, typename As>
auto drop_unsafe(N n, const As& as) -> DropUnsafeReturn<N, As, true> {
    return DropUnsafeReturn<N, As, true>(data(as) + n, length(as) - n);
}

template<typename N, typename As>
auto drop_unsafe(N n, As& as) -> DropUnsafeReturn<N, As, false> {
    return DropUnsafeReturn<N, As, false>(data(as) + n, length(as) - n);
}

// DropReturnImpl

namespace detail {
    template<typename N, typename As, bool is_const>
    struct DropReturnImpl {
        using Type = Conditional<
            IsStaticCapacity<As>::value,
            Conditional<
                is_const,
                ArrVecView<const Element<As>, CtCapacity<As>::value>,
                ArrVecView<Element<As>, CtCapacity<As>::value>>,
            Conditional<is_const, VectorView<const Element<As>>, VectorView<Element<As>>>>;
    };

    template<size_t n, typename As, bool is_const>
    struct DropReturnImpl<Size<n>, As, is_const> {
        static constexpr size_t bound_size = (CtSize<As>::value > n) ? (CtSize<As>::value - n) : 0;
        static constexpr size_t bound_capacity =
            (CtCapacity<As>::value > n) ? (CtCapacity<As>::value - n) : 0;

        using Type = Conditional<
            IsStaticSize<As>::value,
            Conditional<
                is_const,
                ArrayView<const Element<As>, bound_size>,
                ArrayView<Element<As>, bound_size>>,
            Conditional<
                IsStaticCapacity<As>::value,
                Conditional<
                    is_const,
                    ArrVecView<const Element<As>, bound_capacity>,
                    ArrVecView<Element<As>, bound_capacity>>,
                Conditional<is_const, VectorView<const Element<As>>, VectorView<Element<As>>>>>;
    };
}  // namespace detail

// DropReturn

template<typename N, typename As, bool is_const>
using DropReturn = typename detail::DropReturnImpl<N, As, is_const>::Type;

// drop

template<typename N, typename As>
auto drop(N n, const As& as) -> DropReturn<N, As, true> {
    const size_t as_len = length(as);
    const size_t bound_drop_size =
        (static_cast<size_t>(n) > as_len) ? as_len : n;  // Ensuring n doesn't exceed the size of as

    return DropReturn<N, As, true>(data(as) + bound_drop_size, as_len - bound_drop_size);
}

template<typename N, typename As>
auto drop(N n, As& as) -> DropReturn<N, As, false> {
    const size_t as_len = length(as);
    const size_t bound_drop_size =
        (static_cast<size_t>(n) > as_len) ? as_len : n;  // Ensuring n doesn't exceed the size of as

    return DropReturn<N, As, false>(data(as) + bound_drop_size, as_len - bound_drop_size);
}

// SliceUnsafeReturn

template<typename S, typename E, typename As, bool is_const>
using SliceUnsafeReturn = TakeUnsafeReturn<
    decltype(efp::declval<E>() - efp::declval<S>()),
    DropUnsafeReturn<S, As, is_const>,
    is_const>;

// slice_unsafe

// todo Optimization
template<typename S, typename E, typename As>
auto slice_unsafe(S start, E end, const As& as) -> SliceUnsafeReturn<S, E, As, true> {
    return SliceUnsafeReturn<S, E, As, true>(data(as) + start, end - start);
}

template<typename S, typename E, typename As>
auto slice_unsafe(S start, E end, As& as) -> SliceUnsafeReturn<S, E, As, false> {
    return SliceUnsafeReturn<S, E, As, false>(data(as) + start, end - start);
}

// SliceReturn

template<typename S, typename E, typename As, bool is_const>
using SliceReturn = TakeReturn<
    decltype(efp::declval<E>() - efp::declval<S>()),
    DropReturn<S, As, is_const>,
    is_const>;

// slice

// todo Optimization
template<typename S, typename E, typename As>
auto slice(S start, E end, const As& as) -> SliceReturn<S, E, As, true> {
    return SliceReturn<S, E, As, true>(data(as) + start, end - start);
}

template<typename S, typename E, typename As>
auto slice(S start, E end, As& as) -> SliceReturn<S, E, As, false> {
    return SliceReturn<S, E, As, false>(data(as) + start, end - start);
}

// take_while

template<typename As>
using TakeWhileReturn = Conditional<
    IsStaticCapacity<As>::value,
    ArrVecView<Element<As>, CtCapacity<As>::value>,
    VectorView<Element<As>>>;

template<typename As, typename F = bool (*)(const Element<As>&)>
auto take_while(const F& f, const As& as) -> TakeWhileReturn<As> {
    const auto as_len = length(as);

    size_t i = 0;
    while (i < as_len && f(nth(i, as))) {
        ++i;
    }

    return take(i, as);
}

// drop_while

template<typename As>
using DropWhileReturn = Conditional<
    IsStaticCapacity<As>::value,
    ArrVecView<Element<As>, CtCapacity<As>::value>,
    VectorView<Element<As>>>;

template<typename As, typename F = bool (*)(const Element<As>&)>
auto drop_while(const F& f, const As& as) -> DropWhileReturn<As> {
    const auto as_len = length(as);

    size_t i = 0;
    while (i < as_len && f(nth(i, as))) {
        ++i;
    }

    return drop(i, as);
}

// elem

template<typename As>
bool elem(const Element<As>& a, const As& as) {
    const auto as_len = length(as);

    for (size_t i = 0; i < as_len; ++i) {
        if (nth(i, as) == a) {
            return true;
        }
    }

    return false;
}

// find

template<typename As, typename F = bool (*)(const Element<As>&)>
auto find(const F& f, const As& as) -> Maybe<Element<As>> {
    const auto as_len = length(as);

    for (size_t i = 0; i < as_len; ++i) {
        const auto& a = nth(i, as);
        if (f(a)) {
            return a;
        }
    }

    return nothing;
}

// find_index

template<typename As, typename F = void (*)(const Element<As>&)>
auto find_index(const F& f, const As& as) -> Maybe<size_t> {
    const auto as_len = length(as);

    for (size_t i = 0; i < as_len; ++i) {
        if (f(nth(i, as))) {
            return i;
        }
    }

    return nothing;
}

// FindIndicesReturn

template<typename As>
using FindIndicesReturn =
    Conditional<IsStaticCapacity<As>::value, ArrVec<size_t, CtCapacity<As>::value>, Vector<size_t>>;

// find_indices

template<typename As, typename F = void (*)(const Element<As>&)>
auto find_indices(const F& f, const As& as) -> FindIndicesReturn<As> {
    FindIndicesReturn<As> res {};
    const auto as_len = length(as);

    for (size_t i = 0; i < as_len; ++i) {
        if (f(nth(i, as))) {
            res.push_back(i);
        }
    }

    return res;
}

}  // namespace efp

#endif