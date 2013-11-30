
#include "glcc/common.h"
#include "glcc/preproc.h"

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
enum ECommentMode {
    ECM_NoComment = 0,
    ECM_SingleLine,
    ECM_MultiLine,

    ECM_MAX
};

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
struct GLCCPreprocessor {
    char* mFilename
    char* mInputBuffer;
    char* mOutputBuffer;
    char* mErrorString;

    GLCCint mVersionGLSL;
    bool mUsedLineContinuations;

    ECommentMode mCommentMode;



    // --------------------------------------------------------------------------------------------
    GLCCPreprocessor()
    : mFilename(NULL)
    , mInputBuffer(NULL)
    , mOutputBuffer(NULL)
    , mErrorString(NULL)
    , mVersionGLSL(110) // Per the spec, this is the default.
    , mUsedLineContinuations(false)
    , mCommentMode(ECM_NoComment)
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
GLCCstatus genPreprocessor(GLCCPreprocessor** _newPreproc)
{
    if (!_newPreproc) {
        return GLCCError_MissingRequiredParameter;
    }

    (*_newPreproc)) = new GLCCPreprocessor;
    return GLCCError_Ok;
}

// ------------------------------------------------------------------------------------------------
GLCCstatus deletePreprocessor(GLCCPreprocessor** _preproc)
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

    _preproc->mFilename = _filename;
    _preproc->mInputBuffer = fileContentsToString(_filename);
    if (_preproc->mInputBuffer == NULL) {
        return GLCCError_FileNotFound;
    }

    return _preprocess(_preproc);
}

// ------------------------------------------------------------------------------------------------
GLCCint preprocessFromMemory(GLCCPreprocessor* _preproc, const char* _memBuffer)
{
    if (!_preproc || !_memBuffer) {
        return GLCCError_MissingRequiredParameter;
    }

    _preproc->mFilename = stringDuplicate("MemoryBuffer");
    _preproc->mInputBuffer = stringDuplicate(_memBuffer);

    return _preprocess(_preproc);
}

// ------------------------------------------------------------------------------------------------
GLCCint _preprocess(GLCCPreprocess* _preproc)
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
GLCCint _preprocessPhaseTwo(GLCCPreprocess* _preproc)
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

    bool advanceDst = false;
    for (srcFp = 0; _preproc->mInputBuffer[srcFp]; ++srcFp) {
        char thisChar = _preproc->mInputBuffer[srcFp]; 
        char nextChar = _preproc->mInputBuffer[srcFp + 1];

        if (thisChar == '\\') {
            if (nextChar == '\n') {
                advanceDst = false;
                ++srcFp;
            } else {
                return ReportError(GLCCError_InvalidSyntax, _preproc, 
                                   "Syntax error at line %d, column %d. '\\' is only allowed immediately prior to end-of-line.", 
                                   lineNum, charNum);
            }
        } else {
            advanceDst = true;
        }

        // PERFORMANCE: Profile whether an extra branch here matters. We do this a lot. It could
        // be that the branch here costs more than it saves and it could be factored out.
        if (advanceDst && dstFp != srcFp) {
            _preProc->mInputBuffer[dstFp++] = thisChar;
        }

        if (thisChar == '\n') {
            ++lineNum;
            charNum = 1;
        }
    }

    // Write out the new '\0', then record whether we did any line continuations.
    _preproc->mInputBuffer[dstFp] = '\0';
    _preproc->mUsedLineContinuations = (srcFp != dstFp);

    return GLCCError_Ok;
}

// ------------------------------------------------------------------------------------------------
GLCCint _preprocessPhaseThree(GLCCPreprocess* _preproc)
{
    // Phase three is comment processing. At this point, line continuations are gone, so any EOLs
    // that remain are "real", and will have effects on non-comment characters.
    // According to the C Preprocessor documentation:
    //     - Comments are replaced by one space
    //     - EOLs are preserved in both single and multi-line comments.
    // As with line continuations, we do this in-place in _preproc->mInputBuffer.

    size_t srcFp = 0;
    size_t dstFp = 0;
    size_t lineNum = 1;
    size_t charNum = 1;

    bool advanceDst = false;
    for (srcFp = 0; _preproc->mInputBuffer[srcFp]; ++srcFp) {
        char thisChar = _preproc->mInputBuffer[srcFp]; 
        char nextChar = _preproc->mInputBuffer[srcFp + 1];

        switch(_preproc->mCommentMode) {
            case ECM_NoComment: 
            {
                if (thisChar == '/') {
                    if (nextChar == '/') {
                        // Entering a single line comment. Write a space.
                        _preproc->mInputBuffer[dstFp++] = ' ';
                        _preproc->mCommentMode = ECM_SingleLine;
                        // Move the source location. This is for consistency--but not needed for single line
                        // comments. It *is* required for multi-line comments.
                        ++srcFp;
                        break;
                    } else if (nextChar == '*') {
                        // Entering multi-line comment. Write a space.
                        _preproc->mInputBuffer[dstFp++] = ' ';
                        _preproc->mCommentMode = ECM_MultiLine;
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
                    _preproc->mCommentMode = ECM_NoComment;
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
                    _preproc->mCommentMode = ECM_NoComment;
                    // Advance the src pointer, otherwise we'd recognize */* as a finish->start comment.
                    ++srcFp;
                }

                break;
            }

            default:
                assert(!"Error in parser.")
                break;
        };
    }

    _preproc->mInputBuffer[dstFp] = '\0';


    return GLCCError_Ok;
}

// ------------------------------------------------------------------------------------------------
GLCCint _preprocessPhaseFour(GLCCPreprocess* _preproc)
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

    return GLCCError_Ok;
}
