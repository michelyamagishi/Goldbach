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
    # Recebe o número N da linha de comando
    if len(sys.argv) != 2:
        print("Uso: python Goldbach.py <número inteiro>")
        return
    
    N = int(sys.argv[1])

    # Verifica se N é um número par maior que 6
    if (N>6 & N%2 !=0):
        print("Número ímpar. Entre com um número par maior que 2.")
        return

    # Loop de i de 1 até Q+1
    Q = (N//6)
    for i in range(1,Q+1):
        
        # print(f" com i ={i}")     
        # Verifica P = 6 * Q + 1
        P = 6 * i + 1 
        if is_prime(P):
            F = N - P
            if is_prime(F):
                print(f"N = P + F: {N} = {P} + {F} com i = {i}")
                return

        # Verifica P = 6 * Q - 1
            
        P = 6 * i - 1 
        if is_prime(P):
            F = N - P
            if is_prime(F):
                print(f"N = P + F: {N} = {P} + {F} com i = {i}")
                return

    print("Nenhuma combinação encontrada.")

if __name__ == "__main__":
    main()

