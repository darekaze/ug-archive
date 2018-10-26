from __future__ import absolute_import, division, unicode_literals
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


def cracker(guess, cr):
    r = bytes(cr, 'utf-8')
    key = bytes(genKey(genSHA(guess)))
    res = aesEncrypt(key, r)
    print(res)


def main():
    challengeR = "zRcfk9nLTIiKg0om"
    guess = "Amanda"
    cracker(guess, challengeR)


main()
