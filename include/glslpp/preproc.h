#pragma once

#include "common/common.h"

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// This is the type used that contains all state required for preprocessing a file.
struct GLCCPreprocessor;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
struct GLPPOptions
{
    // Attempt to maintain the line count in the presence of line continuation and macro 
    // substitution. This is non-conformant behavior, but will generally provide better 
    // error messages. 
    bool mMaintainLineCount;

    inline GLPPOptions()
    : mMaintainLineCount(true)
    { }
};

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// TODO: Document all of these. 
extern "C" GLCCint genPreprocessor(GLCCPreprocessor** _newPreproc, const GLPPOptions* _optOptions);
extern "C" GLCCint deletePreprocessor(GLCCPreprocessor** _preproc);
extern "C" GLCCint preprocessFromFile(GLCCPreprocessor* _preproc, const char* _filename);
extern "C" GLCCint preprocessFromMemory(GLCCPreprocessor* _preproc, const char* _memBuffer, 
                                        const char* _optFilename);
extern "C" const char* getLastError(GLCCPreprocessor* _preproc);
