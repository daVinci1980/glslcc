
#include "common/common.h"
#include "glslpp/preproc.h"

#include "fileutils.h"
#include "stringutils.h"

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

GLCCint _preprocess(GLCCPreprocessor* _preproc);
GLCCint _preprocessPhaseTwo(GLCCPreprocessor* _preproc);
GLCCint _preprocessPhaseThree(GLCCPreprocessor* _preproc);
GLCCint _preprocessPhaseFour(GLCCPreprocessor* _preproc);

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// TODO: Relocate this into a common header. This will work for any stateful object that has an 
// char* mErrorString. 
template<typename T>
GLCCint ReportError(GLCCint _errCode, T *_where, const char* _fmt, ...)
{
    // Cleanup if anythign is there already.
    if (_where->mErrorString) {
        delete [] _where->mErrorString;
        _where->mErrorString = NULL;
    }

    // determine required size.
    va_list args;
    va_start( args, _fmt);
    int reqSize = vsnprintf(NULL, 0, _fmt, args);
    assert(reqSize >= 0);
    va_end(args);

    // Create new place.
    va_start( args, _fmt);
    _where->mErrorString = new char[reqSize + 1];
    vsnprintf(_where->mErrorString, reqSize + 1, _fmt, args);
    va_end(args);

    return _errCode;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
struct GLCCPreprocessor 
{
    const GLPPOptions mOptions;

    char* mFilename;
    char* mInputBuffer;
    char* mOutputBuffer;
    char* mErrorString;

    GLCCint mVersionGLSL;
    bool mUsedLineContinuations;

    // --------------------------------------------------------------------------------------------
    GLCCPreprocessor(const GLPPOptions& _options)
    : mOptions(_options)
    , mFilename(NULL)
    , mInputBuffer(NULL)
    , mOutputBuffer(NULL)
    , mErrorString(NULL)
    , mVersionGLSL(110) // Per the spec, this is the default.
    , mUsedLineContinuations(false)
    { }

    // --------------------------------------------------------------------------------------------
    ~GLCCPreprocessor()
    {
        delete [] mFilename;
        delete [] mInputBuffer;
        delete [] mOutputBuffer;
        delete [] mErrorString;
    }
};

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
GLCCint genPreprocessor(GLCCPreprocessor** _newPreproc, const GLPPOptions* _optOptions)
{
    if (!_newPreproc) {
        return GLCCError_MissingRequiredParameter;
    }

    // Options are optional. If unspecified, the defaults (per the constructor) will be used.
    GLPPOptions myOpts;
    if (_optOptions) {
        myOpts = (*_optOptions);
    }

    (*_newPreproc) = new GLCCPreprocessor(myOpts);
    return GLCCError_Ok;
}

// ------------------------------------------------------------------------------------------------
GLCCint deletePreprocessor(GLCCPreprocessor** _preproc)
{
    if (!_preproc) {
        return GLCCError_MissingRequiredParameter;
    }

    if (*_preproc) {
        delete (*_preproc);
        (*_preproc) = NULL;
    }

    return GLCCError_Ok;
}

// ------------------------------------------------------------------------------------------------
GLCCint preprocessFromFile(GLCCPreprocessor* _preproc, const char* _filename)
{
    if (!_preproc) {
        return GLCCError_MissingRequiredParameter;
    }

    _preproc->mFilename = stringDuplicate(_filename);
    _preproc->mInputBuffer = fileContentsToString(_filename);
    if (_preproc->mInputBuffer == NULL) {
        return GLCCError_FileNotFound;
    }

    return _preprocess(_preproc);
}

// ------------------------------------------------------------------------------------------------
GLCCint preprocessFromMemory(GLCCPreprocessor* _preproc, const char* _memBuffer, const char* _optFilename)
{
    if (!_preproc || !_memBuffer) {
        return GLCCError_MissingRequiredParameter;
    }

    _preproc->mFilename = stringDuplicate((_optFilename != NULL) ? _optFilename : "MemoryBuffer");
    _preproc->mInputBuffer = stringDuplicate(_memBuffer);

    return _preprocess(_preproc);
}

// ------------------------------------------------------------------------------------------------
const char* getLastError(GLCCPreprocessor* _preproc)
{
    if (!_preproc) {
        return NULL;
    }

    return _preproc->mErrorString;
}

// ------------------------------------------------------------------------------------------------
GLCCint _preprocess(GLCCPreprocessor* _preproc)
{
    GLCCint errCode = GLCCError_Ok;
    // In the C preprocessor, phase 1 is reading into memory. If we're here, we've already 
    // done that.

    if ((errCode = _preprocessPhaseTwo(_preproc)) != GLCCError_Ok)
        return errCode;

    if ((errCode = _preprocessPhaseThree(_preproc)) != GLCCError_Ok)
        return errCode;

    return _preprocessPhaseFour(_preproc);
}

// ------------------------------------------------------------------------------------------------
GLCCint _preprocessPhaseTwo(GLCCPreprocessor* _preproc)
{
    // Phase Two is line continuation. We do this in-place in _preproc->mInputBuffer. 

    // ANNOY: Line continuation isn't supported in all versions of GLSL--added in GLSL420. 
    //     We could detect the #version here and error out if line continuations are used and
    //     the version is less than 420. Instead, we just record whether line continuation 
    //     occurred and a later phase will croak. That simplifies the processing here.
    // 
    // TODO: We should support a non-conformant behavior (which should probably be the default)
    //     that if you use line continuation, we insert an equivalent number of \ns after the next 
    //     non-escaped EOL character. This would keep line numbers in sync for error purposes.

    size_t srcFp = 0;
    size_t dstFp = 0;
    size_t lineNum = 1;
    size_t charNum = 1;
    int contiguousLinesContinued = 0;
    bool anyContinuations = false;

    bool advanceDst = false;
    for (srcFp = 0; _preproc->mInputBuffer[srcFp]; ++srcFp) {
        char thisChar = _preproc->mInputBuffer[srcFp + 0]; 
        char nextChar = _preproc->mInputBuffer[srcFp + 1];
        int advanceSrc = 0;

        if (thisChar == '\\' && nextChar == '\n') {
            ++contiguousLinesContinued;
            anyContinuations = true;
            advanceDst = false;
            advanceSrc = 1;
        } else {
            if (_preproc->mOptions.mMaintainLineCount && thisChar == '\n') {
                while (contiguousLinesContinued-- > 0) {
                    // Combined with the '\n' we're going to pick up from this line, this will
                    // wind up with <SPACE><EOL> on each line.
                    _preproc->mInputBuffer[dstFp++] = '\n';
                    _preproc->mInputBuffer[dstFp++] = ' ';
                }
            }
            advanceDst = true;
        }

        // PERFORMANCE: Profile whether an extra branch here matters. We do this a lot. It could
        // be that the branch here costs more than it saves and it could be factored out.
        if (dstFp != srcFp && advanceDst) {
            _preproc->mInputBuffer[dstFp] = thisChar;
        }

        if (thisChar == '\n') {
            ++lineNum;
            charNum = 1;
        }

        // Move the srcFp forward (skipping characters) as necessary.
        srcFp += advanceSrc;

        if (advanceDst) {
            ++dstFp;
        }
    }

    // Write out the new '\0', then record whether we did any line continuations.
    _preproc->mInputBuffer[dstFp] = '\0';
    _preproc->mUsedLineContinuations = anyContinuations;

    return GLCCError_Ok;
}

// ------------------------------------------------------------------------------------------------
GLCCint _preprocessPhaseThree(GLCCPreprocessor* _preproc)
{
    // Phase three is comment processing. At this point, line continuations are gone, so any EOLs
    // that remain are "real", and will have effects on non-comment characters.
    // According to the C Preprocessor documentation:
    //     - Comments are replaced by one space
    //     - EOLs are preserved in both single and multi-line comments.
    // As with line continuations, we do this in-place in _preproc->mInputBuffer.

    enum ECommentMode {
        ECM_NoComment = 0,
        ECM_SingleLine,
        ECM_MultiLine
    };

    ECommentMode commentMode = ECM_NoComment;
    size_t srcFp = 0;
    size_t dstFp = 0;
    size_t lineNum = 1;
    size_t charNum = 1;

    bool advanceDst = false;
    for (srcFp = 0; _preproc->mInputBuffer[srcFp]; ++srcFp) {
        char thisChar = _preproc->mInputBuffer[srcFp + 0]; 
        char nextChar = _preproc->mInputBuffer[srcFp + 1];

        switch(commentMode) {
            case ECM_NoComment: 
            {
                if (thisChar == '/') {
                    if (nextChar == '/') {
                        // Entering a single line comment. Write a space.
                        _preproc->mInputBuffer[dstFp++] = ' ';
                        commentMode = ECM_SingleLine;
                        // Move the source location. This is for consistency--but not needed for single line
                        // comments. It *is* required for multi-line comments.
                        ++srcFp;
                        break;
                    } else if (nextChar == '*') {
                        // Entering multi-line comment. Write a space.
                        _preproc->mInputBuffer[dstFp++] = ' ';
                        commentMode = ECM_MultiLine;
                        // We must move the src pointer here--otherwise we would recognize /*/ 
                        // as a begin and end of a comment string.
                        ++srcFp;
                        break;
                    } 
                }

                _preproc->mInputBuffer[dstFp++] = thisChar;
                break;
            }

            case ECM_SingleLine:
            {
                if (thisChar == '\n') {
                    commentMode = ECM_NoComment;
                    _preproc->mInputBuffer[dstFp++] = thisChar;
                }
                break;
            }

            case ECM_MultiLine:
            {
                if (thisChar == '\n') {
                    // Write out the newline, they are preserved.
                    _preproc->mInputBuffer[dstFp++] = thisChar;                    
                }

                if (thisChar == '*' && nextChar == '/') {
                    commentMode = ECM_NoComment;
                    // Advance the src pointer, otherwise we'd recognize */* as a finish->start comment.
                    ++srcFp;
                }

                break;
            }

            default:
                assert(!"Error in parser.");
                break;
        };
    }

    _preproc->mInputBuffer[dstFp] = '\0';


    return GLCCError_Ok;
}

// ------------------------------------------------------------------------------------------------
GLCCint _preprocessPhaseFour(GLCCPreprocessor* _preproc)
{
    // Phase four is what people think of when they think of the preprocessor. This does 
    // processing on preprocessor directives, macro substitution, etc.
    // Because of the complexity of this phase, we use a real parser here rather than a hand-
    // rolled parser like the ones used for phase two and phase three.

    // I deferred complaining about the #version directive--in case the author used line 
    // coninuations here, too.

    // ANNOY: The preprocessor has to be both a line parser and a line-independent parser. This
    //     is the result of a particular intersection of rules. Preprocessor directives that
    //     begin with # are processed in a line manner--they take over the complete line they
    //     appear on. However, macro substitution has a particular corner case that's a bit 
    //     dumb. If you have a function-like macro, it doesn't perform macro substitution unless
    //     the call-site looks like a function call. But that site could be split over multiple
    //     lines, meaning that we have to actually do light parsing on that portion of the file.
    //     For this reason, you'll find that the lexxer/parser for this have rules to basically
    //     determine when they do and do not care about EOLs. 

    printf("%s", _preproc->mInputBuffer);

    return ReportError(GLCCError_NotImplemented, _preproc, "Phase %d has not yet been implemented. %s", 4, "food");
}
