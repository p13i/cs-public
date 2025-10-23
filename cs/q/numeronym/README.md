# Numeronym

Given a pattern such as `i18n`, check if a give string
matches it.

For example:

- `F2eb2k` or `8` should match `Facebook`.
- `i18n` should match `internationalization`.

This problem is
["Valid Word Abbreviation" on Leetcode](https://leetcode.com/problems/valid-word-abbreviation/).

## Solution brainstorming

I think this is a rather straight forward problem.

The program will keep two cursors, one for the input string
(`Facebook`) and one for the pattern string.

If a character in `[A-Za-z]` appears, we will check for that
character at the cursor position in the input string. If the
characters match, we safely increment both counters. If not,
return false.

If a number in `[0-9]` appears, we will first parse the
positive integer `> 0` and then check if those many
characters are present in the input string; safely
incrementing the pointers.

If an unknown number appears, return false. Otherwise return
true if we are at the end of both the input string and
pattern string.

The time complexity is `O(n + m)` where the input string has
length `n` and the pattern string has length `m` (`m` will
run twice, an extra pass is needed for parsing ints). The
space complexity is `O(m)` for parsing the int with a stack.

The function signature looks like:

```cc
bool IsNumeronym(std::string input, std::string pattern);
```
