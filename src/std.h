#ifndef STD_H
#define STD_H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define FREE(p) \
    do {             \
        free(p);     \
        (p) = NULL;  \
    } while(0)

size_t slength(const char* s, size_t maxlen);
//void deallocPtr(void* ptr);

#endif // STD_H
