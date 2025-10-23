#ifndef CS_Q_NUMERONYM_NUMERONYM_H
#define CS_Q_NUMERONYM_NUMERONYM_H

#include <stdbool.h>
#include <stddef.h>

// Determines if the given input string is a numeronym of
// the given pattern.
bool IsNumeronym(char* pattern, size_t m, char* input,
                 size_t n);

// Determines if the given index is in the bounds of the
// array.
bool InArrayBounds(const char* s, size_t n, size_t i);

// Checks if a character is in [A-Za-z].
bool IsAlpha(char c);

// Checks if a character is in [0-9].
bool IsNumeral(char c);

// Tries to parse an unsigned int at and after pattern[i].
//
// If an unsigned int can be parsed from the string, the
// indexer *i will be incremented to the last index of the
// number in the string, the *number out param will be
// set to the parsed number, and true will be returned.
//
// If the unsigned int cannot be parsed, *i will not be
// incremented, *number will not be set, and false will be
// returned.
bool TryParseUInt(char* pattern, size_t* i, size_t m,
                  size_t* number);

// Moves the cursor forward in the input string by the given
// `number` of steps. If the final cursor position will be
// in-bounds, `j` will be set. `n` is the length of the
// `input`. The method returns a success flag. If not
// successful, `j` is not set. The cursor will stop on the
// last character of the substring `input[j:j+number]`, i.e.
// `j + number - 1`.
bool ScrollForward(const char* input, size_t* j, size_t n,
                   size_t number);

#endif  // CS_Q_NUMERONYM_NUMERONYM_H