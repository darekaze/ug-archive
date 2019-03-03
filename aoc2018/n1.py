def getNumList(fPath):
    with open(fPath) as f:
        numList = [int(n) for n in f]
    return numList


def getSum(numList):
    return sum(numList)


def getFirstFreq(numList):
    freq = [0]
    n = 0
    while True:
        for num in numList:
            n += num
            if n in freq:
                return n
            else:
                freq.append(n)


nl = getNumList('n1.txt')
print("Sum is: {}".format(getSum(nl)))
print("First Frequency is: {}".format(getFirstFreq(nl)))
