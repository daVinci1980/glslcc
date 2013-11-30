#pragma once

#include "common/common.h"

// This is the type used that contains all state required for preprocessing a file.
struct GLCCPreprocessor;

extern "C" GLCCint genPreprocessor(GLCCPreprocessor** _newPreproc);
extern "C" GLCCint deletePreprocessor(GLCCPreprocessor** _preproc);
extern "C" GLCCint preprocessFromFile(GLCCPreprocessor* _preproc, const char* _filename);
extern "C" GLCCint preprocessFromMemory(GLCCPreprocessor* _preproc, const char* _memBuffer, const char* _optFilename);

extern "C" const char* getLastError(GLCCPreprocessor* _preproc);