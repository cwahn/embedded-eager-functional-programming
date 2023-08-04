#ifndef PRELUDE_HPP_
#define PRELUDE_HPP_

#include <array>
#include <vector>
#include <tuple>
#include <optional>

#include "sfinae.hpp"
#include "enum_type.hpp"
#include "sequence.hpp"
#include "maybe.hpp"

namespace efp
{
    struct Unit
    {
    };

    template <typename A>
    A id(const A &a)
    {
        return a;
    }

    // ! WIP
    // Composed

    // template <typename... Fs>
    // struct Composed;

    // Composed

    template <typename F, typename G>
    struct Composed
    {
        const F &f;
        const G &g;

        Composed(const F &f, const G &g) : f(f), g(g) {}

        template <typename Arg>

        auto operator()(Arg &&arg) const -> decltype(f(g(std::forward<Arg>(arg))))
        {
            return f(g(std::forward<Arg>(arg)));
        }
    };

    // template <typename F, typename... Fs>
    // struct Composed<F, Fs...>
    // {
    //     const F &f;
    //     const Composed<Fs...> fs;

    //     Composed(const F &f, const Fs &...fs) : f(f), fs(compose(fs...)) {}

    //     template <typename Arg>

    //     inline auto operator()(Arg &&arg) -> decltype(f(fs(std::forward<Arg>(arg))))
    //     {
    //         return f(fs(std::forward<Arg>(arg)));
    //     }
    // };

    // template <typename F, typename G>
    // inline auto operator*(F f, G g) -> Composed<F, G>
    // {
    //     return compose(f, g);
    // }

    // compose

    template <typename F, typename G>
    auto compose(const F &f, const G &g) -> Composed<F, G>
    {
        return Composed<F, G>(f, g);
    }

    // template <typename F, typename... Fs>
    // auto compose(const F &f, const Fs &...fs) -> Composed<F, Fs...>
    // {
    //     return Composed<F, Fs...>(f, fs...);
    // }

    // execute_pack

    template <typename... Args>
    void execute_pack(Args... args)
    {
    }

    // AppendSequence_t

    template <typename A, typename... Seqs>
    using AppendSequence_t =
        Conditional_t<
            all_v(IsStaticCapacity<Seqs>::value...),
            Conditional_t<
                all_v(IsStaticLength<Seqs>::value...),
                Array<A, sum_v(StaticCapacity<Seqs>::value...)>,
                ArrVec<A, sum_v(StaticCapacity<Seqs>::value...)>>,
            Vector<A>>;

    // AppendReturn_t
    template <typename Head, typename... Tail>
    using AppendReturn_t = AppendSequence_t<Element_t<Head>, Head, Tail...>;

    // append

    template <typename A, typename B>
    Unit append_(A &result, int &idx, const B &seq)
    {
        for (auto elem : seq)
        {
            result[idx] = elem;
            idx++;
        }
        return Unit{};
    }

    template <typename Head, typename... Tail>
    auto append(const Head &head, const Tail &...tail)
        -> EnableIf_t<
            all_v(IsStaticCapacity<Head>::value, IsStaticCapacity<Tail>::value...) && all_v(IsStaticLength<Head>::value, IsStaticLength<Tail>::value...),
            AppendReturn_t<Head, Tail...>>
    {
        AppendReturn_t<Head, Tail...> result;
        int idx{0};

        execute_pack(append_(result, idx, head), append_(result, idx, tail)...);

        return result;
    }

    template <typename Head, typename... Tail>
    auto append(const Head &head, const Tail &...tail)
        -> EnableIf_t<
            !all_v(IsStaticLength<Head>::value, IsStaticLength<Tail>::value...),
            AppendReturn_t<Head, Tail...>>
    {
        const size_t result_length = sum_v((int)length(head), (int)length(tail)...);

        AppendReturn_t<Head, Tail...> result(result_length);
        int idx{0};

        execute_pack(append_(result, idx, head), append_(result, idx, tail)...);

        return result;
    }

    // min_length

    template <typename SeqA>
    constexpr auto min_length(const SeqA &as)
        -> Conditional_t<
            IsStaticLength<SeqA>::value,
            decltype(length(as)),
            size_t>
    {
        return length(as);
    }

    template <typename Head, typename... Tail>
    constexpr auto min_length(const Head &head, const Tail &...tail)
        -> Conditional_t<
            // All<IsIntegralConst<Head>, IsIntegralConst<Tail>...>::value,
            all_v(IsIntegralConst<Head>::value, IsIntegralConst<Tail>::value...),
            decltype(length(head)),
            size_t>
    {
        return length(head) < min_length(tail...) ? length(head) : min_length(tail...);
    }

    // for_each

    template <typename... Seqs, typename F = void (*)(const Element_t<Seqs> &...)>
    void for_each(const F &f, const Seqs &...seqs)
    {
        // ? Will it be optimized out to a compile time constatnt?
        const int seq_length = min_length(seqs...);

        for (int i = 0; i < seq_length; ++i)
        {
            f(seqs[i]...);
        }
    }

    // for_eachi

    template <typename... Seqs, typename F = void (*)(Element_t<Seqs> &...)>
    void for_eachi(const F &f, Seqs &...seqs)
    {
        // ? Will it be optimized out to a compile time constatnt?
        const int seq_length = min_length(seqs...);

        for (int i = 0; i < seq_length; ++i)
        {
            f(seqs[i]...);
        }
    }

    // MapSequence_t

    template <typename A, typename... Seqs>
    using MapSequence_t =
        Conditional_t<
            all_v(IsStaticCapacity<Seqs>::value...),
            Conditional_t<
                all_v(IsStaticLength<Seqs>::value...),
                Array<A, MinStaticCapacity<Seqs...>::value>,
                ArrVec<A, MinStaticCapacity<Seqs...>::value>>,
            Vector<A>>;

    // MapReturn_t

    template <typename F, typename... Seqs>
    using MapReturn_t = MapSequence_t<CallReturn_t<F, Element_t<Seqs>...>, Seqs...>;

    // map

    template <typename F, typename... Seqs>
    auto map(const F &f, const Seqs &...seqs)
        -> EnableIf_t<
            all_v(IsStaticCapacity<Seqs>::value...) && all_v(IsStaticLength<Seqs>::value...),
            MapReturn_t<F, Seqs...>>
    {
        // using R = CallReturn_t<F, Element_t<Seqs>...>;

        MapReturn_t<F, Seqs...> result;

        for (int i = 0; i < MinStaticCapacity<Seqs...>::value; ++i)
        {
            result[i] = f(seqs[i]...);
        }

        return result;
    }

    template <typename F, typename... Seqs>
    auto map(const F &f, const Seqs &...seqs)
        -> EnableIf_t<
            !all_v(IsStaticLength<Seqs>::value...),
            MapReturn_t<F, Seqs...>>
    {
        const int result_length = min_length(seqs...);

        MapReturn_t<F, Seqs...> result(result_length);

        for (int i = 0; i < result_length; ++i)
        {
            result[i] = f(seqs[i]...);
        }

        return result;
    }

    // try to accept template function

    // template <typename... Seqs, typename R, typename F = R (*)(const Element_t<Seqs> &...)>
    // auto map(const F &f, const Seqs &...seqs)
    //     -> EnableIf_t<
    //         all_v(IsStaticCapacity<Seqs>::value...) && all_v(IsStaticLength<Seqs>::value...),
    //         MapReturn_t<decltype(f), Seqs...>>
    // {
    //     // using R = CallReturn_t<F, Element_t<Seqs>...>;

    //     MapReturn_t<decltype(f), Seqs...> result;

    //     for (int i = 0; i < MinStaticCapacity<Seqs...>::value; ++i)
    //     {
    //         result[i] = f(seqs[i]...);
    //     }

    //     return result;
    // }

    // template <typename... Seqs, typename R, typename F = R (*)(const Element_t<Seqs> &...)>
    // auto map(const F &f, const Seqs &...seqs)
    //     -> EnableIf_t<
    //         !all_v(IsStaticLength<Seqs>::value...),
    //         MapReturn_t<decltype(f), Seqs...>>
    // {
    //     const size_t result_length = min_length(seqs...);

    //     MapReturn_t<decltype(f), Seqs...> result(result_length);

    //     for (int i = 0; i < result_length; ++i)
    //     {
    //         result[i] = f(seqs[i]...);
    //     }

    //     return result;
    // }

    // MapWithIndexReturn_t

    template <typename F, typename... Seqs>
    using MapWithIndexReturn_t =
        MapSequence_t<CallReturn_t<F, int, Element_t<Seqs>...>, Seqs...>;

    // FilterReturn_t

    template <typename SeqA>
    using FilterReturn_t =
        Conditional_t<
            IsStaticCapacity<SeqA>::value,
            ArrVec<Element_t<SeqA>, StaticCapacity<SeqA>::value>,
            Vector<Element_t<SeqA>>>;

    // filter

    template <typename SeqA, typename F = bool (*)(Element_t<SeqA> &...)>
    auto filter(const F &f, const SeqA &as)
        -> EnableIf_t<
            IsStaticCapacity<SeqA>::value,
            ArrVec<Element_t<SeqA>, StaticCapacity<SeqA>::value>>
    {
        ArrVec<Element_t<SeqA>, StaticCapacity<SeqA>::value> result;

        for (int i = 0; i < length(as); ++i)
        {
            const auto a = as[i];
            if (f(a))
            {
                result.push_back(a);
            }
        }

        return result;
    }

    template <typename SeqA, typename F = bool (*)(Element_t<SeqA> &...)>
    auto filter(const F &f, const SeqA &as)
        -> EnableIf_t<
            !IsStaticCapacity<SeqA>::value,
            Vector<Element_t<SeqA>>>
    {
        const size_t sequance_length = length(as);

        Vector<Element_t<SeqA>> result;
        result.reserve(sequance_length * 2);

        for (int i = 0; i < sequance_length; ++i)
        {
            const auto a = as[i];
            if (f(a))
            {
                result.push_back(a);
            }
        }

        return result;
    }

    // foldl

    template <typename R, typename SeqA, typename F = R (*)(const R &, const Element_t<SeqA> &)>
    R foldl(const F &f, const R &initial_value, const SeqA &as)
    {
        R result = initial_value;

        for (int i = 0; i < length(as); ++i)
        {
            result = f(result, as[i]);
        }

        return result;
    }

    // foldr

    template <typename R, typename SeqA, typename F = R (*)(const Element_t<SeqA> &, const R &)>
    R foldr(const F &f, const R &initial_value, const SeqA &as)
    {
        R result = initial_value;

        for (int i = length(as) - 1; i > -1; --i)
        {
            result = f(as[i], result);
        }

        return result;
    }

    // FromFunctionReturn_t

    template <typename N, typename F>
    using FromFunctionReturn_t = Conditional_t<
        IsIntegralConst<N>::value,
        Array<CallReturn_t<F, int>, N::value>,
        Vector<CallReturn_t<F, int>>>;

    // from_function

    template <typename N, typename F>
    auto from_function(const N &length, const F &f)
        -> EnableIf_t<
            IsIntegralConst<N>::value,
            Array<CallReturn_t<F, int>, N::value>>
    {
        Array<CallReturn_t<F, int>, N::value> result;

        for (int i = 0; i < N::value; ++i)
        {
            result[i] = f(i);
        }

        return result;
    }

    template <typename N, typename F>
    auto from_function(const N &length, const F &f)
        -> EnableIf_t<
            !IsIntegralConst<N>::value,
            Vector<CallReturn_t<F, int>>>
    {
        Vector<CallReturn_t<F, int>> result(length);

        for (int i = 0; i < length; ++i)
        {
            result[i] = f(i);
        }

        return result;
    }

    // for_index

    template <typename F = void (*)(const int &)>
    void for_index(const F &f, const int i)
    {
        for (int i_ = 0; i_ < i; ++i_)
        {
            f(i_);
        }
    }

    // for_each_with_index

    template <typename... Seqs, typename F = void (*)(const int &, const Element_t<Seqs> &...)>
    void for_each_with_index(const F &f, const Seqs &...seqs)
    {
        // ? Will it be optimized out to a compile time constatnt?
        const int seq_length = min_length(seqs...);

        for (int i = 0; i < seq_length; ++i)
        {
            f(i, seqs[i]...);
        }
    }

    // for_each_with_indexi

    template <typename... Seqs, typename F = void (*)(const int &, Element_t<Seqs> &...)>
    void for_each_with_indexi(const F &f, Seqs &...seqs)
    {
        // ? Will it be optimized out to a compile time constatnt?
        const int seq_length = min_length(seqs...);

        for (int i = 0; i < seq_length; ++i)
        {
            f(i, seqs[i]...);
        }
    }

    // cartesian_for_each

    template <typename SeqA, typename F = void (*)(const Element_t<SeqA> &)>
    void cartesian_for_each(const F &f, const SeqA &as)
    {
        for_each(f, as);
    }

    template <typename SeqA, typename... Seqs, typename F = void (*)(const Element_t<SeqA> &, const Element_t<Seqs> &...)>
    void cartesian_for_each(const F &f, const SeqA &as, const Seqs &...seqs)
    {
        // ? Will it be optimized out to a compile time constatnt?
        const int as_length = length(as);
        Element_t<SeqA> a;

        for (int i = 0; i < as_length; ++i)
        {
            a = as[i];
            const auto inner = [=](Element_t<Seqs>... xs)
            {
                f(a, xs...);
            };

            cartesian_for_each<Seqs..., decltype(inner)>(inner, seqs...);
        }
    }

    // cartesian_for_eachi

    template <typename SeqA, typename F = void (*)(Element_t<SeqA> &)>
    void cartesian_for_eachi(const F &f, SeqA &as)
    {
        for_eachi(f, as);
    }

    template <typename SeqA, typename... Seqs, typename F = void (*)(Element_t<SeqA> &, Element_t<Seqs> &...)>
    void cartesian_for_eachi(const F &f, SeqA &as, Seqs &...seqs)
    {
        // ? Will it be optimized out to a compile time constatnt?
        const int as_length = length(as);
        Element_t<SeqA> a;

        for (int i = 0; i < as_length; ++i)
        {
            a = as[i];
            const auto inner = [=](Element_t<Seqs>... xs)
            {
                f(a, xs...);
            };

            cartesian_for_eachi<decltype(inner), Seqs...>(inner, seqs...);
        }
    }

    // map_with_index

    template <typename F, typename... Seqs>
    auto map_with_index(const F &f, const Seqs &...seqs)
        -> EnableIf_t<
            all_v(IsStaticCapacity<Seqs>::value...) && all_v(IsStaticLength<Seqs>::value...),
            Array<CallReturn_t<F, int, Element_t<Seqs>...>, MinStaticCapacity<Seqs...>::value>>
    {
        using R = CallReturn_t<F, int, Element_t<Seqs>...>;

        Array<R, MinStaticCapacity<Seqs...>::value> result;

        for (int i = 0; i < MinStaticCapacity<Seqs...>::value; ++i)
        {
            result[i] = f(i, seqs[i]...);
        }

        return result;
    }

    template <typename F, typename... Seqs>
    auto map_with_index(const F &f, const Seqs &...seqs)
        -> EnableIf_t<
            all_v(IsStaticCapacity<Seqs>::value...) && !all_v(IsStaticLength<Seqs>::value...),
            ArrVec<CallReturn_t<F, int, Element_t<Seqs>...>, MinStaticCapacity<Seqs...>::value>>
    {
        using R = CallReturn_t<F, int, Element_t<Seqs>...>;

        const int result_length = min_length(seqs...);

        ArrVec<R, MinStaticCapacity<Seqs...>::value> result(result_length);

        for (int i = 0; i < result_length; ++i)
        {
            result[i] = f(i, seqs[i]...);
        }

        return result;
    }

    template <typename F, typename... Seqs>
    auto map_with_index(const F &f, const Seqs &...seqs)
        -> EnableIf_t<
            !all_v(IsStaticCapacity<Seqs>::value...),
            Vector<CallReturn_t<F, int, Element_t<Seqs>...>>>
    {
        using R = CallReturn_t<F, int, Element_t<Seqs>...>;

        const int result_length = min_length(seqs...);

        Vector<R> result(result_length);

        for (int i = 0; i < result_length; ++i)
        {
            result[i] = f(i, seqs[i]...);
        }

        return result;
    }

    // cartesian_map
    // ! Maybe need to bechmark and optimize
    template <typename F, typename... Seqs>
    auto cartesian_map(const F &f, const Seqs &...seqs)
        -> EnableIf_t<
            all_v(IsStaticCapacity<Seqs>::value...) && all_v(IsStaticLength<Seqs>::value...),
            Array<CallReturn_t<F, Element_t<Seqs>...>, StaticCapacityProduct<Seqs...>::value>>
    {
        using R = CallReturn_t<F, Element_t<Seqs>...>;

        Array<R, StaticCapacityProduct<Seqs...>::value> result;
        int i = 0;

        const auto inner = [&](Element_t<Seqs>... xs)
        {
            result[i++] = f(xs...);
        };

        cartesian_for_each(inner, seqs...);

        return result;
    }

    template <typename F, typename... Seqs>
    auto cartesian_map(const F &f, const Seqs &...seqs)
        -> EnableIf_t<
            all_v(IsStaticCapacity<Seqs>::value...) && !all_v(IsStaticLength<Seqs>::value...),
            ArrVec<CallReturn_t<F, Element_t<Seqs>...>, StaticCapacityProduct<Seqs...>::value>>
    {
        using R = CallReturn_t<F, Element_t<Seqs>...>;

        const size_t result_length = size_v_product(length(seqs)...);

        ArrVec<R, StaticCapacityProduct<Seqs...>::value> result(result_length);
        int i = 0;

        const auto inner = [&](Element_t<Seqs>... xs)
        {
            result[i++] = f(xs...);
        };

        cartesian_for_each(inner, seqs...);

        return result;
    }

    template <typename F, typename... Seqs>
    auto cartesian_map(const F &f, const Seqs &...seqs)
        -> EnableIf_t<
            !all_v(IsStaticCapacity<Seqs>::value...),
            Vector<CallReturn_t<F, Element_t<Seqs>...>>>
    {
        using R = CallReturn_t<F, Element_t<Seqs>...>;

        const size_t result_length = size_v_product(length(seqs)...);

        Vector<R> result(result_length);
        int i = 0;

        const auto inner = [&](Element_t<Seqs>... xs)
        {
            result[i++] = f(xs...);
        };

        cartesian_for_each(inner, seqs...);

        return result;
    }

    // todo begin

    // todo end

    // todo head
    // ! Partial function. Make sure non empty

    // template <typename SeqA>
    // A head(const SeqA &as)
    // {
    //     return *begin(as);
    // }

    // todo last
    // ! Partial function. Make sure non empty

    // template <typename SeqA>
    // A head(const SeqA &as)
    // {
    //     return *(end(as) - 1);
    // }

    // todo tail

    // todo init

    // todo is_null

    // using View_t = Conditional_t<
    //     template <typename SeqA>
    //     IsStaticLength<SeqA>::value,
    //     ArrayView<Element_t<SeqA>>,
    //     VectorView<Element_t<SeqA>>>;

    // todo take

    // drop

    template <typename N, typename SeqA>
    auto drop(const N &n, SeqA &as)
        -> EnableIf_t<
            IsIntegralConst<N>::value && IsStaticLength<SeqA>::value,
            ArrayView<Element_t<SeqA>, StaticLength<SeqA>::value - N::value>>
    {
        // ! What if larger than n? maybe last with 0?
        return ArrayView<Element_t<SeqA>, StaticLength<SeqA>::value - N::value>{data(as) + n};
    }

    template <typename N, typename SeqA>
    auto drop(const N &n, const SeqA &as)
        -> EnableIf_t<
            IsIntegralConst<N>::value && IsStaticLength<SeqA>::value,
            ArrayView<const Element_t<SeqA>, StaticLength<SeqA>::value - N::value>>
    {
        return ArrayView<const Element_t<SeqA>, StaticLength<SeqA>::value - N::value>{data(as) + n};
    }

    template <typename N, typename SeqA>
    auto drop(const N &n, SeqA &as)
        -> EnableIf_t<
            !IsIntegralConst<N>::value && IsStaticLength<SeqA>::value,
            VectorView<Element_t<SeqA>>>
    {
        // ! What if larger than n? maybe last with 0?
        return VectorView<Element_t<SeqA>>{data(as) + n, length(as) - n};
    }

    template <typename N, typename SeqA>
    auto drop(const N &n, const SeqA &as)
        -> EnableIf_t<
            !IsIntegralConst<N>::value && IsStaticLength<SeqA>::value,
            VectorView<const Element_t<SeqA>>>
    {
        return VectorView<const Element_t<SeqA>>{data(as) + n, length(as) - n};
    }

    // elem

    template <typename SeqA>
    bool elem(const Element_t<SeqA> &a, const SeqA &as)
    {
        const int length_as = length(as);

        for (int i = 0; i < length_as; ++i)
        {
            if (as[i] == a)
            {
                return true;
            }
        }

        return false;
    }

    // elem_index

    template <typename SeqA>
    Maybe<int> elem_index(const Element_t<SeqA> &a, const SeqA &as)
    {
        const int length_as = length(as);

        for (int i = 0; i < length_as; ++i)
        {
            if (as[i] == a)
            {
                return i;
            }
        }

        return Nothing{};
    }

    // ElemIndicesReturn_t

    template <typename SeqA>
    using ElemIndicesReturn_t =
        Conditional_t<
            IsStaticCapacity<SeqA>::value,
            ArrVec<int, StaticCapacity<SeqA>::value>,
            Vector<int>>;

    // elem_indices

    template <typename SeqA>
    auto elem_indices(const Element_t<SeqA> &a, const SeqA &as)
        -> EnableIf_t<
            IsStaticCapacity<SeqA>::value,
            ElemIndicesReturn_t<SeqA>>
    {
        ElemIndicesReturn_t<SeqA> result;

        for (int i = 0; i < length(as); ++i)
        {
            if (a == as[i])
            {
                result.push_back(i);
            }
        }

        return result;
    }

    template <typename SeqA>
    auto elem_indices(const Element_t<SeqA> &a, const SeqA &as)
        -> EnableIf_t<
            !IsStaticCapacity<SeqA>::value,
            ElemIndicesReturn_t<SeqA>>
    {
        const size_t sequance_length = length(as);

        ElemIndicesReturn_t<SeqA> result;
        result.reserve(sequance_length * 2);

        for (int i = 0; i < sequance_length; ++i)
        {
            if (a == as[i])
            {
                result.push_back(i);
            }
        }

        return result;
    }

    // find

    // find_index

    // find_indicies

    // take

    // template <typename SeqA>
    // VectorView<Element_t<SeqA>> take(int n, SeqA &as)
    // {
    //     const auto length_as = length(as);
    //     const auto length = n < length_as ? n : length_as;

    //     return VectorView<Element_t<SeqA>>(&(as[0]), length);
    // }
}
#endif