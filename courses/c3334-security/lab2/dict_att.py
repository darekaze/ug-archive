'''
@ JAHJA Darwin, 16094501d
This program requires "pycrypto" module to run:
pip3 install pycrypto
'''
from Crypto.Cipher import AES
from Crypto.Hash import SHA

_IV = bytes([0] * 16)


def genSHA(text):
    h = SHA.new(bytes(text, 'utf-8'))
    return h.hexdigest()


def genKey(text):
    tempKey = []
    for i in range(0, len(text)//2-4):
        hexTeam = text[i*2] + text[i*2+1]
        tempKey.append(int(hexTeam, 16))
    return tempKey


def aesEncrypt(key, text):
    encryption = AES.new(key, AES.MODE_CBC, _IV)
    textBytes = encryption.encrypt(text)
    return textBytes.hex()


def cracker(challengeR, hiddenField):
    r = bytes(challengeR, 'utf-8')
    with open("english.txt", "r") as f:
        for line in f:
            key = bytes(genKey(genSHA(line.strip(" \t\n\r"))))
            res = aesEncrypt(key, r)
            if res == hiddenField:
                return line
    return None


def main():
    """
    -- For testing (Tested in linux environment)
    challengeR = zRcfk9nLTIiKg0om
    hiddenField = 49a9e0285bdf602c7390c2c0ca737edd
    """
    challengeR = input("Please enter the challenge R: ")
    hiddenField = input("Please enter the hidden field value: ")

    ans = cracker(challengeR, hiddenField)
    if ans:
        print("Found! The password is: {}".format(ans))
    else:
        print("Password not found in dict...")


main()
