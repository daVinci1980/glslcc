#pragma once

#include "common/common.h"

inline char* stringDuplicate(const char* _inStr)
{
    if (_inStr == NULL) {
        return NULL;
    }

    // Determine length--doesn't include \0
    size_t strLen = 0;
    while ((*_inStr) != 0) {
        ++strLen;
    }

    // Create return buffer
    char* retBuffer = new char[strLen + 1];

    // And copy into it.
    const char* src = _inStr;
    char* dst = retBuffer;
    while (*src) {
        *dst++ = *src++;
    }
    *dst = '\0';

    return retBuffer;
}

