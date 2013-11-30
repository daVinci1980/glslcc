#pragma once

#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "stringutils.h"

// ------------------------------------------------------------------------------------------------
long long computeFileSize(const char* _filename)
{
    #ifdef _WIN32
    #   error "Implement computeFileSize for Windows."
    #else
        struct stat sb;
        if (stat(_filename, &sb) == -1) {
            return 0;
        }

        return (long long) sb.st_size;
    #endif
}

// ------------------------------------------------------------------------------------------------
char* fileContentsToString(const char* _filename)
{
    // NOTE: This pushes a double-NULL into the end of the file contents--so the contents aren't
    // exactly the same as in the file. This is to allow flex/bison to parse in place.
    long long fileSize = computeFileSize(_filename);
    if (fileSize == 0) {
        return NULL;
    }

    FILE* file = fopen(_filename, "rb");
    if (!file) {
        return NULL;
    }

    char* retBuffer = new char[fileSize + 2];
    if (fread(retBuffer, 1, fileSize, file) != fileSize) {
        assert(!"file read error?");
    }

    retBuffer[fileSize + 0] = '\0';
    retBuffer[fileSize + 1] = '\0';

    return retBuffer;
}