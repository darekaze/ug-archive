from __future__ import absolute_import, division, unicode_literals
from Crypto.Cipher import AES
from Crypto.Hash import SHA

_IV = 16 * '\x00'


def genSHA(text):
    h = SHA.new(bytes(text, 'utf-8'))
    return h.hexdigest()


def aesEncrypt(key, text):
    encryption = AES.new(key, AES.MODE_CBC, _IV)
    byteCipher = encryption.encrypt(text)
    return byteCipher.hex()


def genKey(str):
    tempKey = []
    for i in range(0, len(str)//2-4):
        hexTeam = str[i*2] + str[i*2+1]
        tempKey.append(int(hexTeam, 16))
    return tempKey


def cracker(str, r):
    r = bytes(r, 'utf-8')
    key = bytes(genKey(genSHA(str)))
    encode = aesEncrypt(r, key)
    print(str(encode))


def main():
    challengeR = "zRcfk9nLTIiKg0om"
    guess = "Amanda"
    cracker(guess, challengeR)


main()
