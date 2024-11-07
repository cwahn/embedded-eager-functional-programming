#ifndef EFP_ENUM_HPP_
#define EFP_ENUM_HPP_

#include "efp/meta.hpp"

namespace efp {

    namespace detail {

        // Use bit operation and recursion
        constexpr uint8_t power_2_ceiling(uint8_t n, uint8_t power = 2) {
            return (power >= n) ? power - 1 : power_2_ceiling(n, power << 1);
        }

        // clang-format off
    #define EFP_STAMP2(n, x) x(n) x(n + 1)
    #define EFP_STAMP4(n, x) EFP_STAMP2(n, x) EFP_STAMP2(n + 2, x)
    #define EFP_STAMP8(n, x) EFP_STAMP4(n, x) EFP_STAMP4(n + 4, x)
    #define EFP_STAMP16(n, x) EFP_STAMP8(n, x) EFP_STAMP8(n + 8, x)
    #define EFP_STAMP32(n, x) EFP_STAMP16(n, x) EFP_STAMP16(n + 16, x)
    #define EFP_STAMP64(n, x) EFP_STAMP32(n, x) EFP_STAMP32(n + 32, x)
    #define EFP_STAMP128(n, x) EFP_STAMP64(n, x) EFP_STAMP64(n + 64, x)
    #define EFP_STAMP256(n, x) EFP_STAMP128(n, x) EFP_STAMP128(n + 128, x)

    // Generic case for pattern matching
    // Bound the index to the maximum alternative index to reduce the number of template instantiations
    #define EFP_ENUM_CASE(i) case i: return Case<i < alt_num ? i : alt_num - 1>::call(efp::forward<Args>(args)...); break;

        // clang-format on

        template <uint8_t stamp_n, uint8_t alt_num, template <uint8_t> class Case, typename... Args>
        struct _EnumSwitch {};

        template <uint8_t alt_num, template <uint8_t> class Case, typename... Args>
        struct _EnumSwitch<2 - 1, alt_num, Case, Args...> {
            static auto call(uint8_t index, Args&&... args)
                -> decltype(Case<0>::call(std::forward<Args>(args)...)) {
                switch (index) {
                    EFP_STAMP2(0, EFP_ENUM_CASE)
                default:
                    throw RuntimeError("Invalid alternative index");
                }
            }
        };

        template <uint8_t alt_num, template <uint8_t> class Case, typename... Args>
        struct _EnumSwitch<4 - 1, alt_num, Case, Args...> {
            static auto call(uint8_t index, Args&&... args)
                -> decltype(Case<0>::call(std::forward<Args>(args)...)) {
                switch (index) {
                    EFP_STAMP4(0, EFP_ENUM_CASE)
                default:
                    throw RuntimeError("Invalid alternative index");
                }
            }
        };

        template <uint8_t alt_num, template <uint8_t> class Case, typename... Args>
        struct _EnumSwitch<8 - 1, alt_num, Case, Args...> {
            static auto call(uint8_t index, Args&&... args)
                -> decltype(Case<0>::call(std::forward<Args>(args)...)) {
                switch (index) {
                    EFP_STAMP8(0, EFP_ENUM_CASE)
                default:
                    throw RuntimeError("Invalid alternative index");
                }
            }
        };

        template <uint8_t alt_num, template <uint8_t> class Case, typename... Args>
        struct _EnumSwitch<16 - 1, alt_num, Case, Args...> {
            static auto call(uint8_t index, Args&&... args)
                -> decltype(Case<0>::call(std::forward<Args>(args)...)) {
                switch (index) {
                    EFP_STAMP16(0, EFP_ENUM_CASE)
                default:
                    throw RuntimeError("Invalid alternative index");
                }
            }
        };

        template <uint8_t alt_num, template <uint8_t> class Case, typename... Args>
        struct _EnumSwitch<32 - 1, alt_num, Case, Args...> {
            static auto call(uint8_t index, Args&&... args)
                -> decltype(Case<0>::call(std::forward<Args>(args)...)) {
                switch (index) {
                    EFP_STAMP32(0, EFP_ENUM_CASE)
                default:
                    throw RuntimeError("Invalid alternative index");
                }
            }
        };

        template <uint8_t alt_num, template <uint8_t> class Case, typename... Args>
        struct _EnumSwitch<64 - 1, alt_num, Case, Args...> {
            static auto call(uint8_t index, Args&&... args)
                -> decltype(Case<0>::call(std::forward<Args>(args)...)) {
                switch (index) {
                    EFP_STAMP64(0, EFP_ENUM_CASE)
                default:
                    throw RuntimeError("Invalid alternative index");
                }
            }
        };

        template <uint8_t alt_num, template <uint8_t> class Case, typename... Args>
        struct _EnumSwitch<128 - 1, alt_num, Case, Args...> {
            static auto call(uint8_t index, Args&&... args)
                -> decltype(Case<0>::call(std::forward<Args>(args)...)) {
                switch (index) {
                    EFP_STAMP128(0, EFP_ENUM_CASE)
                default:
                    throw RuntimeError("Invalid alternative index");
                }
            }
        };

        template <uint8_t alt_num, template <uint8_t> class Case, typename... Args>
        struct _EnumSwitch<256 - 1, alt_num, Case, Args...> {
            static auto call(uint8_t index, Args&&... args)
                -> decltype(Case<0>::call(std::forward<Args>(args)...)) {
                switch (index) {
                    EFP_STAMP256(0, EFP_ENUM_CASE)
                default:
                    throw RuntimeError("Invalid alternative index");
                }
            }
        };

#undef EFP_ENUM_CASE

#undef EFP_STAMP2
#undef EFP_STAMP4
#undef EFP_STAMP8
#undef EFP_STAMP16
#undef EFP_STAMP32
#undef EFP_STAMP64
#undef EFP_STAMP128
#undef EFP_STAMP256

        template <uint8_t alt_num, template <uint8_t> class Case, typename... Args>
        using EnumSwitch = _EnumSwitch<power_2_ceiling(alt_num), alt_num, Case, Args...>;

        // todo Maybe support more than 256 alternatives

        // IsWildCard
        // The function or lambda with auto arguments is not considered as a wild card
        template <typename F>
        using IsWildCard = IsSame<Arguments<F>, Tuple<>>;

        // WildCardWrapper
        // Zero-copy wrapper for wild card
        template <typename F>
        class WildCardWrapper {
        public:
            // Constructor that initializes the reference. Note the explicit use of a reference to ensure the lambda is not copied.
            explicit WildCardWrapper(const F& lambda)
                : _lambda(lambda) {}

            // Overload the function call operator to forward calls to the lambda's own call operator.
            template <typename A>
            inline auto operator()(A&&) const -> decltype(efp::declval<F>()()) {
                return _lambda();
            }

        private:
            // A reference to the lambda
            const F& _lambda;
        };

        // MatchBranch
        // Wrap the wild card with WildCardWrapper, otherwise use the original callable
        template <typename F>
        using MatchBranch = Conditional<IsWildCard<F>::value, WildCardWrapper<F>, F>;

        template <typename F, typename = EnableIf<IsWildCard<F>::value>>
        WildCardWrapper<F> match_branch(const F& f) {
            return WildCardWrapper<F>(f);
        }

        // Template specialization for non-wild cards (zero-copy)
        template <typename F, typename = EnableIf<!IsWildCard<F>::value>>
        const F& match_branch(const F& f) {
            return f;
        }

        // BadParam
        struct BadParam {
            template <EnableIf<True::value, int> = 0, typename... Alts>
            BadParam(Alts&&...) {
                // clang-format off
            static_assert(AlwaysFalse<Alts...>::value, "No matching pattern found for the alternative");
                // clang-format on
            }
        };

        // Overloaded
        // Overloaded class for pattern matching
        // todo Make it zero-copy
        template <typename... Fs>
        struct Overloaded;

#if __cplusplus >= 201402L
        template <>
        struct Overloaded<> {
            // Better error message without extra compilation cost is only available in >= C++14
            template <typename... Alts, EnableIf<True::value, int> = 0>
            auto operator()(BadParam, int _ = 0, Alts&&...) const {}
        };
#else
        template <class F>
        struct Overloaded<F> : F {
            using F::operator();

            explicit Overloaded(const F& f)
                : F{f} {}
        };
#endif

        template <class F, class... Fs>
        struct Overloaded<F, Fs...> : F, Overloaded<Fs...> {
            using F::operator();
            using Overloaded<Fs...>::operator();

            explicit Overloaded(const F& f, const Fs&... fs)
                : F{f}, Overloaded<Fs...>{fs...} {}
        };

        // IsSameUnary
        template <typename Alt>
        struct IsSameUnary {
            template <typename Alt_>
            struct Binded {
                static const bool value = IsSame<Alt, Alt_>::value;
            };
        };

        // ! Temp IsCopyConstructibleWith
        template <typename Alt>
        struct IsAltCopyConstructible {
            template <typename A>
            using With = IsConstructible<Alt, const A&>;
        };

        // EnumBase
        // Enum is not permitted to hold a reference, but permitted to hold cv-qualified types
        template <typename A, typename... As>
        class EnumBase {
        public:
            constexpr static uint8_t alt_num = sizeof...(As) + 1;

            // Default constructor initializes the first alternative with default constructor
            EnumBase()
                : _index{0} {
                // new (reinterpret_cast<A*>(&_storage)) A {};
                new (_storage) A{};
            }

            EnumBase(const EnumBase& other)
                : _index{other._index} {
                EnumSwitch<alt_num, CopyCase, EnumBase*, const EnumBase&>::call(_index, this, other);
            }

            EnumBase& operator=(const EnumBase& other) {
                if (this != &other) {
                    EnumSwitch<alt_num, DestroyCase, EnumBase*>::call(_index, this);
                    _index = other._index;
                    EnumSwitch<alt_num, CopyCase, EnumBase*, const EnumBase&>::call(
                        _index,
                        this,
                        other);
                }
                return *this;
            }

            EnumBase(EnumBase&& other)
                : _index{other._index} {
                EnumSwitch<alt_num, MoveCase, EnumBase*, EnumBase&&>::call(
                    _index,
                    this,
                    efp::move(other));
            }

            EnumBase& operator=(EnumBase&& other) {
                if (this != &other) {
                    EnumSwitch<alt_num, DestroyCase, EnumBase*>::call(_index, this);
                    _index = other._index;
                    EnumSwitch<alt_num, MoveCase, EnumBase*, EnumBase&&>::call(
                        _index,
                        this,
                        efp::move(other));
                }
                return *this;
            }

            ~EnumBase() {
                EnumSwitch<alt_num, DestroyCase, EnumBase*>::call(_index, this);
            }

            template <
                typename Alt,
                typename = EnableIf<
                    _or({IsSame<Alt, ConstRemoved<A>>::value, IsSame<Alt, ConstRemoved<As>>::value...})>>
            EnumBase(const Alt& alt)
                : _index(CCAltIndex<Alt>::value) {
                new (reinterpret_cast<Alt*>(_storage)) Alt(alt);
            }

            template <
                typename Alt,
                typename = EnableIf<_or({IsSame<Alt, A>::value, IsSame<Alt, As>::value...})>>
            EnumBase(Alt&& alt)
                : _index(AltIndex<Alt>::value) {
                new (reinterpret_cast<Alt*>(&_storage)) Alt{efp::forward<Alt>(alt)};
            }

            bool operator==(const EnumBase& other) const {
                if (_index != other._index) {
                    return false;
                }

                return EnumSwitch<alt_num, EqCase, const EnumBase*, const EnumBase&>::call(
                    _index,
                    this,
                    other);
            }

            bool operator!=(const EnumBase& other) const {
                return !(*this == other);
            }

            uint8_t index() const {
                return _index;
            }

            template <typename Alt>
            const Alt& get() const {
                if (_index != AltIndex<Alt>::value) {
                    throw RuntimeError("Invalid alternative index");
                }

                return *reinterpret_cast<const Alt*>(&_storage);
            }

            template <typename Alt>
            Alt& get() {
                if (_index != AltIndex<Alt>::value) {
                    throw RuntimeError("Invalid alternative index");
                }

                return *reinterpret_cast<Alt*>(&_storage);
            }

            template <uint8_t i>
            const PackAt<i, A, As...>& get() const {
                if (_index != i) {
                    throw RuntimeError("Invalid alternative index");
                }

                return *reinterpret_cast<const PackAt<i, A, As...>*>(&_storage);
            }

            template <uint8_t i>
            PackAt<i, A, As...>& get() {
                if (_index != i) {
                    throw RuntimeError("Invalid alternative index");
                }

                return *reinterpret_cast<PackAt<i, A, As...>*>(&_storage);
            }

            template <typename Alt>
            const Alt&& move() const {
                if (_index != AltIndex<Alt>::value) {
                    throw RuntimeError("Invalid alternative index");
                }

                return efp::move(*reinterpret_cast<Alt*>(&_storage));
            }

            template <typename Alt>
            Alt&& move() {
                if (_index != AltIndex<Alt>::value) {
                    throw RuntimeError("Invalid alternative index");
                }

                return efp::move(*reinterpret_cast<Alt*>(&_storage));
            }

            template <uint8_t i>
            const PackAt<i, A, As...>&& move() const {
                if (_index != i) {
                    throw RuntimeError("Invalid alternative index");
                }

                return efp::move(*reinterpret_cast<PackAt<i, A, As...>*>(&_storage));
            }

            template <uint8_t i>
            PackAt<i, A, As...>&& move() {
                if (_index != i) {
                    throw RuntimeError("Invalid alternative index");
                }

                return efp::move(*reinterpret_cast<PackAt<i, A, As...>*>(&_storage));
            }

            // todo Improve signature to reduce copy of branches

            // match
            // Pattern matching
            template <typename F, typename... Fs>
            auto match(const F& f, const Fs&... fs) const
                -> InvokeResult<Overloaded<MatchBranch<F>, MatchBranch<Fs>...>, A> const {
                static_assert(PatternCheck<F, Fs...>::value, "Invalid pattern matching branches");

                using Pattern = Overloaded<MatchBranch<F>, MatchBranch<Fs>...>;

                return EnumSwitch<
                    alt_num,
                    Match<Pattern>::template Case,
                    const EnumBase*,
                    const Pattern&>::call(_index, this, Pattern{match_branch(f), match_branch(fs)...});
            }

        private:
            template <typename Alt>
            using AltIndex = FindIndex<IsSameUnary<ReferenceRemoved<Alt>>::template Binded, A, As...>;

            // ! Temp
            template <typename _A>
            using CCAltIndex = FindIndex<IsAltCopyConstructible<_A>::template With, A, As...>;

            // ! Temp
            template <typename _A>
            using CCAlt = PackAt<CCAltIndex<_A>::value, A, As...>;

            // Assume that the index is altready bounded
            template <uint8_t i>
            struct CopyCase {
                using Alt = PackAt<i, A, As...>;

                static inline void call(EnumBase* self, const EnumBase& other) {
                    // new (self->_storage) Alt {*reinterpret_cast<const Alt*>(&other._storage)};
                    new (reinterpret_cast<Alt*>(&self->_storage))
                        Alt{*reinterpret_cast<const Alt*>(&other._storage)};
                }
            };

            template <uint8_t i>
            struct MoveCase {
                using Alt = PackAt<i, A, As...>;

                static inline void call(EnumBase* self, EnumBase&& other) {
                    new (self->_storage) Alt{efp::move(*reinterpret_cast<Alt*>(&other._storage))};
                }
            };

            template <uint8_t i>
            struct DestroyCase {
                using Alt = PackAt<i, A, As...>;

                static inline void call(EnumBase* self) {
                    reinterpret_cast<Alt*>(&self->_storage)->~Alt();
                }
            };

            template <uint8_t i>
            struct EqCase {
                using Alt = PackAt<i, A, As...>;

                static inline bool call(const EnumBase* self, const EnumBase& other) {
                    return *reinterpret_cast<const Alt*>(&self->_storage) == *reinterpret_cast<const Alt*>(&other._storage);
                }
            };

            template <typename Overloaded>
            struct Match {
                template <uint8_t i>
                struct Case {
                    using Alt = PackAt<i, A, As...>;

                    static inline auto call(const EnumBase* self, const Overloaded& overloaded)
                        -> decltype(efp::declval<Overloaded>()(efp::declval<A>())) {
                        return overloaded(self->get<Alt>());
                    }
                };
            };

            // Match brach validation
            template <typename F>
            using IsRelevantBranch =
                Bool<_or({IsInvocable<F, A>::value, IsInvocable<F, As>::value...})>;

            template <typename F, typename... Fs>
            struct PatternCheck : Bool<
                                      IsRelevantBranch<F>::value && (!IsWildCard<F>::value) && PatternCheck<Fs...>::value> {};

            template <typename F>
            struct PatternCheck<F> : Bool<IsRelevantBranch<F>::value || IsWildCard<F>::value> {};

            // Private member variables
            alignas(_maximum({alignof(A), alignof(As)...})) uint8_t _storage[_maximum({sizeof(A), sizeof(As)...})];

            uint8_t _index; // Current maxinum variant is 256
        };

    } // namespace detail

    template <typename... As>
    class Enum : public detail::EnumBase<As...> {
    public:
        using Base = detail::EnumBase<As...>;
        using Base::Base;
    };

    // EnumAt

    namespace detail {
        template <size_t n, typename A>
        struct _EnumAt {};

        template <size_t n, typename... As>
        struct _EnumAt<n, Enum<As...>> {
            using Type = PackAt<n, As...>;
        };
    } // namespace detail

    template <size_t n, typename A>
    using EnumAt = typename detail::_EnumAt<n, A>::Type;

} // namespace efp

#endif // EFP_ENUM_HPP_