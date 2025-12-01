# This script computes and verifies the mathematical identity from Michel Yamagishi's work:
# For even n >= 8, π₂(n-5) = π(n-3) + π(n-5) + H(n) - n/2
# where π(n) is the prime counting function, π₂(n) is the twin prime counting function,
# and H(n) is the number of even k from 4 to n where the minimal Goldbach partition prime p(k) >= 7.
# The script uses the Sieve of Eratosthenes to generate primes up to n and finds minimal Goldbach partitions
# for each even k to compute H(n). It outputs a table verifying the identity for even values from 8 to n.

# Libraries used:
# - sys: For handling command-line arguments (e.g., reading the input value of n).
# - math: For mathematical functions like square root (used in the sieve).
# - bisect: For efficient binary search on sorted lists (used to compute π(n) via bisect_right on the primes list).

import sys
import math
import bisect

def sieve_of_eratosthenes(limit):
    # Generate all primes up to 'limit' using the Sieve of Eratosthenes.
    # Returns a list of primes and a boolean array indicating primality.
    if limit < 2:
        return [], []
    is_prime = [True] * (limit + 1)
    is_prime[0] = is_prime[1] = False
    for i in range(2, int(math.sqrt(limit)) + 1):
        if is_prime[i]:
            for j in range(i * i, limit + 1, i):
                is_prime[j] = False
    primes = [i for i in range(2, limit + 1) if is_prime[i]]
    return primes, is_prime

def main():
    # Parse command-line input: expect exactly one argument, the even n >= 8.
    if len(sys.argv) != 2:
        print("Usage: python VerifyFormula_H.py <even_n>")
        sys.exit(1)
    
    n = int(sys.argv[1])
    if n < 8 or n % 2 != 0:
        print("n must be even and >= 8")
        sys.exit(1)
    
    # Generate primes and primality array up to n using the sieve.
    primes, is_prime = sieve_of_eratosthenes(n)
    
    # Precompute cumulative twin prime counts (cum_pi2[i] = number of twin pairs among first i primes).
    cum_pi2 = [0] * (len(primes) + 1)
    for i in range(1, len(primes) + 1):
        cum_pi2[i] = cum_pi2[i - 1]
        if i >= 2 and primes[i - 1] - primes[i - 2] == 2:
            cum_pi2[i] += 1
    
    # List of even k from 4 to n.
    even_ks = list(range(4, n + 1, 2))
    p_ks = []  # Will store the minimal prime p(k) for each even k.
    
    for k in even_ks:
        found = False
        # Start from index 0 for k=4 (allows p=2), skip for larger k (since k-2 even, not prime).
        start_idx = 0 if k == 4 else 1
        for idx in range(start_idx, len(primes)):
            p = primes[idx]
            if p > k // 2:
                break  # No need to check larger p, as q = k - p would be < p.
            q = k - p
            if is_prime[q]:  # Check if q is prime using the array for O(1) lookup.
                p_ks.append(p)
                found = True
                break
        if not found:
            # If no partition found, exit (though unlikely under Goldbach conjecture).
            print(f"Goldbach partition not found for {k}")
            sys.exit(1)
    
    # Compute cumulative H(n): count of k where p(k) >= 7.
    cum_H = [0] * len(even_ks)
    current_h = 0
    
    for idx, k in enumerate(even_ks):
        p = p_ks[idx]
        if p >= 7:
            current_h += 1
        cum_H[idx] = current_h
    
    # Output the table header.
    print("n\tpi_2(n-5)\tpi(n-3)\tpi(n-5)\tH(n)\tn/2\tright_side\tformula_correct")
    
    # Find starting index for n >= 8.
    start_idx = even_ks.index(8)
    for idx in range(start_idx, len(even_ks)):
        current_n = even_ks[idx]
        
        # Compute left side: π₂(n-5) = number of twin pairs where the larger prime <= (n-5) + 2 = n-3.
        # Use bisect to find index of largest prime <= n-3, then get cum_pi2 at that index.
        pi2_idx = bisect.bisect_right(primes, current_n - 3)
        left = cum_pi2[pi2_idx]
        
        # Compute π(n-3) and π(n-5) using binary search on primes list.
        pi_n_minus_3 = bisect.bisect_right(primes, current_n - 3)
        pi_n_minus_5 = bisect.bisect_right(primes, current_n - 5)
        
        # Get H(n) from cumulative.
        h_n = cum_H[idx]
        
        # Compute n/2 (integer division since n even).
        n_over_2 = current_n // 2
        
        # Compute right side of the identity.
        right = pi_n_minus_3 + pi_n_minus_5 + h_n - n_over_2
        
        # Check if left == right.
        equal = "Yes" if left == right else "No"
        
        # Print the row.
        print(f"{current_n}\t{left}\t{pi_n_minus_3}\t{pi_n_minus_5}\t{h_n}\t{n_over_2}\t{right}\t{equal}")

if __name__ == "__main__":
    main()
