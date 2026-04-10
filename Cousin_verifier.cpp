#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Cousin_verifier
//
// This program exhaustively checks, for every even n with 10 <= n <= limit:
//
//   pi4(n-7)
//   =
//   pi(n-3) + pi(n-5) + pi(n-7) + H11(n)
//   - pi2(n-5) - pi2(n-7) - n/2
//
// where:
//   pi(x)   = number of primes p <= x
//   pi2(x)  = number of primes p <= x such that p+2 is prime
//   pi4(x)  = number of primes p <= x such that p+4 is prime
//   H11(n)  = number of even k in [4, n] such that k-3, k-5 and k-7 are all composite
//
// The main verifier is a one-pass odd-only segmented sieve. It never stores all
// primes up to the limit and never builds giant prefix tables. Instead, it keeps:
//   - base primes only up to sqrt(limit)
//   - one odd-only sieve segment
//   - a tiny rolling window of streamed counters
//
// That is enough because each update only depends on primality at m, m-2 and m-4.

using u64 = std::uint64_t;
using u32 = std::uint32_t;

namespace {

constexpr u64 DEFAULT_LIMIT = 10000000000ULL;
constexpr u64 SELF_CHECK_LIMIT = 1000000ULL;
// Segment size measured in odd numbers. The segment buffer stores one byte per odd.
constexpr std::size_t SEGMENT_ODD_COUNT = 1u << 24;
// Periodic reporting cadence in terms of the tested even n values.
constexpr u64 PROGRESS_EVERY = 100000000ULL;
constexpr int TABLE_COL_WIDTH = 14;

struct FailureInfo {
    u64 n = 0;
    u64 lhs = 0;
    u64 rhs = 0;
    u64 pi_n3 = 0;
    u64 pi_n5 = 0;
    u64 pi_n7 = 0;
    u64 pi2_n5 = 0;
    u64 pi2_n7 = 0;
    u64 pi4_n7 = 0;
    u64 H11_n = 0;
};

struct VerifyResult {
    bool ok = true;
    FailureInfo failure{};
};

void print_formula_table_header();
void print_formula_table_row(
    u64 n,
    u64 pi4_n7,
    u64 pi_n3,
    u64 pi_n5,
    u64 pi_n7,
    u64 H11_n,
    u64 pi2_n5,
    u64 pi2_n7,
    u64 rhs
);

[[noreturn]] void fail(const std::string& msg) {
    throw std::runtime_error(msg);
}

// Build odd base primes up to floor(sqrt(limit)) for the segmented sieve.
std::vector<u32> build_base_primes(const u32 limit) {
    std::vector<std::uint8_t> is_prime(limit + 1, 1);
    is_prime[0] = 0;
    if (limit >= 1) {
        is_prime[1] = 0;
    }
    for (u32 p = 2; static_cast<u64>(p) * p <= limit; ++p) {
        if (!is_prime[p]) {
            continue;
        }
        for (u32 q = p * p; q <= limit; q += p) {
            is_prime[q] = 0;
        }
    }

    std::vector<u32> primes;
    primes.reserve(limit / 10);
    for (u32 p = 3; p <= limit; p += 2) {
        if (is_prime[p]) {
            primes.push_back(p);
        }
    }
    return primes;
}

// Odd-only segmented sieve on [low_odd, high_odd], inclusive.
// flags[idx] corresponds to low_odd + 2*idx and is 1 iff that odd value is prime.
void sieve_segment_odd(
    const u64 low_odd,
    const u64 high_odd,
    const std::vector<u32>& base_primes,
    std::vector<std::uint8_t>& flags
) {
    const u64 count = ((high_odd - low_odd) >> 1) + 1;
    flags.assign(static_cast<std::size_t>(count), std::uint8_t{1});

    for (const u32 p32 : base_primes) {
        const u64 p = p32;
        const u64 pp = p * p;
        if (pp > high_odd) {
            break;
        }

        u64 start = pp;
        if (start < low_odd) {
            const u64 rem = low_odd % p;
            start = (rem == 0) ? low_odd : (low_odd + (p - rem));
        }
        if ((start & 1ULL) == 0) {
            start += p;
        }

        const u64 step = p << 1;
        for (u64 x = start; x <= high_odd; x += step) {
            flags[static_cast<std::size_t>((x - low_odd) >> 1)] = 0;
        }
    }

    if (low_odd == 1) {
        flags[0] = 0;
    }
}

// Main verifier.
//
// We stream odd values m = 7, 9, 11, ... and test the identity for n = m + 3.
// The key point is that the needed quantities can all be updated from just three
// primality bits:
//   I(m), I(m-2), I(m-4)
//
// Define:
//   P(m) = pi(m)
//   T(m) = pi2(m)
//   Q(m) = pi4(m)
//
// In a true one-pass stream, the naturally available updates are:
//   P(m)     = P(m-2) + I(m)
//   T(m-2)   = T(m-4) + I(m) I(m-2)
//   Q(m-4)   = Q(m-6) + I(m) I(m-4)
//   H11(m+3) = H11(m+1) + (1-I(m))(1-I(m-2))(1-I(m-4))
//
// so the verifier keeps exactly those delayed states and checks:
//   Q(m-4) = P(m) + P(m-2) + P(m-4) + H11(m+3) - T(m-2) - T(m-4) - (m+3)/2
VerifyResult verify_streaming(const u64 requested_limit, const bool print_table) {
    if (requested_limit < 10) {
        return {};
    }

    const u64 limit = requested_limit & ~1ULL;
    const u32 root = static_cast<u32>(std::sqrt(static_cast<long double>(limit - 3)) + 1.0L);
    const std::vector<u32> base_primes = build_base_primes(root);

    std::vector<std::uint8_t> segment;

    // We consume odd m in increasing order. At each step we only keep:
    //   pi_m_minus_2 = pi(m-2)
    //   pi_m_minus_4 = pi(m-4)
    //   pi2_m_minus_4 = pi2(m-4)
    //   pi4_m_minus_6 = pi4(m-6)
    //   h11_m_plus_1 = H11(m+1)
    // plus primality bits I(m-2), I(m-4).
    //
    // Then after reading I(m):
    //   pi(m)     = pi_m_minus_2 + I(m)
    //   pi2(m-2)  = pi2_m_minus_4 + I(m)I(m-2)
    //   pi4(m-4)  = pi4_m_minus_6 + I(m)I(m-4)
    //   H11(m+3)  = h11_m_plus_1 + (1-I(m))(1-I(m-2))(1-I(m-4))
    //
    // This delayed form is what is actually available in a one-pass stream.
    constexpr std::uint8_t I3 = 1;
    constexpr std::uint8_t I5 = 1;

    // Seed the rolling state so that the first streamed odd is m = 7,
    // corresponding to the first tested even value n = 10.
    u64 pi_m_minus_2 = 3;   // pi(5)
    u64 pi_m_minus_4 = 2;   // pi(3)
    u64 pi2_m_minus_4 = 1;  // pi2(3), contributed by prime 3 because 3+2=5 is prime
    u64 pi4_m_minus_6 = 0;  // pi4(1)
    u64 h11_m_plus_1 = 0;   // H11(8)

    std::uint8_t Im4 = I3;  // I(m-4) when m starts at 7
    std::uint8_t Im2 = I5;  // I(m-2) when m starts at 7

    const u64 max_m = limit - 3;
    u64 next_progress = PROGRESS_EVERY;
    bool printed_table_header = false;

    for (u64 low_odd = 7; low_odd <= max_m; ) {
        u64 high_odd = low_odd + 2 * (SEGMENT_ODD_COUNT - 1);
        if (high_odd > max_m) {
            high_odd = max_m;
        }
        if ((high_odd & 1ULL) == 0) {
            --high_odd;
        }

        sieve_segment_odd(low_odd, high_odd, base_primes, segment);

        const std::size_t count = segment.size();
        for (std::size_t idx = 0; idx < count; ++idx) {
            const u64 m = low_odd + (static_cast<u64>(idx) << 1);
            const std::uint8_t Im = segment[idx];

            const u64 pi_m = pi_m_minus_2 + Im;
            const u64 pi2_m_minus_2 = pi2_m_minus_4 + static_cast<u64>(Im) * Im2;
            const u64 pi4_m_minus_4 = pi4_m_minus_6 + static_cast<u64>(Im) * Im4;
            const u64 h11_m_plus_3 = h11_m_plus_1 +
                static_cast<u64>(1 - Im) * static_cast<u64>(1 - Im2) * static_cast<u64>(1 - Im4);

            const u64 n = m + 3;
            const u64 lhs = pi4_m_minus_4;
            const u64 rhs = pi_m + pi_m_minus_2 + pi_m_minus_4 + h11_m_plus_3
                          - pi2_m_minus_2 - pi2_m_minus_4 - (n >> 1);

            if (lhs != rhs) {
                VerifyResult result;
                result.ok = false;
                result.failure.n = n;
                result.failure.lhs = lhs;
                result.failure.rhs = rhs;
                result.failure.pi_n3 = pi_m;
                result.failure.pi_n5 = pi_m_minus_2;
                result.failure.pi_n7 = pi_m_minus_4;
                result.failure.pi2_n5 = pi2_m_minus_2;
                result.failure.pi2_n7 = pi2_m_minus_4;
                result.failure.pi4_n7 = pi4_m_minus_4;
                result.failure.H11_n = h11_m_plus_3;
                return result;
            }

            if (print_table && n >= next_progress) {
                if (!printed_table_header) {
                    print_formula_table_header();
                    printed_table_header = true;
                }
                print_formula_table_row(
                    n,
                    lhs,
                    pi_m,
                    pi_m_minus_2,
                    pi_m_minus_4,
                    h11_m_plus_3,
                    pi2_m_minus_2,
                    pi2_m_minus_4,
                    rhs
                );
                do {
                    next_progress += PROGRESS_EVERY;
                } while (next_progress <= n);
            }

            pi_m_minus_4 = pi_m_minus_2;
            pi_m_minus_2 = pi_m;
            pi2_m_minus_4 = pi2_m_minus_2;
            pi4_m_minus_6 = pi4_m_minus_4;
            h11_m_plus_1 = h11_m_plus_3;
            Im4 = Im2;
            Im2 = Im;
        }

        low_odd = high_odd + 2;
    }

    return {};
}

// Slow exact verifier used only for self-check mode.
// It builds full prefix arrays up to a small limit and compares against the same identity.
VerifyResult verify_naive(const u64 requested_limit) {
    if (requested_limit < 10) {
        return {};
    }

    const u64 limit = requested_limit & ~1ULL;
    std::vector<std::uint8_t> is_prime(limit + 1, 1);
    is_prime[0] = 0;
    if (limit >= 1) {
        is_prime[1] = 0;
    }
    for (u64 p = 2; p * p <= limit; ++p) {
        if (!is_prime[static_cast<std::size_t>(p)]) {
            continue;
        }
        for (u64 q = p * p; q <= limit; q += p) {
            is_prime[static_cast<std::size_t>(q)] = 0;
        }
    }

    std::vector<u64> pi(limit + 1, 0);
    std::vector<u64> pi2(limit + 1, 0);
    std::vector<u64> pi4(limit + 1, 0);

    for (u64 x = 1; x <= limit; ++x) {
        pi[x] = pi[x - 1] + (is_prime[static_cast<std::size_t>(x)] ? 1ULL : 0ULL);

        pi2[x] = pi2[x - 1];
        if (is_prime[static_cast<std::size_t>(x)] && x + 2 <= limit &&
            is_prime[static_cast<std::size_t>(x + 2)]) {
            ++pi2[x];
        }

        pi4[x] = pi4[x - 1];
        if (is_prime[static_cast<std::size_t>(x)] && x + 4 <= limit &&
            is_prime[static_cast<std::size_t>(x + 4)]) {
            ++pi4[x];
        }
    }

    u64 H = 0;
    for (u64 n = 10; n <= limit; n += 2) {
        if (!is_prime[static_cast<std::size_t>(n - 3)] &&
            !is_prime[static_cast<std::size_t>(n - 5)] &&
            !is_prime[static_cast<std::size_t>(n - 7)]) {
            ++H;
        }

        const u64 lhs = pi4[n - 7];
        const u64 rhs = pi[n - 3] + pi[n - 5] + pi[n - 7] + H - pi2[n - 5] - pi2[n - 7] - (n >> 1);
        if (lhs != rhs) {
            VerifyResult result;
            result.ok = false;
            result.failure.n = n;
            result.failure.lhs = lhs;
            result.failure.rhs = rhs;
            result.failure.pi_n3 = pi[n - 3];
            result.failure.pi_n5 = pi[n - 5];
            result.failure.pi_n7 = pi[n - 7];
            result.failure.pi2_n5 = pi2[n - 5];
            result.failure.pi2_n7 = pi2[n - 7];
            result.failure.pi4_n7 = pi4[n - 7];
            result.failure.H11_n = H;
            return result;
        }
    }

    return {};
}

void print_failure(const FailureInfo& info) {
    std::cout << "Failure at n = " << info.n << '\n'
              << "lhs = " << info.lhs << '\n'
              << "rhs = " << info.rhs << '\n'
              << "pi(n-3)  = " << info.pi_n3 << '\n'
              << "pi(n-5)  = " << info.pi_n5 << '\n'
              << "pi(n-7)  = " << info.pi_n7 << '\n'
              << "pi2(n-5) = " << info.pi2_n5 << '\n'
              << "pi2(n-7) = " << info.pi2_n7 << '\n'
              << "pi4(n-7) = " << info.pi4_n7 << '\n'
              << "H11(n)   = " << info.H11_n << '\n';
}

// Periodic output during the main run: one row per checkpoint showing every term
// that participates in the identity at that n.
void print_formula_table_header() {
    std::cout << std::right
              << std::setw(TABLE_COL_WIDTH) << "n"
              << std::setw(TABLE_COL_WIDTH) << "pi4(n-7)"
              << std::setw(TABLE_COL_WIDTH) << "pi(n-3)"
              << std::setw(TABLE_COL_WIDTH) << "pi(n-5)"
              << std::setw(TABLE_COL_WIDTH) << "pi(n-7)"
              << std::setw(TABLE_COL_WIDTH) << "H11(n)"
              << std::setw(TABLE_COL_WIDTH) << "pi2(n-5)"
              << std::setw(TABLE_COL_WIDTH) << "pi2(n-7)"
              << std::setw(TABLE_COL_WIDTH) << "n/2"
              << std::setw(TABLE_COL_WIDTH) << "rhs"
              << '\n';
}

void print_formula_table_row(
    const u64 n,
    const u64 pi4_n7,
    const u64 pi_n3,
    const u64 pi_n5,
    const u64 pi_n7,
    const u64 H11_n,
    const u64 pi2_n5,
    const u64 pi2_n7,
    const u64 rhs
) {
    std::cout << std::right
              << std::setw(TABLE_COL_WIDTH) << n
              << std::setw(TABLE_COL_WIDTH) << pi4_n7
              << std::setw(TABLE_COL_WIDTH) << pi_n3
              << std::setw(TABLE_COL_WIDTH) << pi_n5
              << std::setw(TABLE_COL_WIDTH) << pi_n7
              << std::setw(TABLE_COL_WIDTH) << H11_n
              << std::setw(TABLE_COL_WIDTH) << pi2_n5
              << std::setw(TABLE_COL_WIDTH) << pi2_n7
              << std::setw(TABLE_COL_WIDTH) << (n >> 1)
              << std::setw(TABLE_COL_WIDTH) << rhs
              << '\n';
}

u64 parse_limit_arg(const char* text) {
    char* end = nullptr;
    const unsigned long long value = std::strtoull(text, &end, 10);
    if (end == text || *end != '\0') {
        fail(std::string("invalid --limit value: ") + text);
    }
    return static_cast<u64>(value);
}

}  // namespace

int main(int argc, char** argv) {
    try {
        bool self_check = false;
        u64 limit = DEFAULT_LIMIT;

        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--self-check") {
                self_check = true;
            } else if (arg == "--limit") {
                if (i + 1 >= argc) {
                    fail("missing value after --limit");
                }
                limit = parse_limit_arg(argv[++i]);
            } else {
                fail("unknown argument: " + arg);
            }
        }

        const auto t0 = std::chrono::steady_clock::now();

        if (self_check) {
            const u64 bound = std::min(limit, SELF_CHECK_LIMIT);
            std::cout << "Running naive self-check up to " << bound << "...\n";
            const VerifyResult naive = verify_naive(bound);
            if (!naive.ok) {
                print_failure(naive.failure);
                return 1;
            }

            std::cout << "Running streaming verifier up to " << bound << "...\n";
            const VerifyResult stream = verify_streaming(bound, true);
            if (!stream.ok) {
                print_failure(stream.failure);
                return 1;
            }

            const auto t1 = std::chrono::steady_clock::now();
            const double elapsed = std::chrono::duration<double>(t1 - t0).count();
            std::cout << "Verified for all even n <= " << bound << '\n';
            std::cout << "Elapsed time: " << elapsed << " seconds\n";
            return 0;
        }

        std::cout << "Cousin_verifier streaming verification up to " << limit << "...\n";
        const VerifyResult result = verify_streaming(limit, true);
        if (!result.ok) {
            print_failure(result.failure);
            return 1;
        }

        const auto t1 = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration<double>(t1 - t0).count();
        std::cout << "Verified for all even n <= " << limit << '\n';
        std::cout << "Elapsed time: " << elapsed << " seconds\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 2;
    }
}
