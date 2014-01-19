
#include "glslppafx.h"

#include "tokens.h"
#include "productions.h"

#include "glslpp/preproc.h"
#include <iostream>

struct StateObjectGLSL 
{ 
    bool IsValidType(const std::string& _type) const { return false; }
};

typedef LexerT<GLSLTokenID> GlslLexer;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    GLCCPreprocessor* preproc = NULL;
    GLCCint errCode = GLCCError_Ok;
    GLCCint tmpErrCode = GLCCError_Ok;
    
    GLPPOptions opts;

    GlslLexer lex(glslTokens, "struct { Foo f; };", new StateObjectGLSL);

    while (!lex.Peek().IsEOF()) {
        std::cout << lex.Pop() << "\n";
    }

    ParserGLSL someVar("3");
    Node* root = someVar.Parse();
    if (root != nullptr) {
        printf("Parse success!\n");
    } else {
        printf("Parse fail!\n");        
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

