
#include "glslppafx.h"

#include "glslpp/preproc.h"
#include "common/parserutil.h"
#include <iostream>

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
    extern const char** GetGlslReservedTypes();

    StateObject parserState(GetGlslReservedTypes());
    Lexer myLex(GetGlslTokens(), "struct Foo {\n\tdmat2x2 bar[3];\n\tfloat baz;\n};", &parserState);
    for (Token tok = myLex.Pop(); !tok.IsEOF(); tok = myLex.Pop()) {
        std::cout << tok << std::endl;
    }

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

