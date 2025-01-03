import sys
import math

def is_prime(num):
    if num <= 1:
        return False
    if num <= 3:
        return True
    if num % 2 == 0 or num % 3 == 0:
        return False
    i = 5
    while i * i <= num:
        if num % i == 0 or num % (i + 2) == 0:
            return False
        i += 6
    return True

def main():
    # Input the maximum even number N
    if len(sys.argv) != 2:
        print("Usage: python Periodic_sequence_from_6_to_N.py <even integer number>")
        return

    N = int(sys.argv[1])

    # Verify if N is even and greater than or equal to 4
    if N < 4 or N % 2 != 0:
        print("You should choose an even number greater than or equal to 4")
        return

    # Loop through all even numbers from 4 to N
    for even_num in range(6, N + 1, 2):
        found = False

        # Loop: i from 1 to Q
        F = even_num // 6
        
        ## a(n) = (Q - 2 P) mod 6
        ## a(n) equivalent a(n+3) mod 6
        ## Ref: https://www.researchgate.net/publication/384406538_Goldbach's_Conjecture_A_Simple_and_Fast_Algorithm
        
        if is_prime(even_num-3):
            print(f"{even_num} = {3} + {even_num-3} -> a({even_num//2}) = {((even_num-3)-6) % 6} ")
            found = True

        if not found:

            for i in range(1, F):
                # Check P = 6 * i - 1
                P = 6 * i + 1
                if is_prime(P):
                    Q = even_num - P
                    T = Q % 6
                    if (T == 1 or T == 5):
                        if is_prime(Q):
                            print(f"{even_num} = {P} + {Q} -> a({even_num//2}) = {(Q-2*P) % 6 } ")
                            found = True
                            break

                # Check P = 6 * i + 1
                P = 6 * i - 1
                if is_prime(P):
                    Q = even_num - P
                    T = Q % 6
                    if (T == 1 or T == 5):
                        if is_prime(Q):
                            print(f"{even_num} = {P} + {Q} -> a({even_num//2}) = {(Q-2*P) % 6 }")
                            found = True
                            break

        if not found:
            print(f"{even_num} can't be written as the sum of two primes. "
                  "In this case, you've just disproved the Goldbach's Conjecture! "
                  "Congratulations!")

if __name__ == "__main__":
    main()

