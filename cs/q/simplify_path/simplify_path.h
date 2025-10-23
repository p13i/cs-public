#ifndef CS_Q_SIMPLIFY_PATH_SIMPLIFY_PATH_H
#define CS_Q_SIMPLIFY_PATH_SIMPLIFY_PATH_H

#include <stddef.h>

/**
 * Reduces the given Unix file path to it's canonical form.
 *
 * @param s The messy path.
 * @param n Length of given string.
 */
char* SimplifyPath(const char* s, size_t n);

#endif  // CS_Q_SIMPLIFY_PATH_SIMPLIFY_PATH_H