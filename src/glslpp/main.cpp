
#include "glslppafx.h"

#include "glslpp/preproc.h"
#include "common/parserutil.h"

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    GLCCPreprocessor* preproc = NULL;
    GLCCint errCode = GLCCError_Ok;
    GLCCint tmpErrCode = GLCCError_Ok;
    
    GLPPOptions opts;

    extern const LexicalEntry* GetGlslTokens();
    Lexer myLex(GetGlslTokens(), "   foo[]", NULL);
    myLex.Pop();
    myLex.Pop();
    myLex.Pop();

    if (argc < 2) {
        errCode = GLCCError_MissingRequiredParameter;
        goto exit;
    }

    if ((errCode = genPreprocessor(&preproc, &opts)) != GLCCError_Ok) 
        goto exit;

    if ((errCode = preprocessFromFile(preproc, argv[1])) != GLCCError_Ok) {
        const char* errText = getLastError(preproc);
        if (errText) {
            printf("Error reported during preprocessing: \"%s\"\n", errText);
        }

        goto cleanup;
    }


cleanup:
    tmpErrCode = deletePreprocessor(&preproc);
    if (tmpErrCode != GLCCError_Ok) {
        printf("Error while cleaning up: '%d', main still returning preprocess status.", tmpErrCode);
    }

exit:
    return errCode;
}

