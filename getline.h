#ifndef GETLINE_H
#define GETLINE_H

#include <stdio.h>

#ifdef _WIN32

#define restrict __restrict
typedef long long ssize_t;

ssize_t getline(char **restrict lineptr, size_t *restrict n, FILE *restrict stream);
ssize_t getdelim(char **restrict lineptr, size_t *restrict n, int delim, FILE *restrict stream);

#endif
#endif /* GETLINE_H */
