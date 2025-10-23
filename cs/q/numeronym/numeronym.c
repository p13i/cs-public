#include "numeronym.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

bool IsNumeronym(char* pattern, size_t m, char* input,
                 size_t n) {
  if (pattern == NULL || input == NULL) {
    fprintf(stderr, "pattern or input parameter is null\n");
    return false;
  }
  size_t i = 0;  // pattern
  size_t j = 0;  // input
  while (i < m && j < n) {
    if (IsAlpha(pattern[i]) && pattern[i] == input[j]) {
      i++;
      j++;
    } else if (IsNumeral(pattern[i])) {
      size_t number = 0;
      if (!TryParseUInt(pattern, &i, m, &number)) {
        fprintf(stderr, "Failed to parse uint.\n");
        return false;
      }
      i++;  // increment i to move past the last digit
      if (!ScrollForward(input, &j, n, number)) {
        fprintf(stderr, "Unable to scroll forward.\n");
        return false;
      }
    } else {
      fprintf(stderr,
              "Invalid character in pattern: "
              "pattern[i=%zu]=%c, input[j=%zu]=%c\n",
              i, pattern[i], j, input[j]);
      return false;
    }
  }
  return (i == m) && (j == n);
}

bool InArrayBounds(const char* s, size_t n, size_t i) {
  return i < n;
}

bool IsAlpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool IsNumeral(char c) { return ('0' <= c && c <= '9'); }

bool TryParseUInt(char* pattern, size_t* i, size_t m,
                  size_t* number) {
  if (pattern == NULL || i == NULL || number == NULL) {
    fprintf(stderr,
            "pattern, i, and number pointer arguments may "
            "not be null.");
    return false;
  }
  // Find length of the uint.
  size_t local_i = *i;
  size_t uint_char_count = 0;
  while (local_i < m && IsNumeral(pattern[local_i])) {
    uint_char_count++;
    local_i++;
  }
  if (uint_char_count == 0) {
    fprintf(stderr, "No digits found to parse as uint.\n");
    return false;
  }

#if 0
  // From ChatGPT:
  // Parse digits forward.
  size_t value = 0;
  for (size_t idx = *i; idx < *i + uint_char_count; idx++) {
    char c = pattern[idx];
    if (!IsNumeral(c)) {
      fprintf(stderr,
              "pattern[idx=%zu]=%c is unexpectedly not a "
              "numeral.\n",
              idx, c);
      return false;
    }
    size_t digit = (size_t)(c - '0');
    value = value * 10 + digit;
  }
#else
  size_t value = 0;
  // Add digits from the back.
  size_t parsing_i = *i + uint_char_count - 1;
  size_t tens_mult = 1;
  while (parsing_i >= *i && parsing_i < m) {
    char c = pattern[parsing_i];
    if (!IsNumeral(c)) {
      fprintf(
          stderr,
          "pattern[parsing_i=%zu]=%c is unexpectedly not a "
          "numeral.\n",
          parsing_i, c);
      return false;
    }
    size_t digit = c - '0';
    value += digit * tens_mult;
    parsing_i = parsing_i - 1ul;
    tens_mult *= 10;
  }
#endif

  // Assign out params.
  *i = *i + uint_char_count - 1;  // leave i on last digit
  *number = value;
  return true;
}

bool ScrollForward(const char* input, size_t* j, size_t n,
                   size_t number) {
  if ((*j + number) > n) {
    fprintf(stderr,
            "Scroll function found that index will go out "
            "of bounds: (*j + number) > n, for *j=%zu, "
            "number=%zu, n=%zu.\n",
            *j, number, n);
    return false;
  }
  size_t local_j = *j;
  for (size_t count = 0; count < number; count++) {
    if (!InArrayBounds(input, n, local_j)) {
      fprintf(stderr,
              "Index is unexpectedly out of bounds.\n");
      return false;
    }
    local_j++;
  }
  *j = local_j;
  return true;
}
