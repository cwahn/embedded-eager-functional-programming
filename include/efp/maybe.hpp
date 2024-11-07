#ifndef MAYBE_HPP_
#define MAYBE_HPP_

#include "efp/enum.hpp"
#include "efp/meta.hpp"
#include "efp/trait.hpp"

namespace efp {

    struct Nothing {};

    constexpr Nothing nothing;

    // Specialization of Enum for Maybe

    template <typename A>
    using Maybe = Enum<Nothing, A>;

    template <typename A>
    struct IsMaybe : False {};

    template <typename A>
    struct IsMaybe<Maybe<A>> : True {};

    template <typename A>
    class Enum<Nothing, A> : public detail::EnumBase<Nothing, A> {
    public:
        using Base = detail::EnumBase<Nothing, A>;
        using Base::Base;

        bool is_nothing() const {
            return Base::index() == 0;
        }

        bool has_value() const {
            return Base::index() == 1;
        }

        // ! Will crash if empty
        A value() const {
            return Base::template get<A>();
        }

        A&& move() {
            return Base::template move<A>();
        }

        const A&& move() const {
            return Base::template move<A>();
        }

        explicit operator bool() const {
            return has_value();
        }

        template <typename F>
        static Enum<Nothing, A> pure(const A& a) {
            return a;
        }

        template <typename F>
        auto fmap(const F& f) -> Enum<Nothing, InvokeResult<F, A>> const {
            if (has_value())
                return f(value());
            else
                return nothing;
        }

        template <typename F>
        auto ap(const Enum<Nothing, F>& mf) -> Enum<Nothing, InvokeResult<F, A>> const {
            if (has_value() && mf.has_value())
                return mf.value()(value());
            else
                return nothing;
        }

        template <typename F>
        auto bind(const F& f)
            -> EnableIf<IsMaybe<InvokeResult<F, A>>::value, InvokeResult<F, A>> const {
            if (has_value())
                return f(value());
            else
                return nothing;
        }

        template <typename F>
        auto operator>>=(const F& f)
            -> EnableIf<IsMaybe<InvokeResult<F, A>>::value, InvokeResult<F, A>> const {
            if (has_value())
                return f(value());
            else
                return nothing;
        }
    };

    // Element specializetion for Maybe
    template <typename A>
    struct ElementImpl<Enum<Nothing, A>> {
        using Type = A;
    };

    // functor

    template <typename A, typename F>
    auto fmap(const F& f, const Maybe<A>& ma) -> Maybe<InvokeResult<F, A>> {
        if (ma)
            return f(ma.value());
        else
            return nothing;
    }

    // applicative
    template <typename A>
    auto pure(const Element<A>& a) -> EnableIf<IsMaybe<A>::value, A> {
        return a;
    }

    template <typename A, typename F>
    auto ap(const Maybe<F>& mf, const Maybe<A>& ma) -> Maybe<InvokeResult<F, A>> {
        if (mf && ma)
            return mf.value()(ma.value());
        else
            return nothing;
    }

    // monad

    template <typename A, typename F>
    auto bind(const Maybe<A>& ma, const F& f)
        -> EnableIf<IsMaybe<InvokeResult<F, A>>::value, InvokeResult<F, A>> {
        if (ma)
            return f(ma.value());
        else
            return nothing;
    }

} // namespace efp

#endif