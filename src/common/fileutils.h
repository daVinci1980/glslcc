#pragma once

#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _WIN32
#	include <unistd.h>
#endif

#include "stringutils.h"

// ------------------------------------------------------------------------------------------------
long long computeFileSize(const char* _filename)
{
	struct stat sb;
    if (stat(_filename, &sb) == -1) {
        return 0;
    }

    return (long long) sb.st_size;
}

// ------------------------------------------------------------------------------------------------
char* fileContentsToString(const char* _filename)
{
    // NOTE: This pushes a double-NULL into the end of the file contents--so the contents aren't
    // exactly the same as in the file. This is to allow flex/bison to parse in place.
    long long realFileSize = computeFileSize(_filename);
	if (realFileSize == 0) {
        return NULL;
    }
	
	size_t effectiveFileSize = size_t(realFileSize);

	if (effectiveFileSize != realFileSize) {
		assert(!"Don't actually support files larger than 4G right now, please file a bug report. Also lol.");
		return NULL;
	}
	

    FILE* file = fopen(_filename, "rb");
    if (!file) {
        return NULL;
    }

	char* retBuffer = new char[effectiveFileSize + 2];
	if (fread(retBuffer, 1, effectiveFileSize, file) != effectiveFileSize) {
        assert(!"file read error?");
    }

	retBuffer[effectiveFileSize + 0] = '\0';
	retBuffer[effectiveFileSize + 1] = '\0';

    return retBuffer;
}