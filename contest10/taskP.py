import numpy.fft as fft

def count_triples(a: str) -> int:
    n = len(a)
    b = [1 if a[i] == '1' else -1 for i in range(n)]
    c = fft.fft(b)

    count = 0
    for j in range(1, n):
        for i in range(j-1, -1, -1):
            if 2*j-i < n and j-i == 2*j-2*i and c[i]*c[j]*c[2*j-i].real > 0:
                count += 1

    return count



S = input()

print(count_triples(S))