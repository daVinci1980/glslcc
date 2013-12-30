#include "glslppafx.h"

#include "common/parserutil.h"

#include <algorithm>
#include <iostream>

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
    CENTROID,
    IN,
    OUT,
    INOUT,
    UNIFORM,
    VARYING,
    PATCH,
    SAMPLE,
    NOPERSPECTIVE,
    FLAT,
    SMOOTH,
    LAYOUT,
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
    INTCONSTANT,

    TYPE_NAME
};

// ------------------------------------------------------------------------------------------------
const char* glslReservedTypes[] = {
    "bool",
    "float", 
    "double", 
    "int", 
    "uint", 
    "bvec2", 
    "bvec3", 
    "bvec4", 
    "ivec2", 
    "ivec3", 
    "ivec4", 
    "uvec2", 
    "uvec3", 
    "uvec4", 
    "vec2", 
    "vec3", 
    "vec4", 
    "mat2", 
    "mat3", 
    "mat4", 
    "dvec2", 
    "dvec3", 
    "dvec4", 
    "dmat2", 
    "dmat3", 
    "dmat4", 
    "mat2x2", 
    "mat2x3", 
    "mat2x4", 
    "mat3x2", 
    "mat3x3", 
    "mat3x4", 
    "mat4x2", 
    "mat4x3", 
    "mat4x4", 
    "dmat2x2", 
    "dmat2x3", 
    "dmat2x4", 
    "dmat3x2", 
    "dmat3x3", 
    "dmat3x4", 
    "dmat4x2", 
    "dmat4x3", 
    "dmat4x4", 
    "sampler1D", 
    "sampler2D", 
    "sampler3D", 
    "samplerCube", 
    "sampler1DShadow", 
    "sampler2DShadow", 
    "samplerCubeShadow", 
    "sampler1DArray", 
    "sampler2DArray", 
    "sampler1DArrayShadow", 
    "sampler2DArrayShadow", 
    "isampler1D", 
    "isampler2D", 
    "isampler3D", 
    "isamplerCube", 
    "isampler1DArray", 
    "isampler2DArray", 
    "usampler1D", 
    "usampler2D", 
    "usampler3D", 
    "usamplerCube", 
    "usampler1DArray", 
    "usampler2DArray", 
    "sampler2DRect", 
    "sampler2DRectShadow", 
    "isampler2DRect", 
    "usampler2DRect", 
    "samplerBuffer", 
    "isamplerBuffer", 
    "usamplerBuffer", 
    "samplerCubeArray", 
    "samplerCubeArrayShadow", 
    "isamplerCubeArray", 
    "usamplerCubeArray", 
    "sampler2DMS", 
    "isampler2DMS", 
    "usampler2DMS", 
    "sampler2DMSArray", 
    "isampler2DMSArray", 
    "usampler2DMSArray", 

    nullptr
};

// ------------------------------------------------------------------------------------------------
int DetermineIdentifierSubtype(const std::string& _match, const int _initialToken, StateObject* _state)
{
    if (_state) {
        if (_state->IsValidType(_match)) {
            return TYPE_NAME;
        }
    }
    return _initialToken;
}


// ------------------------------------------------------------------------------------------------
const LexicalEntry glslTokens[] = {
    { "[ \t\n]+"                                , IGNORETOKEN      , nullptr },
    { "attribute"                               , ATTRIBUTE        , nullptr },
    { "const"                                   , CONST            , nullptr },
    { "break"                                   , BREAK            , nullptr },
    { "continue"                                , CONTINUE         , nullptr },
    { "do"                                      , DO               , nullptr },
    { "else"                                    , ELSE             , nullptr },
    { "for"                                     , FOR              , nullptr },
    { "if"                                      , IF               , nullptr },
    { "discard"                                 , DISCARD          , nullptr },
    { "return"                                  , RETURN           , nullptr },
    { "switch"                                  , SWITCH           , nullptr },
    { "case"                                    , CASE             , nullptr },
    { "default"                                 , DEFAULT          , nullptr },
    { "subroutine"                              , SUBROUTINE       , nullptr },
    { "centroid"                                , CENTROID         , nullptr },
    { "in"                                      , IN               , nullptr },
    { "out"                                     , OUT              , nullptr },
    { "inout"                                   , INOUT            , nullptr },
    { "uniform"                                 , UNIFORM          , nullptr },
    { "varying"                                 , VARYING          , nullptr },
    { "patch"                                   , PATCH            , nullptr },
    { "sample"                                  , SAMPLE           , nullptr },
    { "noperspective"                           , NOPERSPECTIVE    , nullptr },
    { "flat"                                    , FLAT             , nullptr },
    { "smooth"                                  , SMOOTH           , nullptr },
    { "layout"                                  , LAYOUT           , nullptr },
    { "struct"                                  , STRUCT           , nullptr },
    { "void"                                    , VOID             , nullptr },
    { "while"                                   , WHILE            , nullptr },
    { "<<"                                      , LEFT_OP          , nullptr },
    { ">>"                                      , RIGHT_OP         , nullptr },
    { "\\+\\+"                                  , INC_OP           , nullptr },
    { "--"                                      , DEC_OP           , nullptr },
    { "<="                                      , LE_OP            , nullptr },
    { ">="                                      , GE_OP            , nullptr },
    { "=="                                      , EQ_OP            , nullptr },
    { "!="                                      , NE_OP            , nullptr },
    { "&&"                                      , AND_OP           , nullptr },
    { "\\|\\|"                                  , OR_OP            , nullptr },
    { "\\^\\^"                                  , XOR_OP           , nullptr },
    { "\\*="                                    , MUL_ASSIGN       , nullptr },
    { "/="                                      , DIV_ASSIGN       , nullptr },
    { "\\+="                                    , ADD_ASSIGN       , nullptr },
    { "%="                                      , MOD_ASSIGN       , nullptr },
    { "<<="                                     , LEFT_ASSIGN      , nullptr },
    { ">>="                                     , RIGHT_ASSIGN     , nullptr },
    { "&="                                      , AND_ASSIGN       , nullptr },
    { "\\^="                                    , XOR_ASSIGN       , nullptr },
    { "\\|="                                    , OR_ASSIGN        , nullptr },
    { "-="                                      , SUB_ASSIGN       , nullptr },
    { "\\("                                     , LEFT_PAREN       , nullptr },
    { "\\)"                                     , RIGHT_PAREN      , nullptr },
    { "\\["                                     , LEFT_BRACKET     , nullptr },
    { "\\]"                                     , RIGHT_BRACKET    , nullptr },
    { "\\{"                                     , LEFT_BRACE       , nullptr },
    { "\\}"                                     , RIGHT_BRACE      , nullptr },
    { "\\."                                     , DOT              , nullptr },
    { ","                                       , COMMA            , nullptr },
    { ":"                                       , COLON            , nullptr },
    { "="                                       , EQUAL            , nullptr },
    { ";"                                       , SEMICOLON        , nullptr },
    { "!"                                       , BANG             , nullptr },
    { "-"                                       , DASH             , nullptr },
    { "~"                                       , TILDE            , nullptr },
    { "\\+"                                     , PLUS             , nullptr },
    { "\\*"                                     , STAR             , nullptr },
    { "/"                                       , SLASH            , nullptr },
    { "%"                                       , PERCENT          , nullptr },
    { "<"                                       , LEFT_ANGLE       , nullptr },
    { ">"                                       , RIGHT_ANGLE      , nullptr },
    { "\\|"                                     , VERTICAL_BAR     , nullptr },
    { "\\^"                                     , CARET            , nullptr },
    { "&"                                       , AMPERSAND        , nullptr },
    { "\\?"                                     , QUESTION         , nullptr },
    { "invariant"                               , INVARIANT        , nullptr },
    { "highp"                                   , HIGH_PRECISION   , nullptr },
    { "mediump"                                 , MEDIUM_PRECISION , nullptr },
    { "lowp"                                    , LOW_PRECISION    , nullptr },
    { "precision"                               , PRECISION        , nullptr },
    { "false|true"                              , BOOLCONSTANT     , nullptr },
    { "[_a-zA-Z][_a-zA-Z0-9]*"                  , IDENTIFIER       , DetermineIdentifierSubtype },
    { "[0-9]+\\.[0-9]*([eE][-+]?[0-9]+)?[fF]?"  , FLOATCONSTANT    , nullptr },
    { "[0-9]*\\.[0-9]+([eE][-+]?[0-9]+)?[fF]?"  , FLOATCONSTANT    , nullptr },
    { "[0-9]+[eE][-+]?[0-9]+[fF]?"              , FLOATCONSTANT    , nullptr },
    { "[1-9][0-9]*[uU]"                         , UINTCONSTANT     , nullptr },
    { "[1-9][0-9]*"                             ,  INTCONSTANT     , nullptr },
    { "[0-7]+[uU]"                              , UINTCONSTANT     , nullptr },
    { "[0-7]+"                                  ,  INTCONSTANT     , nullptr },
    { "0[xX][0-9a-fA-F]+[uU]"                   , UINTCONSTANT     , nullptr },
    { "0[xX][0-9a-fA-F]+"                       ,  INTCONSTANT     , nullptr },
    { "."                                       , REJECTTOKEN      , nullptr },

    SentinelRule
};

// ------------------------------------------------------------------------------------------------
const LexicalEntry* GetGlslTokens() { return glslTokens; }
const char** GetGlslReservedTypes() { return glslReservedTypes; }

// ------------------------------------------------------------------------------------------------
StreamPosition::StreamPosition(StreamOffset _lineNum, StreamOffset _colNum)
: mLineNum(_lineNum)
, mColNum(_colNum)
{ }

// ------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& _os, const StreamPosition& _streamPos)
{
    return _os << "StreamPosition(" << _streamPos.mLineNum
               << ", " << _streamPos.mColNum
               << ")";
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
StateObject::StateObject(const char** _reservedTypes)
{
    for (int i = 0; _reservedTypes[i]; ++i) {
        mTypes.insert(_reservedTypes[i]);
    }
}

// ------------------------------------------------------------------------------------------------
bool StateObject::IsValidType(const std::string& _identifier) const
{
    return mTypes.find(_identifier) != mTypes.end();
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
Token::Token()
: mType(REJECTTOKEN)
, mString("")
{ }

// ------------------------------------------------------------------------------------------------
Token::Token(int _streamTok, const std::string& _tokenString, StreamPosition _streamPos)
: mType(_streamTok)
, mString(_tokenString)
, mLocation(_streamPos)
{

}

// ------------------------------------------------------------------------------------------------
bool Token::IsEOF() const
{
    return mType == EOFTOKEN;
}

// ------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& _os, const Token& _token)
{
    return _os << "Token(" << _token.mType
               << ", " << _token.mString
               << ", " << _token.mLocation
               << ")";
}

// ------------------------------------------------------------------------------------------------
Lexer::Lexer(const LexicalEntry* _rules, const char* _stringToLex, StateObject* _state)
: mStringToLex(_stringToLex)
, mSrcPos(mStringToLex.cbegin())
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
        std::regex re = std::regex(_rules[i].Pattern, std::regex::optimize | std::regex::ECMAScript);
        mRules.push_back({ re, _rules[i].Token, _rules[i].TokenCallback });
    }
}

// ------------------------------------------------------------------------------------------------
Token Lexer::FindNextToken()
{
    Token retToken;

    int tokenFound = IGNORETOKEN;
    while (tokenFound == IGNORETOKEN)
    {
        // Check if we're at the EOF. If so, bail.
        if (mSrcPos == mStringToLex.cend()) {
            retToken = Token(EOFTOKEN, "\0", mStreamPosition);
            break;    
        }

        // If not, then we need to find out what token we're at, and either use it or
        // continue. We may continue if the token is something we're supposed to ignore.
        std::smatch res;
        bool found = false;
        for (auto it = mRules.begin(); it != mRules.end(); ++it) {
            if (std::regex_search(mSrcPos, mStringToLex.cend(), res, it->Regex, std::regex_constants::match_continuous)) {
                // Definitely needs to not be 0, but also needs to not be >1--if so indicates 
                // something wonky has happened.
                std::string matchStr = res.str();
                size_t resultLength = res.length();
                int localTok = it->Token;

                // If there's a token callback function, call it and allow it to replace our 
                // default value.
                if (it->TokenCallback) {
                    localTok = it->TokenCallback(matchStr, localTok, mState);
                }

                if (localTok == REJECTTOKEN) {
                    continue;
                }

                // Mark this as the found token.
                tokenFound = localTok;

                // Record the token for further processing.
                retToken = Token(tokenFound, matchStr, mStreamPosition);

                // Update position variables.
                int newLines = std::count(matchStr.begin(), matchStr.end(), '\n');
                if (newLines > 0) {
                    mStreamPosition.mLineNum += newLines;
                    mStreamPosition.mColNum = resultLength - matchStr.find_last_of('\n');
                } else {
                    mStreamPosition.mColNum += resultLength;
                }

                // Advance our search position, and mark that we have a hit.
                std::advance(mSrcPos, resultLength);
                found = true;

                // Then go around again.
                break;
            }
        }

        if (!found) {
            printf("Error! Couldn't match token!\n");
            return Token();
        }
    }

    return retToken;
}

