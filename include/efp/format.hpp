#ifndef EFP_FORMAT_HPP_
#define EFP_FORMAT_HPP_

#define EFP_FMT_HEADER_ONLY
#include "efp/sequence.hpp"
#include "efp/string.hpp"
#include "efp/cyclic.hpp"
#include "efp/enum.hpp"

#include "efp/fmt/core.h"

namespace efp {

    template <typename... Args>
    using FormatString = efp_fmt::format_string<Args...>;

    template <typename... Args>
    inline auto format(FormatString<Args...> fmt, Args&&... args) -> String {
        return efp_fmt::vformat(fmt, efp_fmt::make_format_args(args...));
    }

    template <typename... Args>
    inline void print(FormatString<Args...> fmt, Args&&... args) {
        const auto& vargs = efp_fmt::make_format_args(args...);
        return efp_fmt::detail::is_utf8() ? efp_fmt::vprint(fmt, vargs)
                                          : efp_fmt::detail::vprint_mojibake(stdout, fmt, vargs);
    }

    // todo Move to io.hpp
    // todo Need to make it use Maybe or Result rather than exceptions
    // template<typename... Args>
    // inline void print(std::FILE* f, efp_fmt::format_string<Args...> fmt, Args&&... args) {
    //     const auto& vargs = efp_fmt::make_format_args(args...);
    //     return efp_fmt::detail::is_utf8() ? efp_fmt::vprint(f, fmt, vargs)
    //                                   : efp_fmt::detail::vprint_mojibake(f, fmt, vargs);
    // }

    template <typename... Args>
    inline void println(FormatString<Args...> fmt, Args&&... args) {
        return efp_fmt::println(stdout, fmt, efp::forward<Args>(args)...);
    }

    // todo Move to io.hpp
    // todo Need to make it use Maybe or Result rather than exceptions
    // template<typename... Args>
    // inline void println(std::FILE* f, efp_fmt::format_string<Args...> fmt, Args&&... args) {
    //     return efp_fmt::print(f, "{}\n", efp_fmt::format(fmt, efp::forward<Args>(args)...));
    // }

} // namespace efp

// Specialize efp_fmt::formatter for efp::Array
template <typename T, size_t ct_size>
struct efp_fmt::formatter<efp::Array<T, ct_size>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) const -> format_parse_context::iterator {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const efp::Array<T, ct_size>& arr, FormatContext& ctx) -> format_context::iterator {
        return efp_fmt::format_to(ctx.out(), "[{}]", efp_fmt::join(arr.begin(), arr.end(), ", "));
    }
};

// Specialize efp_fmt::formatter for efp::ArrVec
template <typename T, size_t ct_cap>
struct efp_fmt::formatter<efp::ArrVec<T, ct_cap>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) const -> format_parse_context::iterator {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const efp::ArrVec<T, ct_cap>& arrvec, FormatContext& ctx)
        -> format_context::iterator {
        return efp_fmt::format_to(
            ctx.out(),
            "[{}]",
            efp_fmt::join(arrvec.begin(), arrvec.end(), ", "));
    }
};

// Specialize efp_fmt::formatter for efp::Vector
template <typename T, typename Allocator, typename Traits>
struct efp_fmt::formatter<efp::Vector<T, Allocator, Traits>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) const -> format_parse_context::iterator {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const efp::Vector<T, Allocator, Traits>& vec, FormatContext& ctx)
        -> format_context::iterator {
        return efp_fmt::format_to(ctx.out(), "[{}]", efp_fmt::join(vec.begin(), vec.end(), ", "));
    }
};

// Sepecialize efp_fmt::formatter for efp::Vcb
template <typename T, size_t ct_size>
struct efp_fmt::formatter<efp::Vcb<T, ct_size>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) const -> format_parse_context::iterator {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const efp::Vcb<T, ct_size>& vcb, FormatContext& ctx) -> format_context::iterator {
        return efp_fmt::format_to(ctx.out(), "[{}]", efp_fmt::join(vcb.begin(), vcb.end(), ", "));
    }
};

// Specialize efp_fmt::formatter for efp::Vcq
template <typename T, size_t ct_cap>
struct efp_fmt::formatter<efp::Vcq<T, ct_cap>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) const -> format_parse_context::iterator {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const efp::Vcq<T, ct_cap>& vcq, FormatContext& ctx) -> format_context::iterator {
        return efp_fmt::format_to(ctx.out(), "[{}]", efp_fmt::join(vcq.begin(), vcq.end(), ", "));
    }
};

// todo enum support
// Specialize efp_fmt::formatter for efp::Enum
// template<typename... As>
// struct efp_fmt::formatter<efp::Enum<As...>> {
//     template<typename ParseContext>
//     constexpr auto parse(ParseContext& ctx) const -> format_parse_context::iterator {
//         return ctx.begin();
//     }

//     template<typename FormatContext>
//     auto format(const efp::Enum<As...>& enm, FormatContext& ctx) -> format_context::iterator {
//         return efp_fmt::format_to(ctx.out(), "{}", enm.to_string());
//     }
// };

#endif