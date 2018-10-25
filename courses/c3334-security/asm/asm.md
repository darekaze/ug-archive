# COMP 3334 Assignment 1

JAHJA Darwin, 16094501d

---

Let my X = 16094501

## Q1

Given plaintext: `WELCOMETOTHEBIGSCREENS`

The Secret Key is: $(16094501)^{10}\space mod \space 26 = 17$

Then, the plain to cipher mapping is:

```tex
ABCDEFGHIJKLMNOPQRSTUVWXYZ
RSTUVWXYZABCDEFGHIJKLMNOPQ
```

Therefore, the encrypted cipher text is `NVCTFDVKFKYVSZXJTIVVEJ`.

## Q2

Given Integer $X = 16094501$; Modulo $M = 26$.

If $gcd(X, M) = 1$, then the multiplicative inverse $X^{-1}$ exists. Here, $gcd(16094501, 26) = 1$. Therefore, in this case, $X^{-1}$ exists.

To find $X^{-1}$, we can use **Extended Euclidean Algorithm**:

Steps:

|    A1    | A2 |    A3    |    B1    |  B2 |    B3    |    T1    |  T2 | T3 |    Q   |
|:--------:|:--:|:--------:|:--------:|:---:|:--------:|:--------:|:---:|:--:|:------:|
|     1    |  0 |    26    |     0    |  1  | 16094501 |     1    |  0  | 26 |    0   |
|     0    |  1 | 16094501 |     1    |  0  |    26    |  -619019 |  1  |  7 | 619019 |
|     1    |  0 |    26    |  -619019 |  1  |     7    |  1857058 |  -3 |  5 |    3   |
|  -619019 |  1 |     7    |  1857058 |  -3 |     5    | -2476077 |  4  |  2 |    1   |
|  1857058 | -3 |     5    | -2476077 |  4  |     2    |  6809212 | -11 |  1 |    2   |
| -2476077 |  4 |     2    |  6809212 | **-11** |**1** |          |     |    |        |

Therefore, the modular multiplicative inverse of 16094501 is $-11 \space mod \space 26 = 15$

## Q3

The 5x5 matrix using key `MANGKHUT`: 

| M | A | N |  G  | K |
|:-:|:-:|:-:|:---:|:-:|
| **H** | **U** | **T** |  B  | C |
| D | E | F | I/J | L |
| O | P | Q |  R  | S |
| V | W | X |  Y  | Z |

Cipher text decrypting: 

```tex
AP SL VH FU QH UD IR KR BS FW FA QZ 
WE LC OM ET OT HE BI GS CR EX EN SX
```

Therefore, the original message is `WELCOMETOTHEBIGSCREENS`.

## Q4

