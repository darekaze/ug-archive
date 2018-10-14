# Define const


def findLCS(A, B, C):
    UP_LEFT_IN, UP, LEFT, IN = 0, 1, 2, 3
    la, lb, lc = len(A), len(B), len(C)

    # set Score table and traceback table
    score = [[[0 for _ in range(lc+1)]
             for _ in range(lb+1)]
             for _ in range(la+1)]
    trace = [[[0 for _ in range(lc+1)]
             for _ in range(lb+1)]
             for _ in range(la+1)]

    # Initialize base case
    for x in range(1, la+1):
        for y in range(0, lb+1):
            score[x][y][0] = 0
            trace[x][y][0] = UP

    for y in range(1, lb+1):
        for z in range(0, lc+1):
            score[0][y][z] = 0
            trace[0][y][z] = LEFT

    for z in range(1, lc+1):
        for x in range(0, la+1):
            score[x][0][z] = 0
            trace[x][0][z] = IN
    # Main loop
    for x in range(1, la+1):
        for y in range(1, lb+1):
            for z in range(1, lc+1):
                if (A[x-1] == B[y-1] and A[x-1] == C[z-1]):
                    score[x][y][z] = score[x-1][y-1][z-1] + 1
                    trace[x][y][z] = UP_LEFT_IN
                else:
                    score[x][y][z] = score[x-1][y][z]
                    trace[x][y][z] = UP
                    if (score[x][y-1][z] >= score[x][y][z]):
                        score[x][y][z] = score[x][y-1][z]
                        trace[x][y][z] = LEFT
                    if (score[x][y][z-1] >= score[x][y][z]):
                        score[x][y][z] = score[x][y][z-1]
                        trace[x][y][z] = IN

    # length of LCS now equal to score[lenA][lenB][lenC]
    x, y, z = la, lb, lc
    res = []
    while x > 0 or y > 0 or z > 0:
        if trace[x][y][z] == UP_LEFT_IN:
            x -= 1
            y -= 1
            z -= 1
            res.insert(0, A[x])
        elif trace[x][y][z] == UP:
            x -= 1
        elif trace[x][y][z] == LEFT:
            y -= 1
        elif trace[x][y][z] == IN:
            z -= 1

    return ''.join(res)


def q2():
    str1 = "nine"
    str2 = "singing"
    str3 = "nine"
    res = findLCS(str1, str2, str3)
    print(res)


q2()
