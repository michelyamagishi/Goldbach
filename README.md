Goldbach’s Conjecture (GC) is a well-known unproven mathematical statement. It asserts that every even integer,
 $n\geq 2$ can be expressed as the sum of two prime numbers. Assuming GC holds true, then for any even integer $n$, there exist prime 
 numbers $p$ and $q$ such that $n = p + q$. Let $p_n$ be the smallest prime number for which $q_n= n-p_n$ is also a prime. Define the following functions:
	$\pi(n)$ as the number of primes less than or equal to $n$, $\pi_2(n)$ as the number of twin primes less than or equal to $n$, $\delta_{6}(n)$ as the number of primes $p$ that $q-p \geq 6$, where $q$ is the next prime after $p$, $F_{n}(x)$ as the number of times that $p_k=x$ for every $6 \leq k \leq n$. Then, the following relationships hold: $\pi(n) = F_n(3)+1$, $\pi_2(n) =F_n(3)-F_n(5)$ and $\delta_6(n) = F_n(7)$

 Compile: g++ Goldbach_optimize.cc -o Goldbach_optimize -lz

 Run: ./Goldbach_optimize 1000000 result_1000000.gz

 Frequencies: python Goldbach_freq.py result_1000000.gz 1000000

 Output: $F_{1000000}(3)=78497$, $F_{1000000}(5)=70328$ and $F_{1000000}(7)=62185$
 
 Therefore, $\pi(1000000) = F_{1000000}(3)+1 = 78498$,   $\pi_2(1000000)=F_{1000000}(3) - F_{1000000}(5) = 8169$ and $\delta_6(1000000)=F_{1000000}(7)=62185$.
 
        
PS.: $F_n(5)$ is the number of primes $p \leq n$ such that $p+2$ is not a prime.



---------------------------------------------



Using the well-known fact that every prime number, except $2$ and $3$, is adjacent to a multiple of $6$ ($p=6k \pm 1 $, where $p$ is prime and $k \ge 1$ is an integer), we have developed a simple and fast algorithm to identify the prime numbers that form the sum. Take a look at https://oeis.org/A377540


python Goldbach.py <even number greater than 6>


