def getIdList(fPath):
    with open(fPath) as f:
        idList = [n for n in f]
    return idList


def getChecksum(idList):
    three = twice = 0
    for id in idList:
        charCount = {}
        f2 = f3 = False
        for char in id:
            charCount[char] = charCount.get(char, 0) + 1
        for v in charCount.values():
            if v == 2:
                f2 = True
            if v == 3:
                f3 = True
        twice += 1 if f2 else 0
        three += 1 if f3 else 0
    return three * twice


def getCommon(idList):
    for i in range(len(idList)-1):
        for j in range(i+1, len(idList)):
            count = 0
            for k in range(len(idList[i])):
                if idList[i][k] != idList[j][k]:
                    count += 1
                    if count > 1:
                        break
            if count == 1:
                res = []
                for k in range(len(idList[i])):
                    if idList[i][k] == idList[j][k]:
                        res.append(idList[i][k])
                return ''.join(res).rstrip()
    return None


idList = getIdList('n2.txt')
# print("Checksum is: {}".format(getChecksum(idList)))
print(getCommon(idList))
