#ifndef SCIENTIFIC_HPP_
#define SCIENTIFIC_HPP_

#include "efp/numeric.hpp"
#include "efp/prelude.hpp"

// todo Make default return type automation
// todo Separate to efp-scientific library

namespace efp {
    template <typename R, typename As>
    constexpr R mean(const As& as) {
        return sum(as) / (R)length(as);
    }

    template <typename R, typename As>
    constexpr R rms(const As& as) {
        return sqrt<R>(mean<R>(map(square<Element<As>>, as)));
    }

    template <typename R, typename As, typename Bs>
    R sse(const As& as, const Bs& bs) {
        const auto square_error = [](const Element<As>& a, const Element<Bs>& b) {
            return square(a - b);
        };

        return sum(map(square_error, as, bs));
    }

    template <typename R, typename As, typename Bs>
    R mse(const As& as, const Bs& bs) {
        const auto square_error = [](const Element<As>& a, const Element<Bs>& b) {
            return square(a - b);
        };

        return mean<R>(map(square_error, as, bs));
    }

    template <typename R, typename As, typename Bs>
    R rmse(const As& as, const Bs& bs) {
        return sqrt(mse<R>(as, bs));
    }

    template <typename R, typename As, typename Bs>
    R nrmse_mean(const As& as, const Bs& bs) {
        return rmse<R>(as, bs) / (R)mean<R>(bs);
    }

    template <typename R, typename As, typename Bs>
    R nrmse_max_min(const As& as, const Bs& bs) {
        return rmse<R>(as, bs) / (R)max_min(bs);
    }

    template <typename R, bool bessel_correction = false, typename As>
    R variance(const As& as) {
        const auto as_mean = mean<R>(as);

        auto square_deviation = [&](const Element<As>& x) { return square(x - as_mean); };

        return bessel_correction ? sum(map(square_deviation, as)) / (R)(length(as) - 1)
                                 : sum(map(square_deviation, as)) / (R)length(as);
    }

    template <typename R, bool bessel_correction = false, typename As>
    R standard_deviation(const As& as) {
        return sqrt(variance<R, bessel_correction>(as));
    }

    template <typename R, bool bessel_correction = false, typename As, typename Bs>
    R covariance(const As& as, const Bs& bs) {
        const auto as_mean = mean<R>(as);
        const auto bs_mean = mean<R>(bs);

        const auto covar = [&](const Element<As>& a, const Element<Bs>& b) {
            return (a - as_mean) * (b - bs_mean);
        };

        return bessel_correction ? sum(map(covar, as, bs)) / (R)(length(as) - 1)
                                 : sum(map(covar, as, bs)) / (R)length(as);
    }

    template <typename R, bool bessel_correction = false, typename As, typename Bs>
    R correlation(const As& as, const Bs& bs) {
        return covariance<R, bessel_correction>(as, bs) / (standard_deviation<R, bessel_correction>(as) * standard_deviation<R, bessel_correction>(bs));
    }

    template <typename R, bool bessel_correction = false, bool adjusted = false, typename As>
    R autocovariance(const As& as, const int& lag) {
        const auto as_mean = mean<R>(as);
        const auto n = length(as);
        const int sum_length = n - lag;
        R summation = 0;

        for_index(
            [&](const int& i) { summation += (nth(i, as) - as_mean) * (as[i + lag] - as_mean); },
            sum_length);

        if (adjusted) {
            return bessel_correction ? summation / (R)(n - lag - 1) : summation / (R)(n - lag);
        } else {
            return bessel_correction ? summation / (R)(n - 1) : summation / (R)n;
        }
    }

    template <typename R, bool bessel_correction = false, bool adjusted = false, typename As>
    R autocorrelation(const As& as, const int& lag) {
        return autocovariance<R, bessel_correction, adjusted>(as, lag) / variance<R, bessel_correction>(as);
    }

    template <typename R, typename As>
    NAryReturn<R, As> remove_dc(const As& as) {
        const auto as_mean = mean<R>(as);

        const auto minus_mean_as = [&](const Element<As>& x) { return (R)x - as_mean; };

        return map(minus_mean_as, as);
    }

    template <typename R, typename As, typename Bs>
    Tuple<R, R> linear_regression(const As& as, const Bs& bs) {
        const auto as_mean = mean<R>(as);
        const auto bs_mean = mean<R>(bs);

        const auto minus_mean_as = [&](const Element<As>& x) { return (R)x - as_mean; };
        const auto minus_mean_bs = [&](const Element<Bs>& x) { return (R)x - bs_mean; };

        const auto a_deviations = map(minus_mean_as, as);
        const auto b_deviations = map(minus_mean_bs, bs);

        const auto ss_ab = sum(map(op_mul<R>, a_deviations, b_deviations));
        const auto ss_aa = sum(map(square<R>, a_deviations));

        const auto beta_1 = ss_ab / ss_aa;
        const auto beta_2 = bs_mean - (beta_1 * as_mean);

        return tuple(beta_1, beta_2);
    }

    template <typename R, typename As>
    Tuple<R, R> linear_regression_with_index(const As& as) {
        const int n = length(as);

        const auto mean_is = (n - 1) / 2.;
        const auto as_mean = mean<R>(as);

        const auto get_ss_ia = [&](const int& i, const Element<As>& a) {
            return (i - mean_is) * ((R)a - as_mean);
        };

        const auto ss_ia = sum(map_with_index(get_ss_ia, as));
        const auto ss_ii = n * (n * n - 1) / 12.;

        const auto beta_1 = ss_ia / ss_ii;
        const auto beta_2 = as_mean - (beta_1 * mean_is);

        return tuple(beta_1, beta_2);
    }

    template <typename R, typename As>
    NAryReturn<R, As> detrend(const As& as) {
        const auto betas = linear_regression_with_index<R>(as);
        const auto beta_1 = get<0>(betas);
        const auto beta_2 = get<1>(betas);

        const auto detrend_elem = [&](const Element<As>& i, const Element<As>& x) {
            return (R)x - (beta_1 * (R)i + beta_2);
        };

        return map_with_index(detrend_elem, as);
    }
} // namespace efp

#endif