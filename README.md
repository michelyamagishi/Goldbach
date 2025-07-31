Goldbach's Conjecture and how to calculate $\pi(n)$ and $\pi_2(n)$

-----------------------------------------------------

Goldbach’s Conjecture (GC) is a well-known unproven mathematical statement. It asserts that every even integer,
 $n\geq 4$ can be expressed as the sum of two prime numbers. Assuming GC holds true, then for any even integer $n$, there exist prime 
 numbers $p$ and $q$ such that $n = p + q$. Let $p_n$ be the smallest prime number for which $q_n= n-p_n$ is also a prime. Define the following functions:
	$\pi(n)$ as the number of primes less than or equal to $n$, $\pi_2(n)$ as the number of twin primes less than or equal to $n$, $\delta_{6}(n)$ as the number of primes $p$ that $q-p \geq 6$, where $q$ is the next prime after $p$, $F_{n}(x)$ as the number of times that $p_k=x$ for every $k$, where $6 \leq k \leq n$, in other words, $F_{n}(x)$ counts how often a prime $x$ appears as the smallest prime in the Goldbach partition for even integers $k$ within the range $6 \leq k \leq n$. Then, the following relationships hold: 
 
 (i) $\pi(n) = F_n(3)+1$, 
 
 (ii) $\pi_2(n) =F_n(3)-F_n(5)$ and 
 
 (iii) $\delta_6(n) = F_n(7)$

-----------------------------------------------------

 Compile: g++ Goldbach_optimize.cc -o Goldbach_optimize -lz

 Run: ./Goldbach_optimize 1000000 result_1000000.gz

 Frequencies: python Goldbach_freq.py result_1000000.gz 1000000

 Output: $F_{1000000}(3)=78497$, $F_{1000000}(5)=70328$ and $F_{1000000}(7)=62185$
 
 Therefore, $\pi(1000000) = F_{1000000}(3)+1 = 78498$,   $\pi_2(1000000)=F_{1000000}(3) - F_{1000000}(5) = 8169$ and $\delta_6(1000000)=F_{1000000}(7)=62185$.
 
        
PS.: $F_n(5)$ is the number of primes $p \leq n$ such that $p+2$ is not a prime.

------------------------------------------------------

Simple Example:

n = $p_n$ + $q_n$

4 = 2 + 2,

6 = 3 + 3,

8 = 3 + 5,

10 = 3 + 7,

12 = 5 + 7,

14 = 3 + 11,

16 = 3 + 13,

18 = 5 + 13,

20 = 3 + 17,

22 = 3 + 19,

24 = 5 + 19,

26 = 3 + 23,

28 = 5 + 23,

30 = 7 + 23,

32 = 3 + 29,

34 = 3 + 31,

36 = 5 + 31,

38 = 7 + 31,

40 = 3 + 37,

42 = 5 + 37,

44 = 3 + 41,

46 = 3 + 43,

48 = 5 + 43,

50 = 3 + 47


In this example, $F_{50}(3) = 14$ and $F_{50}(5) = 7$.
Therefore, $\pi(50) = F_{50}(3) + 1 = 15$ and $\pi_{2}(50) = F_{50}(3) - F_{50}(5) = 14 - 7 = 7$.

cite: https://doi.org/10.13140/RG.2.2.22119.76963

--------------------------------------------------

Many people have asked me: how does your discovery come about? 
I wish I could claim that it was the fruit of a towering intellect and 
long years spent labouring over Goldbach’s Conjecture—that I endured 
endless nights of calculation, frustration and false starts, until 
at last a flash of insight broke through. But the truth is far quieter 
than that.

It began less than a year ago, with a dream. The problem presented itself 
not as a revelation, but as the faintest suggestion, lodged somewhere 
at the back of my mind. The next morning, I recalled a familiar fact: 
every prime p>3 is congruent to either 1 or −1 modulo 6. With that simple 
observation in hand, I wrote a very simple, quick algorithm to compute 
the minimal Goldbach partitions. Then I simply let the numbers run their course. 
For weeks, I did little more than watch, noting their quiet rhythms and 
half-formed patterns. When a few sequences became apparent, 
I submitted them to the OEIS—not with any expectation, but merely 
because they struck me as curious.

Then came the second dream, which provided the true glimmer of understanding. 
In it, I saw that whenever the number 3 appeared as the least prime in a partition, 
a new prime seemed to join the endless procession of the primes. That simple dream, 
unlikely as it sounds, was the key. Afterwards, I noticed the subtle 
thread linking π(n) and π₂(n) with the minimal Goldbach partitions. 
I checked my figures, and there it was—plain, neat, and quietly satisfying.

And so the “discovery” arrived. No stroke of genius—only two stray dreams, 
a modest little algorithm, and the patience to sit with the numbers until 
they whispered their secret.
