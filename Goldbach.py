## this new algorithm uses the result that (Q-2P) mod 6 is a periodic sequence
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
	periodo = [3, 5, 1]
	
	# Input an even number N
	if len(sys.argv) != 2:
		print("Usage: python GoldBach.py <even integer number>")
		return

	N = int(sys.argv[1])

	# Verify if N is even and greater than 6
	if (N<=6 or N%2 !=0):
		print("Odd number or <= 6. You should choose an even number greater than 2")
		return
	if is_prime(N-3):
        	print(f"N = P + F: {N} = {3} + {N-3}")
        	return
		
	
	Q = (N//6)

	for i in range(1,Q):

		# Verify if  P = 6 * i + 1 is prime
		P = 6 * i - 1
		if is_prime(P):
			F = N - P
			T = F % 6
			if ((T == 1 or T == 5) and (((F-2*P) % 6) == periodo[(N//2) % 3])):
				if is_prime(F):
					print(f"N = P + F: {N} = {P} + {F} and i = {i}")
					return

		# Verify if P = 6 * i - 1 is prime
		P = 6 * i + 1
		if is_prime(P):
			F = N - P
			T = F % 6
			if ((T == 1 or T == 5) and (((F-2*P) % 6) == periodo[(N//2) % 3])):
				if is_prime(F):
					print(f"N = P + F: {N} = {P} + {F} and i = {i}")
					return

	print(f"The number {N} cannot be expressed as the sum of two prime numbers. In this case, you have just disproved Goldbach's Conjecture. Congratulations!")

if __name__ == "__main__":
	main()
