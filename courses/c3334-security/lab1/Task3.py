# Task 3 to find d

def doElu(e, phi_n):
    A1, A2, A3 = 1, 0, phi_n
    B1, B2, B3 = 0, 1, e
    while True:
        if B3 == 0:
            return A3, None
        elif B3 == 1:
            return B3, B2
        Q = A3 // B3
        T1, T2, T3 = A1 - Q * B1, A2 - Q * B2, A3 - Q * B3
        A1, A2, A3 = B1, B2, B3
        B1, B2, B3 = T1, T2, T3

def main():
    e = 587
    p, q = 2861, 4259
    phi_n = (p - 1) * (q - 1)

    g, i = doElu(e, phi_n)

    if g == 1:
        res = phi_n + i
        print("The modular multiplicative inverse (d) is {}".format(res))
    else:
        print("There is no modular multiplicative inverse")

main()
