#include "glslppafx.h"

#include "common/parserutil.h"
#include <regex>

const LexicalEntry SentinelRule = { nullptr, 0 };
const int IGNORETOKEN = -1;
const int REJECTTOKEN = -2;
const int EOFTOKEN = -3;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
enum GLSLTokenIDs {
    ATTRIBUTE = 1,
    CONST,
    BOOL,
    FLOAT,
    DOUBLE,
    INT,
    UINT,
    BREAK,
    CONTINUE,
    DO,
    ELSE,
    FOR,
    IF,
    DISCARD,
    RETURN,
    SWITCH,
    CASE,
    DEFAULT,
    SUBROUTINE,
    BVEC2,
    BVEC3,
    BVEC4,
    IVEC2,
    IVEC3,
    IVEC4,
    UVEC2,
    UVEC3,
    UVEC4,
    VEC2,
    VEC3,
    VEC4,
    MAT2,
    MAT3,
    MAT4,
    CENTROID,
    IN,
    OUT,
    INOUT,
    UNIFORM,
    VARYING,
    PATCH,
    SAMPLE,
    DVEC2,
    DVEC3,
    DVEC4,
    DMAT2,
    DMAT3,
    DMAT4,
    NOPERSPECTIVE,
    FLAT,
    SMOOTH,
    LAYOUT,
    MAT2X2,
    MAT2X3,
    MAT2X4,
    MAT3X2,
    MAT3X3,
    MAT3X4,
    MAT4X2,
    MAT4X3,
    MAT4X4,
    DMAT2X2,
    DMAT2X3,
    DMAT2X4,
    DMAT3X2,
    DMAT3X3,
    DMAT3X4,
    DMAT4X2,
    DMAT4X3,
    DMAT4X4,
    SAMPLER1D,
    SAMPLER2D,
    SAMPLER3D,
    SAMPLERCUBE,
    SAMPLER1DSHADOW,
    SAMPLER2DSHADOW,
    SAMPLERCUBESHADOW,
    SAMPLER1DARRAY,
    SAMPLER2DARRAY,
    SAMPLER1DARRAYSHADOW,
    SAMPLER2DARRAYSHADOW,
    ISAMPLER1D,
    ISAMPLER2D,
    ISAMPLER3D,
    ISAMPLERCUBE,
    ISAMPLER1DARRAY,
    ISAMPLER2DARRAY,
    USAMPLER1D,
    USAMPLER2D,
    USAMPLER3D,
    USAMPLERCUBE,
    USAMPLER1DARRAY,
    USAMPLER2DARRAY,
    SAMPLER2DRECT,
    SAMPLER2DRECTSHADOW,
    ISAMPLER2DRECT,
    USAMPLER2DRECT,
    SAMPLERBUFFER,
    ISAMPLERBUFFER,
    USAMPLERBUFFER,
    SAMPLERCUBEARRAY,
    SAMPLERCUBEARRAYSHADOW,
    ISAMPLERCUBEARRAY,
    USAMPLERCUBEARRAY,
    SAMPLER2DMS,
    ISAMPLER2DMS,
    USAMPLER2DMS,
    SAMPLER2DMSARRAY,
    ISAMPLER2DMSARRAY,
    USAMPLER2DMSARRAY,

    STRUCT,
    VOID,
    WHILE,

    LEFT_OP,
    RIGHT_OP,
    INC_OP,
    DEC_OP,
    LE_OP,
    GE_OP,
    EQ_OP,
    NE_OP,
    AND_OP,
    OR_OP,
    XOR_OP,
    MUL_ASSIGN,
    DIV_ASSIGN,
    ADD_ASSIGN,
    MOD_ASSIGN,
    LEFT_ASSIGN,
    RIGHT_ASSIGN,
    AND_ASSIGN,
    XOR_ASSIGN,
    OR_ASSIGN,
    SUB_ASSIGN,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    DOT,
    COMMA,
    COLON,
    EQUAL,
    SEMICOLON,
    BANG,
    DASH,
    TILDE,
    PLUS,
    STAR,
    SLASH,
    PERCENT,
    LEFT_ANGLE,
    RIGHT_ANGLE,
    VERTICAL_BAR,
    CARET,
    AMPERSAND,
    QUESTION,

    INVARIANT,
    HIGH_PRECISION,
    MEDIUM_PRECISION,
    LOW_PRECISION,
    PRECISION,
    BOOLCONSTANT,
    IDENTIFIER,

    FLOATCONSTANT,
    UINTCONSTANT,
    INTCONSTANT
};

// ------------------------------------------------------------------------------------------------
const LexicalEntry glslTokens[] = {
    { "[ \t\n]+"                                , IGNORETOKEN },
	{ "attribute"				                , ATTRIBUTE },
	{ "const"					                , CONST },
	{ "bool"                                    , BOOL },
	{ "float"                                   , FLOAT },
	{ "double"                                  , DOUBLE },
	{ "int"                                     , INT },
	{ "uint"                                    , UINT },
	{ "break"                                   , BREAK },
	{ "continue"                                , CONTINUE },
	{ "do"                                      , DO },
	{ "else"                                    , ELSE },
	{ "for"                                     , FOR },
	{ "if"                                      , IF },
	{ "discard"                                 , DISCARD },
	{ "return"                                  , RETURN },
	{ "switch"                                  , SWITCH },
	{ "case"                                    , CASE },
	{ "default"                                 , DEFAULT },
	{ "subroutine"                              , SUBROUTINE },
	{ "bvec2"                                   , BVEC2 },
	{ "bvec3"                                   , BVEC3 },
	{ "bvec4"                                   , BVEC4 },
	{ "ivec2"                                   , IVEC2 },
	{ "ivec3"                                   , IVEC3 },
	{ "ivec4"                                   , IVEC4 },
	{ "uvec2"                                   , UVEC2 },
	{ "uvec3"                                   , UVEC3 },
	{ "uvec4"                                   , UVEC4 },
	{ "vec2"                                    , VEC2 },
	{ "vec3"                                    , VEC3 },
	{ "vec4"                                    , VEC4 },
	{ "mat2"                                    , MAT2 },
	{ "mat3"                                    , MAT3 },
	{ "mat4"                                    , MAT4 },
	{ "centroid"                                , CENTROID },
	{ "in"                                      , IN },
	{ "out"                                     , OUT },
	{ "inout"                                   , INOUT },
	{ "uniform"                                 , UNIFORM },
	{ "varying"                                 , VARYING },
	{ "patch"                                   , PATCH },
	{ "sample"                                  , SAMPLE },
	{ "dvec2"                                   , DVEC2 },
	{ "dvec3"                                   , DVEC3 },
	{ "dvec4"                                   , DVEC4 },
	{ "dmat2"                                   , DMAT2 },
	{ "dmat3"                                   , DMAT3 },
	{ "dmat4"                                   , DMAT4 },
	{ "noperspective"                           , NOPERSPECTIVE },
	{ "flat"                                    , FLAT },
	{ "smooth"                                  , SMOOTH },
	{ "layout"                                  , LAYOUT },
	{ "mat2x2"                                  , MAT2X2 },
	{ "mat2x3"                                  , MAT2X3 },
	{ "mat2x4"                                  , MAT2X4 },
	{ "mat3x2"                                  , MAT3X2 },
	{ "mat3x3"                                  , MAT3X3 },
	{ "mat3x4"                                  , MAT3X4 },
	{ "mat4x2"                                  , MAT4X2 },
	{ "mat4x3"                                  , MAT4X3 },
	{ "mat4x4"                                  , MAT4X4 },
	{ "dmat2x2"                                 , DMAT2X2 },
	{ "dmat2x3"                                 , DMAT2X3 },
	{ "dmat2x4"                                 , DMAT2X4 },
	{ "dmat3x2"                                 , DMAT3X2 },
	{ "dmat3x3"                                 , DMAT3X3 },
	{ "dmat3x4"                                 , DMAT3X4 },
	{ "dmat4x2"                                 , DMAT4X2 },
	{ "dmat4x3"                                 , DMAT4X3 },
	{ "dmat4x4"                                 , DMAT4X4 },
	{ "sampler1D"                               , SAMPLER1D },
	{ "sampler2D"                               , SAMPLER2D },
	{ "sampler3D"                               , SAMPLER3D },
	{ "samplerCube"                             , SAMPLERCUBE },
	{ "sampler1DShadow"                         , SAMPLER1DSHADOW },
	{ "sampler2DShadow"                         , SAMPLER2DSHADOW },
	{ "samplerCubeShadow"                       , SAMPLERCUBESHADOW },
	{ "sampler1DArray"                          , SAMPLER1DARRAY },
	{ "sampler2DArray"                          , SAMPLER2DARRAY },
	{ "sampler1DArrayShadow"                    , SAMPLER1DARRAYSHADOW },
	{ "sampler2DArrayShadow"                    , SAMPLER2DARRAYSHADOW },
	{ "isampler1D"                              , ISAMPLER1D },
	{ "isampler2D"                              , ISAMPLER2D },
	{ "isampler3D"                              , ISAMPLER3D },
	{ "isamplerCube"                            , ISAMPLERCUBE },
	{ "isampler1DArray"                         , ISAMPLER1DARRAY },
	{ "isampler2DArray"                         , ISAMPLER2DARRAY },
	{ "usampler1D"                              , USAMPLER1D },
	{ "usampler2D"                              , USAMPLER2D },
	{ "usampler3D"                              , USAMPLER3D },
	{ "usamplerCube"                            , USAMPLERCUBE },
	{ "usampler1DArray"                         , USAMPLER1DARRAY },
	{ "usampler2DArray"                         , USAMPLER2DARRAY },
	{ "sampler2DRect"                           , SAMPLER2DRECT },
	{ "sampler2DRectShadow"                     , SAMPLER2DRECTSHADOW },
	{ "isampler2DRect"                          , ISAMPLER2DRECT },
	{ "usampler2DRect"                          , USAMPLER2DRECT },
	{ "samplerBuffer"                           , SAMPLERBUFFER },
	{ "isamplerBuffer"                          , ISAMPLERBUFFER },
	{ "usamplerBuffer"                          , USAMPLERBUFFER },
	{ "samplerCubeArray"                        , SAMPLERCUBEARRAY },
	{ "samplerCubeArrayShadow"                  , SAMPLERCUBEARRAYSHADOW },
	{ "isamplerCubeArray"                       , ISAMPLERCUBEARRAY },
	{ "usamplerCubeArray"                       , USAMPLERCUBEARRAY },
	{ "sampler2DMS"                             , SAMPLER2DMS },
	{ "isampler2DMS"                            , ISAMPLER2DMS },
	{ "usampler2DMS"                            , USAMPLER2DMS },
	{ "sampler2DMSArray"                        , SAMPLER2DMSARRAY },
	{ "isampler2DMSArray"                       , ISAMPLER2DMSARRAY },
	{ "usampler2DMSArray"                       , USAMPLER2DMSARRAY },

    { "struct"                                  , STRUCT },
    { "void"                                    , VOID },
    { "while"                                   , WHILE },

    { "<<"                                      , LEFT_OP },
    { ">>"                                      , RIGHT_OP },
    { "\\+\\+"                                  , INC_OP },
    { "--"                                      , DEC_OP },
    { "<="                                      , LE_OP },
    { ">="                                      , GE_OP },
    { "=="                                      , EQ_OP },
    { "!="                                      , NE_OP },
    { "&&"                                      , AND_OP },
    { "\\|\\|"                                  , OR_OP },
    { "\\^\\^"                                  , XOR_OP },
    { "\\*="                                    , MUL_ASSIGN },
    { "/="                                      , DIV_ASSIGN },
    { "\\+="                                    , ADD_ASSIGN },
    { "%="                                      , MOD_ASSIGN },
    { "<<="                                     , LEFT_ASSIGN },
    { ">>="                                     , RIGHT_ASSIGN },
    { "&="                                      , AND_ASSIGN },
    { "\\^="                                    , XOR_ASSIGN },
    { "\\|="                                    , OR_ASSIGN },
    { "-="                                      , SUB_ASSIGN },
    { "\\("                                     , LEFT_PAREN },
    { "\\)"                                     , RIGHT_PAREN },
    { "\\["                                     , LEFT_BRACKET },
    { "\\]"                                     , RIGHT_BRACKET },
    { "\\{"                                     , LEFT_BRACE },
    { "\\}"                                     , RIGHT_BRACE },
    { "\\."                                     , DOT },
    { ","                                       , COMMA },
    { ":"                                       , COLON },
    { "="                                       , EQUAL },
    { ";"                                       , SEMICOLON },
    { "!"                                       , BANG },
    { "-"                                       , DASH },
    { "~"                                       , TILDE },
    { "\\+"                                     , PLUS },
    { "\\*"                                     , STAR },
    { "/"                                       , SLASH },
    { "%"                                       , PERCENT },
    { "<"                                       , LEFT_ANGLE },
    { ">"                                       , RIGHT_ANGLE },
    { "\\|"                                     , VERTICAL_BAR },
    { "\\^"                                     , CARET },
    { "&"                                       , AMPERSAND },
    { "\\?"                                     , QUESTION },

    { "invariant"                               , INVARIANT },
    { "highp"                                   , HIGH_PRECISION },
    { "mediump"                                 , MEDIUM_PRECISION },
    { "lowp"                                    , LOW_PRECISION },
    { "precision"                               , PRECISION },
    { "false|true"                              , BOOLCONSTANT },
    { "[_a-zA-Z][_a-zA-Z0-9]*"                  , IDENTIFIER },

    { "[0-9]+\\.[0-9]*([eE][-+]?[0-9]+)?[fF]?"  , FLOATCONSTANT },
    { "[0-9]*\\.[0-9]+([eE][-+]?[0-9]+)?[fF]?"  , FLOATCONSTANT },
    { "[0-9]+[eE][-+]?[0-9]+[fF]?"              , FLOATCONSTANT },

    { "[1-9][0-9]*[uU]"                         , UINTCONSTANT },
    { "[1-9][0-9]*"                             ,  INTCONSTANT },
    { "[0-7]+[uU]"                              , UINTCONSTANT },
    { "[0-7]+"                                  ,  INTCONSTANT },
    { "0[xX][0-9a-fA-F]+[uU]"                   , UINTCONSTANT },
    { "0[xX][0-9a-fA-F]+"                       ,  INTCONSTANT },
    { "."                                       , REJECTTOKEN },
    SentinelRule
};

const LexicalEntry* GetGlslTokens() { return glslTokens; }

// ------------------------------------------------------------------------------------------------
Lexer::Lexer(const LexicalEntry* _rules, const char* _stringToLex, StateObject* _state)
: mStringToLex(_stringToLex)
, mSrcPos(mStringToLex.cbegin())
, mLineNum(1)
, mColNum(1)
, mState(_state)
{
    ConstructRules(_rules);
    mLookaheadToken = FindNextToken();
}

// ------------------------------------------------------------------------------------------------
Lexer::~Lexer()
{

}

// ------------------------------------------------------------------------------------------------
Token Lexer::Peek() const 
{ 
    return mLookaheadToken; 
}

// ------------------------------------------------------------------------------------------------
Token Lexer::Pop() 
{ 
    Token retToken = mLookaheadToken;
    mLookaheadToken = FindNextToken();
    return retToken; 
}

// ------------------------------------------------------------------------------------------------
void Lexer::ConstructRules(const LexicalEntry* _rules)
{
    for (int i = 0; _rules[i].Pattern; ++i) {
        mRules.push_back({ std::regex(_rules[i].Pattern, std::regex::optimize | std::regex::ECMAScript), _rules[i].Token });
    }
}

// ------------------------------------------------------------------------------------------------
Token Lexer::FindNextToken()
{
    int tokenFound = IGNORETOKEN;
    while (tokenFound == IGNORETOKEN)
    {
        // Check if we're at the EOF. If so, bail.
        if (mSrcPos == mStringToLex.cend()) {
            tokenFound = EOFTOKEN;
            break;    
        }

        // If not, then we need to find out what token we're at, and either use it or
        // continue. We may continue if the token is something we're supposed to ignore.
        std::smatch res;
        bool found = false;
        for (auto it = mRules.begin(); it != mRules.end(); ++it) {
            if (std::regex_search(mSrcPos, mStringToLex.cend(), res, it->Regex, std::regex_constants::match_continuous)) {
                printf("Found match!\n");
                tokenFound = it->Token;
                assert(!res.empty());

                std::advance(mSrcPos, res.length());
                found = true;
                break;
            }
        }

        if (!found) {
            printf("Error! Couldn't match token!\n");
            return Token();
        }
    }

    return Token();
}
