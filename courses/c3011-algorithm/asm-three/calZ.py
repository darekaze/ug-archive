def calZ(pre, suf):
    string = "{}${}".format(pre, suf)
    z = len(string) * [None]
    lt, rt = 0, 0
    for k in range(1, len(string)):
        if k > rt:
            lt = rt = k
            while rt < len(string) and string[rt] == string[rt-lt]:
                rt += 1
            z[k] = rt - lt
            rt -= 1
        else:
            k1 = k - lt
            if z[k1] < rt-k+1:
                z[k] = z[k1]
            else:
                lt = k
                while rt < len(string) and string[rt] == string[rt-lt]:
                    rt += 1
                z[k] = rt - lt
                rt -= 1
        # Check if suffix
        if k + z[k] == len(string):
            return pre[0:z[k]]
    return None


def compare(pre, suf):
    z = calZ(pre, suf)
    print(z)


compare("aabczzz", "zzzabc")
