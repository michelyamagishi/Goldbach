import math
import sys
from collections import Counter

def is_prime(num):
    if num <= 1: return False
    if num == 2: return True
    if num % 2 == 0: return False
    for i in range(3, int(math.sqrt(num)) + 1, 2):
        if num % i == 0: return False
    return True

def next_prime(p):
    p += 1 if p == 2 else 2
    while not is_prime(p):
        p += 2
    return p

def find_min_p(k):
    p = 2
    while not is_prime(k - p):
        p = next_prime(p)
    return p

def get_polynomial(n):
    if n < 4 or n % 2 != 0:
        return "Invalid n: n must be an even integer >= 4"
    exponents = []
    for k in range(4, n + 1, 2):
        exponents.append(find_min_p(k))
    counts = Counter(exponents)
    terms = []
    for exp in sorted(counts):
        coef = counts[exp]
        if coef == 1:
            term = f"x^{exp}"
        else:
            term = f"{coef}x^{exp}"
        terms.append(term)
    return " + ".join(terms)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <even_number_n>")
        sys.exit(1)
    try:
        n = int(sys.argv[1])
    except ValueError:
        print("Invalid input: n must be an integer")
        sys.exit(1)
    result = get_polynomial(n)
    print(result)
