#include "std.h"

size_t slength(const char* s, size_t maxlen)
{
    char* it = (char*)s;
    for(; it < s + maxlen && *it != '\0'; it++);
    return it - s;
}

// void deallocPtr(void* ptr)
// {
//     if(ptr != NULL) {
//         free(ptr);
//         ptr = NULL;
//     }
// }