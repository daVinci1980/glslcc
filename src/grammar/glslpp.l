%{
#include <string>
// #include "node.h"
#include "parserpp.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng);
#define TOKEN(t) (yylval.token = t)

int gLineNum = 0;
bool gTokenizeEOL = false;
bool gTokenizeMacroFuncDef = false;
const char* gFileName = "stdin";
%}

%option noyywrap

WS                          [ \t]+
OWS                         [ \t]*

%%

[ \t]                       ;

^{OWS}\#{OWS}$                { gTokenizeEOL = true; return TOKEN(PPHASH); } 
^{OWS}\#{OWS}"define"         { gTokenizeEOL = true; gTokenizeMacroFuncDef = true; return TOKEN(PPDEFINE); }
^{OWS}\#{OWS}"undef"          { gTokenizeEOL = true; return TOKEN(PPUNDEF); }

^{OWS}\#{OWS}"if"             { gTokenizeEOL = true; return TOKEN(PPIF); }
^{OWS}\#{OWS}"ifdef"          { gTokenizeEOL = true; return TOKEN(PPIFDEF); }
^{OWS}\#{OWS}"ifndef"         { gTokenizeEOL = true; return TOKEN(PPIFNDEF); }
^{OWS}\#{OWS}"else"           { gTokenizeEOL = true; return TOKEN(PPELSE); }
^{OWS}\#{OWS}"elif"           { gTokenizeEOL = true; return TOKEN(PPELIF); }
^{OWS}\#{OWS}"endif"          { gTokenizeEOL = true; return TOKEN(PPENDIF); }

^{OWS}\#{OWS}"error"          { gTokenizeEOL = true; return TOKEN(PPERROR); }
^{OWS}\#{OWS}"pragma"         { gTokenizeEOL = true; return TOKEN(PPPRAGMA); }

^{OWS}\#{OWS}"extension"      { gTokenizeEOL = true; return TOKEN(PPEXTENSION); }
^{OWS}\#{OWS}"version"        { gTokenizeEOL = true; return TOKEN(PPVERSION); }

^{OWS}\#{OWS}"line"           { gTokenizeEOL = true; return TOKEN(PPLINE); }

"STDGL"                     return TOKEN(PPSTDGL);

"defined"                   return TOKEN(PPDEFINED);
"##"                        return TOKEN(PPPASTING);

"attribute"                 return TOKEN(ATTRIBUTE);
"const"                     return TOKEN(CONST);
"bool"                      return TOKEN(BOOL);
"float"                     return TOKEN(FLOAT);
"double"                    return TOKEN(DOUBLE);
"int"                       return TOKEN(INT);
"uint"                      return TOKEN(UINT);
"break"                     return TOKEN(BREAK);
"continue"                  return TOKEN(CONTINUE);
"do"                        return TOKEN(DO);
"else"                      return TOKEN(ELSE);
"for"                       return TOKEN(FOR);
"if"                        return TOKEN(IF);
"discard"                   return TOKEN(DISCARD);
"return"                    return TOKEN(RETURN);
"switch"                    return TOKEN(SWITCH);
"case"                      return TOKEN(CASE);
"default"                   return TOKEN(DEFAULT);
"subroutine"                return TOKEN(SUBROUTINE);
"bvec2"                     return TOKEN(BVEC2);
"bvec3"                     return TOKEN(BVEC3);
"bvec4"                     return TOKEN(BVEC4);
"ivec2"                     return TOKEN(IVEC2);
"ivec3"                     return TOKEN(IVEC3);
"ivec4"                     return TOKEN(IVEC4);
"uvec2"                     return TOKEN(UVEC2);
"uvec3"                     return TOKEN(UVEC3);
"uvec4"                     return TOKEN(UVEC4);
"vec2"                      return TOKEN(VEC2);
"vec3"                      return TOKEN(VEC3);
"vec4"                      return TOKEN(VEC4);
"mat2"                      return TOKEN(MAT2);
"mat3"                      return TOKEN(MAT3);
"mat4"                      return TOKEN(MAT4);
"centroid"                  return TOKEN(CENTROID);
"in"                        return TOKEN(IN);
"out"                       return TOKEN(OUT);
"inout"                     return TOKEN(INOUT);
"uniform"                   return TOKEN(UNIFORM);
"varying"                   return TOKEN(VARYING);
"patch"                     return TOKEN(PATCH);
"sample"                    return TOKEN(SAMPLE);
"dvec2"                     return TOKEN(DVEC2);
"dvec3"                     return TOKEN(DVEC3);
"dvec4"                     return TOKEN(DVEC4);
"dmat2"                     return TOKEN(DMAT2);
"dmat3"                     return TOKEN(DMAT3);
"dmat4"                     return TOKEN(DMAT4);
"noperspective"             return TOKEN(NOPERSPECTIVE);
"flat"                      return TOKEN(FLAT);
"smooth"                    return TOKEN(SMOOTH);
"layout"                    return TOKEN(LAYOUT);
"mat2x2"                    return TOKEN(MAT2X2);
"mat2x3"                    return TOKEN(MAT2X3);
"mat2x4"                    return TOKEN(MAT2X4);
"mat3x2"                    return TOKEN(MAT3X2);
"mat3x3"                    return TOKEN(MAT3X3);
"mat3x4"                    return TOKEN(MAT3X4);
"mat4x2"                    return TOKEN(MAT4X2);
"mat4x3"                    return TOKEN(MAT4X3);
"mat4x4"                    return TOKEN(MAT4X4);
"dmat2x2"                   return TOKEN(DMAT2X2);
"dmat2x3"                   return TOKEN(DMAT2X3);
"dmat2x4"                   return TOKEN(DMAT2X4);
"dmat3x2"                   return TOKEN(DMAT3X2);
"dmat3x3"                   return TOKEN(DMAT3X3);
"dmat3x4"                   return TOKEN(DMAT3X4);
"dmat4x2"                   return TOKEN(DMAT4X2);
"dmat4x3"                   return TOKEN(DMAT4X3);
"dmat4x4"                   return TOKEN(DMAT4X4);
"sampler1D"                 return TOKEN(SAMPLER1D);
"sampler2D"                 return TOKEN(SAMPLER2D);
"sampler3D"                 return TOKEN(SAMPLER3D);
"samplerCube"               return TOKEN(SAMPLERCUBE);
"sampler1DShadow"           return TOKEN(SAMPLER1DSHADOW);
"sampler2DShadow"           return TOKEN(SAMPLER2DSHADOW);
"samplerCubeShadow"         return TOKEN(SAMPLERCUBESHADOW);
"sampler1DArray"            return TOKEN(SAMPLER1DARRAY);
"sampler2DArray"            return TOKEN(SAMPLER2DARRAY);
"sampler1DArrayShadow"      return TOKEN(SAMPLER1DARRAYSHADOW);
"sampler2DArrayShadow"      return TOKEN(SAMPLER2DARRAYSHADOW);
"isampler1D"                return TOKEN(ISAMPLER1D);
"isampler2D"                return TOKEN(ISAMPLER2D);
"isampler3D"                return TOKEN(ISAMPLER3D);
"isamplerCube"              return TOKEN(ISAMPLERCUBE);
"isampler1DArray"           return TOKEN(ISAMPLER1DARRAY);
"isampler2DArray"           return TOKEN(ISAMPLER2DARRAY);
"usampler1D"                return TOKEN(USAMPLER1D);
"usampler2D"                return TOKEN(USAMPLER2D);
"usampler3D"                return TOKEN(USAMPLER3D);
"usamplerCube"              return TOKEN(USAMPLERCUBE);
"usampler1DArray"           return TOKEN(USAMPLER1DARRAY);
"usampler2DArray"           return TOKEN(USAMPLER2DARRAY);
"sampler2DRect"             return TOKEN(SAMPLER2DRECT);
"sampler2DRectShadow"       return TOKEN(SAMPLER2DRECTSHADOW);
"isampler2DRect"            return TOKEN(ISAMPLER2DRECT);
"usampler2DRect"            return TOKEN(USAMPLER2DRECT);
"samplerBuffer"             return TOKEN(SAMPLERBUFFER);
"isamplerBuffer"            return TOKEN(ISAMPLERBUFFER);
"usamplerBuffer"            return TOKEN(USAMPLERBUFFER);
"samplerCubeArray"          return TOKEN(SAMPLERCUBEARRAY);
"samplerCubeArrayShadow"    return TOKEN(SAMPLERCUBEARRAYSHADOW );
"isamplerCubeArray"         return TOKEN(ISAMPLERCUBEARRAY);
"usamplerCubeArray"         return TOKEN(USAMPLERCUBEARRAY);
"sampler2DMS"               return TOKEN(SAMPLER2DMS);
"isampler2DMS"              return TOKEN(ISAMPLER2DMS);
"usampler2DMS"              return TOKEN(USAMPLER2DMS);
"sampler2DMSArray"          return TOKEN(SAMPLER2DMSARRAY);
"isampler2DMSArray"         return TOKEN(ISAMPLER2DMSARRAY);
"usampler2DMSArray"         return TOKEN(USAMPLER2DMSARRAY);

"struct"                    return TOKEN(STRUCT);
"void"                      return TOKEN(VOID);
"while"                     return TOKEN(WHILE);

"<<"                        return TOKEN(LEFT_OP);
">>"                        return TOKEN(RIGHT_OP);
"++"                        return TOKEN(INC_OP);
"--"                        return TOKEN(DEC_OP);
"<="                        return TOKEN(LE_OP);
">="                        return TOKEN(GE_OP);
"=="                        return TOKEN(EQ_OP);
"!="                        return TOKEN(NE_OP);
"&&"                        return TOKEN(AND_OP);
"||"                        return TOKEN(OR_OP);
"^^"                        return TOKEN(XOR_OP);
"*="                        return TOKEN(MUL_ASSIGN);
"/="                        return TOKEN(DIV_ASSIGN);
"+="                        return TOKEN(ADD_ASSIGN);
"%="                        return TOKEN(MOD_ASSIGN);
"<<="                       return TOKEN(LEFT_ASSIGN);
">>="                       return TOKEN(RIGHT_ASSIGN);
"&="                        return TOKEN(AND_ASSIGN);
"^="                        return TOKEN(XOR_ASSIGN);
"|="                        return TOKEN(OR_ASSIGN);
"-="                        return TOKEN(SUB_ASSIGN);
"("                         return TOKEN(LEFT_PAREN);
")"                         return TOKEN(RIGHT_PAREN);
"["                         return TOKEN(LEFT_BRACKET);
"]"                         return TOKEN(RIGHT_BRACKET);
"{"                         return TOKEN(LEFT_BRACE);
"}"                         return TOKEN(RIGHT_BRACE);
\.                          return TOKEN(DOT);
","                         return TOKEN(COMMA);
":"                         return TOKEN(COLON);
"="                         return TOKEN(EQUAL);
";"                         return TOKEN(SEMICOLON);
"!"                         return TOKEN(BANG);
"-"                         return TOKEN(DASH);
"~"                         return TOKEN(TILDE);
"+"                         return TOKEN(PLUS);
"*"                         return TOKEN(STAR);
"/"                         return TOKEN(SLASH);
"%"                         return TOKEN(PERCENT);
"<"                         return TOKEN(LEFT_ANGLE);
">"                         return TOKEN(RIGHT_ANGLE);
"|"                         return TOKEN(VERTICAL_BAR);
"^"                         return TOKEN(CARET);
"&"                         return TOKEN(AMPERSAND);
"?"                         return TOKEN(QUESTION);

"invariant"                 return TOKEN(INVARIANT);
"highp"                     return TOKEN(HIGH_PRECISION);
"mediump"                   return TOKEN(MEDIUM_PRECISION);
"lowp"                      return TOKEN(LOW_PRECISION);
"precision"                 return TOKEN(PRECISION);

false|true                  SAVE_TOKEN; return BOOLCONSTANT;


[_a-zA-Z][_a-zA-Z0-9]*                          { if (gTokenizeMacroFuncDef) { gTokenizeMacroFuncDef = false; } SAVE_TOKEN; return IDENTIFIER; }
[_a-zA-Z][_a-zA-Z0-9]*"("                        { if (gTokenizeMacroFuncDef) { gTokenizeMacroFuncDef = false; SAVE_TOKEN; return MACRO_FUNC_IDENTIFIER; } else { REJECT; } }


[0-9]+\.[0-9]*([eE][-+]?[0-9]+)?[fF]?           SAVE_TOKEN; return FLOATCONSTANT;
[0-9]*\.[0-9]+([eE][-+]?[0-9]+)?[fF]?           SAVE_TOKEN; return FLOATCONSTANT;
[0-9]+[eE][-+]?[0-9]+[fF]?                      SAVE_TOKEN; return FLOATCONSTANT;

[1-9][0-9]*[uU]             SAVE_TOKEN; return UINTCONSTANT;
[1-9][0-9]*                 SAVE_TOKEN; return  INTCONSTANT;
[0-7]+[uU]                  SAVE_TOKEN; return UINTCONSTANT;
[0-7]+                      SAVE_TOKEN; return  INTCONSTANT;
0[xX][0-9a-fA-F]+[uU]       SAVE_TOKEN; return UINTCONSTANT;
0[xX][0-9a-fA-F]+           SAVE_TOKEN; return  INTCONSTANT;
    
\.[xyzw]{1,4}               SAVE_TOKEN; return FIELD_SELECTION;
\.[rgba]{1,4}               SAVE_TOKEN; return FIELD_SELECTION;
\.[stpq]{1,4}               SAVE_TOKEN; return FIELD_SELECTION;

\n                          { ++gLineNum; if (gTokenizeEOL) { gTokenizeEOL = false; return EOL; } }

.                           printf("Unknown token!\n"); yyterminate();

%%