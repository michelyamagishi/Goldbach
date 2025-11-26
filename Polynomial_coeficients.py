#!/usr/bin/env python3
import sys
from bisect import bisect_right

# ------------------------------------------------------------
#  Prime utilities
# ------------------------------------------------------------
def sieve_up_to(N):
    """Return list of primes <= N using Sieve of Eratosthenes."""
    sieve = [True]*(N+1)
    sieve[0:2] = [False, False]
    for p in range(2, int(N**0.5)+1):
        if sieve[p]:
            step = p
            start = p*p
            sieve[start:N+1:step] = [False]*(((N-start)//step)+1)
    return [i for i, flag in enumerate(sieve) if flag], sieve


# ------------------------------------------------------------
# Compute coefficient for odd prime m
# ------------------------------------------------------------
def compute_c_m(n, m, primes, isprime):
    """
    Implements the definition:

    c_m = # { r prime | 3 ≤ r ≤ n − m, and
                     for all p in primes < m:
                         r + m − p NOT prime }
    """
    L = n - m
    # find all primes r <= L
    idx = bisect_right(primes, L)
    valid_primes = primes[:idx]

    # primes < m
    idxm = bisect_right(primes, m-1)
    small_primes = primes[:idxm]

    cnt = 0
    for r in valid_primes:
        if r < 3:
            continue
        ok = True
        for p in small_primes:
            if isprime[r + m - p]:
                ok = False
                break
        if ok:
            cnt += 1
    return cnt


# ------------------------------------------------------------
# Main
# ------------------------------------------------------------
def main():
    if len(sys.argv) < 2:
        print("Usage: python3 compute_cm_nonzero.py <n>")
        sys.exit(1)

    n = int(sys.argv[1])
    if n < 2:
        print("n must be >= 2")
        sys.exit(1)

    # We need primes up to 2n for shifted primality checks
    primes, isprime = sieve_up_to(2*n)

    # c2
    if n >= 4:
        print("c_2 = 1")

    # odd primes m
    for m in primes:
        if m < 3:
            continue
        if m > n:
            break
        cm = compute_c_m(n, m, primes, isprime)
        if cm != 0:
            print(f"c_{m} = {cm}")


if __name__ == "__main__":
    main()
