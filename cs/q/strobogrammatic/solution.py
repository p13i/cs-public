# Leetcode 246: Strobogrammatic Number
# https://leetcode.com/problems/strobogrammatic-number/

LOOKUP = {"0": "0", "1": "1", "6": "9", "8": "8", "9": "6"}


def IsStrobogrammatic(num: str) -> bool:
    if not num:
        return False
    if len(num) > 1 and num[0] == "0":
        return False
    i = 0
    j = len(num) - 1
    while i <= j:
        if num[i] not in LOOKUP:
            return False
        if LOOKUP[num[i]] != num[j]:
            return False
        i += 1
        j -= 1
    return True


class Solution:
    def isStrobogrammatic(self, num: str) -> bool:
        return IsStrobogrammatic(num)
