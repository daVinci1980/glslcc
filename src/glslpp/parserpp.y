%{
    // #include "node.h"
    #include <string>
    #include <stdio.h>

    extern int yylex();
    extern int gLineNum; 
    extern char* gFileName;

    void yyerror(const char *s) { printf("%s(%d): Error: %s\n", gFileName, gLineNum, s); }
%}

%union {
    std::string *string;
    int token;
    // NBlock* block;
    // NBlock* node;
}

%error-verbose

%token <token> PPHASH PPDEFINE PPUNDEF PPIF PPIFDEF PPIFNDEF PPELSE PPELIF PPENDIF 
%token <token> PPERROR PPPRAGMA PPEXTENSION PPVERSION PPLINE PPSTDGL PPDEFINED PPPASTING
%token <token> MACRO_FUNC_IDENTIFIER EOL

%token <token> ATTRIBUTE CONST BOOL FLOAT DOUBLE INT UINT
%token <token> BREAK CONTINUE DO ELSE FOR IF DISCARD RETURN SWITCH CASE DEFAULT SUBROUTINE
%token <token> BVEC2 BVEC3 BVEC4 IVEC2 IVEC3 IVEC4 UVEC2 UVEC3 UVEC4 VEC2 VEC3 VEC4
%token <token> MAT2 MAT3 MAT4 CENTROID IN OUT INOUT 
%token <token> UNIFORM VARYING PATCH SAMPLE
%token <token> DVEC2 DVEC3 DVEC4 DMAT2 DMAT3 DMAT4

%token <token> NOPERSPECTIVE FLAT SMOOTH LAYOUT 
%token <token> MAT2X2 MAT2X3 MAT2X4
%token <token> MAT3X2 MAT3X3 MAT3X4
%token <token> MAT4X2 MAT4X3 MAT4X4
%token <token> DMAT2X2 DMAT2X3 DMAT2X4
%token <token> DMAT3X2 DMAT3X3 DMAT3X4
%token <token> DMAT4X2 DMAT4X3 DMAT4X4

%token <token> SAMPLER1D SAMPLER2D SAMPLER3D SAMPLERCUBE SAMPLER1DSHADOW SAMPLER2DSHADOW
%token <token> SAMPLERCUBESHADOW SAMPLER1DARRAY SAMPLER2DARRAY SAMPLER1DARRAYSHADOW
%token <token> SAMPLER2DARRAYSHADOW ISAMPLER1D ISAMPLER2D ISAMPLER3D ISAMPLERCUBE
%token <token> ISAMPLER1DARRAY ISAMPLER2DARRAY USAMPLER1D USAMPLER2D USAMPLER3D
%token <token> USAMPLERCUBE USAMPLER1DARRAY USAMPLER2DARRAY
%token <token> SAMPLER2DRECT SAMPLER2DRECTSHADOW ISAMPLER2DRECT USAMPLER2DRECT
%token <token> SAMPLERBUFFER ISAMPLERBUFFER USAMPLERBUFFER
%token <token> SAMPLERCUBEARRAY SAMPLERCUBEARRAYSHADOW 
%token <token> ISAMPLERCUBEARRAY USAMPLERCUBEARRAY
%token <token> SAMPLER2DMS ISAMPLER2DMS USAMPLER2DMS
%token <token> SAMPLER2DMSARRAY ISAMPLER2DMSARRAY USAMPLER2DMSARRAY

%token <token> STRUCT VOID WHILE
%token <string> IDENTIFIER 
%token <string> FLOATCONSTANT INTCONSTANT UINTCONSTANT BOOLCONSTANT
%token <string> FIELD_SELECTION
%token <token> LEFT_OP RIGHT_OP
%token <token> INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP
%token <token> AND_OP OR_OP XOR_OP MUL_ASSIGN DIV_ASSIGN ADD_ASSIGN
%token <token> MOD_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token <token> SUB_ASSIGN
%token <token> LEFT_PAREN RIGHT_PAREN LEFT_BRACKET RIGHT_BRACKET LEFT_BRACE RIGHT_BRACE DOT
%token <token> COMMA COLON EQUAL SEMICOLON BANG DASH TILDE PLUS STAR SLASH PERCENT
%token <token> LEFT_ANGLE RIGHT_ANGLE VERTICAL_BAR CARET AMPERSAND QUESTION
%token <token> INVARIANT
%token <token> HIGH_PRECISION MEDIUM_PRECISION LOW_PRECISION PRECISION

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */

%type <block> translation_unit
%type <node> external_declaration

/* 
    Operator precedence for mathematical operators 
%left PLUS DASH
%left MUL DIV

// TODO: Need to figure out how to do right-to-left for ppunary_expression.
*/

%start translation_unit

%%
TYPE_NAME: IDENTIFIER { }
         ;

ppversion_line: PPVERSION INTCONSTANT EOL
              ;

ppdefine_line: PPDEFINE IDENTIFIER tokenlistopt EOL { }
             | PPDEFINE MACRO_FUNC_IDENTIFIER variable_identifier_list RIGHT_PAREN tokenlistopt EOL { } 
             ;

ppundef_line: PPUNDEF IDENTIFIER EOL { }
            ;

ppvariable_identifier: IDENTIFIER { }
                     ;

ppprimary_expression: ppvariable_identifier { }
                    | INTCONSTANT { }
                    | UINTCONSTANT { }
                    | FLOATCONSTANT { }
                    | BOOLCONSTANT { }
                    | LEFT_PAREN ppexpression RIGHT_PAREN { }
                    ;

ppunary_expression: ppprimary_expression { }
                  | PPDEFINED IDENTIFIER { }
                  | PPDEFINED LEFT_PAREN IDENTIFIER RIGHT_PAREN { }
                  | ppunary_operator ppunary_expression { }
                  ;

ppunary_operator: PLUS { }
                | DASH { }
                | BANG { }
                | TILDE { }
                ;

ppmultiplicative_expression: ppunary_expression { }
                           | ppmultiplicative_expression STAR ppunary_expression
                           | ppmultiplicative_expression SLASH ppunary_expression
                           | ppmultiplicative_expression PERCENT ppunary_expression
                           ;

ppadditive_expression: ppmultiplicative_expression { }
                     | ppadditive_expression PLUS ppmultiplicative_expression { }
                     | ppadditive_expression DASH ppmultiplicative_expression { }
                     ;

ppshift_expression: ppadditive_expression { }
                  | ppshift_expression LEFT_OP ppadditive_expression { }
                  | ppshift_expression RIGHT_OP ppadditive_expression { }
                  ;

pprelational_expression: ppshift_expression { }
                       | pprelational_expression LEFT_ANGLE ppshift_expression { }
                       | pprelational_expression RIGHT_ANGLE ppshift_expression { }
                       | pprelational_expression LE_OP ppshift_expression { }
                       | pprelational_expression GE_OP ppshift_expression { }
                       ;

ppequality_expression: pprelational_expression { }
                     | ppequality_expression EQ_OP pprelational_expression { }
                     | ppequality_expression NE_OP pprelational_expression { }
                     ;

ppand_expression: ppequality_expression { }
                | ppand_expression AMPERSAND ppequality_expression { }
                ;

ppexclusive_or_expression: ppand_expression { }
                         | ppexclusive_or_expression CARET ppand_expression { } 
                         ;

ppinclusive_or_expression: ppexclusive_or_expression { }
                         | ppinclusive_or_expression VERTICAL_BAR ppexclusive_or_expression { }
                         ;

pplogical_and_expression: ppinclusive_or_expression { }
                        | pplogical_and_expression AND_OP ppinclusive_or_expression { }
                        ; 

pplogical_or_expression: pplogical_and_expression { }
                       | pplogical_or_expression OR_OP pplogical_and_expression { }
                       ;

ppexpression: pplogical_or_expression { }
            | ppexpression COMMA pplogical_or_expression { }
            ;


ppif_line: PPIF ppexpression EOL { }
         ;

ppdef_check: ppvariable_identifier { }
           | LEFT_PAREN ppvariable_identifier RIGHT_PAREN { }
           ;

ppifdef_line: PPIFDEF ppdef_check EOL { }
            ;

ppifndef_line: PPIFNDEF ppdef_check EOL { }
             ;

ppelse_line: PPELSE EOL { }
           ;

ppelif_line: PPELIF ppexpression EOL { }
           ;

ppendif_line: PPENDIF EOL { }
            ;

pperror_line: PPERROR tokenlistopt EOL { }
            ;

pppragma_line: PPPRAGMA PPSTDGL EOL { }
             | PPPRAGMA IDENTIFIER LEFT_PAREN IDENTIFIER RIGHT_PAREN EOL { }
             ;

ppextension_line: PPEXTENSION IDENTIFIER COLON IDENTIFIER EOL { }
                ;

ppline_line: PPLINE INTCONSTANT EOL
           | PPLINE INTCONSTANT INTCONSTANT EOL
           ;

preprocessor_line: PPHASH { }
                 | ppversion_line { }
                 | ppdefine_line { }
                 | ppundef_line { }
                 | ppif_line { }
                 | ppifdef_line { }
                 | ppifndef_line { }
                 | ppelse_line { }
                 | ppelif_line { }
                 | ppendif_line { }
                 | pperror_line { }
                 | pppragma_line { }
                 | ppextension_line { }
                 | ppline_line { }
                 ;

variable_identifier: IDENTIFIER { }

variable_identifier_list: variable_identifier
                        | variable_identifier_list COMMA variable_identifier
                        ;

primary_expression: variable_identifier { }
                  | INTCONSTANT { }
                  | UINTCONSTANT { }
                  | FLOATCONSTANT { }
                  | BOOLCONSTANT { }
                  | LEFT_PAREN expression RIGHT_PAREN { }
                  ;

postfix_expression: primary_expression { }
                  | postfix_expression LEFT_BRACKET integer_expression RIGHT_BRACKET { }
                  | function_call { } 
                  | postfix_expression DOT FIELD_SELECTION { }
                  | postfix_expression INC_OP { }
                  | postfix_expression DEC_OP { }
                  ;

integer_expression: expression { }
                  ;

function_call: function_call_or_method { }
             ;

function_call_or_method: function_call_generic { }
                       | postfix_expression DOT function_call_generic { }
                       ;
 
function_call_generic: function_call_header_with_parameters RIGHT_PAREN { }
                     | function_call_header_no_parameters RIGHT_PAREN { }
                     ;

function_call_header_no_parameters: function_call_header VOID { }
                                  | function_call_header { }
                                  ;

function_call_header_with_parameters: function_call_header assignment_expression { }
                                    | function_call_header_with_parameters COMMA assignment_expression { }
                                    ;

function_call_header: function_identifier LEFT_PAREN { }
                    ;

function_identifier: type_specifier { }
                   | FIELD_SELECTION { }
                   ;

unary_expression: postfix_expression { }
                | INC_OP unary_expression { }
                | DEC_OP unary_expression { }
                | unary_operator unary_expression { }
                ;

unary_operator: PLUS { }
              | DASH { }
              | BANG { }
              | TILDE { }
              ;

multiplicative_expression: unary_expression { }
                         | multiplicative_expression STAR unary_expression
                         | multiplicative_expression SLASH unary_expression
                         | multiplicative_expression PERCENT unary_expression
                         ;

additive_expression: multiplicative_expression { }
                   | additive_expression PLUS multiplicative_expression { }
                   | additive_expression DASH multiplicative_expression { }
                   ;

shift_expression: additive_expression { }
                | shift_expression LEFT_OP additive_expression { }
                | shift_expression RIGHT_OP additive_expression { }
                ;

relational_expression: shift_expression { }
                     | relational_expression LEFT_ANGLE shift_expression { }
                     | relational_expression RIGHT_ANGLE shift_expression { }
                     | relational_expression LE_OP shift_expression { }
                     | relational_expression GE_OP shift_expression { }
                     ;

equality_expression: relational_expression { }
                   | equality_expression EQ_OP relational_expression { }
                   | equality_expression NE_OP relational_expression { }
                   ;

and_expression: equality_expression { }
              | and_expression AMPERSAND equality_expression { }
              ;

exclusive_or_expression: and_expression { }
                       | exclusive_or_expression CARET and_expression { } 
                       ;

inclusive_or_expression: exclusive_or_expression { }
                       | inclusive_or_expression VERTICAL_BAR exclusive_or_expression { }
                       ;

logical_and_expression: inclusive_or_expression { }
                      | logical_and_expression AND_OP inclusive_or_expression { }
                      ; 

logical_xor_expression: logical_and_expression { }
                      | logical_xor_expression XOR_OP logical_and_expression { }
                      ;

logical_or_expression: logical_xor_expression { }
                     | logical_or_expression OR_OP logical_xor_expression { }
                     ;

conditional_expression: logical_or_expression { }
                      | logical_or_expression QUESTION expression COLON assignment_expression { }
                      ;

assignment_expression: conditional_expression { }
                     | unary_expression assignment_operator assignment_expression { }
                     ;

assignment_operator: EQUAL { }
                   | MUL_ASSIGN { }
                   | DIV_ASSIGN { }
                   | MOD_ASSIGN { }
                   | ADD_ASSIGN { }
                   | SUB_ASSIGN { }
                   | LEFT_ASSIGN { }
                   | RIGHT_ASSIGN { }
                   | AND_ASSIGN { }
                   | XOR_ASSIGN { }
                   | OR_ASSIGN { }
                   ;

expression: assignment_expression { }
          | expression COMMA assignment_expression { }
          ;

constant_expression: conditional_expression { }
                   ;

declaration: function_prototype SEMICOLON { }
           | init_declarator_list SEMICOLON { }
           | PRECISION precision_qualifier type_specifier_no_prec SEMICOLON { }
           | type_qualifier IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE SEMICOLON { }
           | type_qualifier IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE IDENTIFIER SEMICOLON { }
           | type_qualifier IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE IDENTIFIER LEFT_BRACKET RIGHT_BRACKET SEMICOLON { }
           | type_qualifier IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET SEMICOLON { }
           | type_qualifier SEMICOLON { }
           ; 

function_prototype: function_declarator RIGHT_PAREN { }
                  ; 

function_declarator: function_header { }
                   | function_header_with_parameters { }
                   ;

function_header_with_parameters: function_header parameter_declaration { }
                               | function_header_with_parameters COMMA parameter_declaration { }
                               ;

function_header: fully_specified_type IDENTIFIER LEFT_PAREN { }
               ;

parameter_declarator: type_specifier IDENTIFIER { }
                    | type_specifier IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET { }
                    ;

parameter_declaration: parameter_type_qualifier parameter_qualifier parameter_declarator { }
                     | parameter_qualifier parameter_declarator { }
                     | parameter_type_qualifier parameter_qualifier parameter_type_specifier { }
                     | parameter_qualifier parameter_type_specifier { }
                     ;

parameter_qualifier: /* empty */ { }
                   | IN { }
                   | OUT { }
                   | INOUT { }
                   ;

parameter_type_specifier: type_specifier { }
                        ;

init_declarator_list: single_declaration { }
                    | init_declarator_list COMMA IDENTIFIER { }
                    | init_declarator_list COMMA IDENTIFIER LEFT_BRACKET RIGHT_BRACKET { }
                    | init_declarator_list COMMA IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET { }
                    | init_declarator_list COMMA IDENTIFIER LEFT_BRACKET RIGHT_BRACKET EQUAL initializer { }
                    | init_declarator_list COMMA IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET EQUAL initializer { }
                    | init_declarator_list COMMA IDENTIFIER EQUAL initializer { }
                    ;

single_declaration: fully_specified_type { }
                  | fully_specified_type IDENTIFIER { }
                  | fully_specified_type IDENTIFIER LEFT_BRACKET RIGHT_BRACKET { }
                  | fully_specified_type IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET { }
                  | fully_specified_type IDENTIFIER LEFT_BRACKET RIGHT_BRACKET EQUAL initializer { }
                  | fully_specified_type IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET EQUAL initializer { }
                  | fully_specified_type IDENTIFIER EQUAL initializer INVARIANT IDENTIFIER { }
                  ;

fully_specified_type: type_specifier { }
                    | type_qualifier type_specifier { }
                    ;

invariant_qualifier: INVARIANT { }
                   ;

interpolation_qualifier: SMOOTH { }
                       | FLAT { }
                       | NOPERSPECTIVE { }
                       ;

layout_qualifier: LAYOUT LEFT_PAREN layout_qualifier_id_list RIGHT_PAREN { }
                ; 

layout_qualifier_id_list: layout_qualifier_id { }
                        | layout_qualifier_id_list COMMA layout_qualifier_id { }
                        ;

layout_qualifier_id: IDENTIFIER { }
                   | IDENTIFIER EQUAL INTCONSTANT { }
                   ;

parameter_type_qualifier: CONST { }
                 ; 

type_qualifier: storage_qualifier { }
              | layout_qualifier { }
              | layout_qualifier storage_qualifier { }
              | interpolation_qualifier storage_qualifier { }
              | interpolation_qualifier { }
              | invariant_qualifier storage_qualifier { } 
              | invariant_qualifier interpolation_qualifier storage_qualifier { } 
              | INVARIANT { }
              ;

storage_qualifier: CONST { }
                 | ATTRIBUTE { }
                 | VARYING { }
                 | CENTROID VARYING { }
                 | IN { } 
                 | OUT { }
                 | CENTROID IN { }
                 | CENTROID OUT { }
                 | PATCH IN { }
                 | PATCH OUT { }
                 | SAMPLE IN { }
                 | SAMPLE OUT { }
                 | UNIFORM { }
                 ;

type_specifier: type_specifier_no_prec { }
              | precision_qualifier type_specifier_no_prec { }
              ;

type_specifier_no_prec: type_specifier_nonarray { } 
                      | type_specifier_nonarray LEFT_BRACKET RIGHT_BRACKET { }
                      | type_specifier_nonarray LEFT_BRACKET constant_expression RIGHT_BRACKET { }
                      ;

type_specifier_nonarray: VOID { }
                       | FLOAT { }
                       | DOUBLE { }
                       | INT { }
                       | UINT { }
                       | BOOL { }
                       | VEC2 { }
                       | VEC3 { }
                       | VEC4 { }
                       | DVEC2 { }
                       | DVEC3 { }
                       | DVEC4 { }
                       | BVEC2 { }
                       | BVEC3 { }
                       | BVEC4 { }
                       | IVEC2 { }
                       | IVEC3 { }
                       | IVEC4 { }
                       | UVEC2 { }
                       | UVEC3 { }
                       | UVEC4 { }
                       | MAT2 { }
                       | MAT3 { }
                       | MAT4 { }
                       | MAT2X2 { }
                       | MAT2X3 { }
                       | MAT2X4 { }
                       | MAT3X2 { }
                       | MAT3X3 { }
                       | MAT3X4 { }
                       | MAT4X2 { }
                       | MAT4X3 { }
                       | MAT4X4 { }
                       | DMAT2 { }
                       | DMAT3 { }
                       | DMAT4 { }
                       | DMAT2X2 { }
                       | DMAT2X3 { }
                       | DMAT2X4 { }
                       | DMAT3X2 { }
                       | DMAT3X3 { }
                       | DMAT3X4 { }
                       | DMAT4X2 { }
                       | DMAT4X3 { }
                       | DMAT4X4 { }
                       | SAMPLER1D { }
                       | SAMPLER2D { }
                       | SAMPLER3D { }
                       | SAMPLERCUBE { }
                       | SAMPLER1DSHADOW { }
                       | SAMPLER2DSHADOW { }
                       | SAMPLERCUBESHADOW { }
                       | SAMPLER1DARRAY { }
                       | SAMPLER2DARRAY { }
                       | SAMPLER1DARRAYSHADOW { }
                       | SAMPLER2DARRAYSHADOW { }
                       | SAMPLERCUBEARRAY { }
                       | SAMPLERCUBEARRAYSHADOW { }
                       | ISAMPLER1D { }
                       | ISAMPLER2D { }
                       | ISAMPLER3D { }
                       | ISAMPLERCUBE { }
                       | ISAMPLER1DARRAY { }
                       | ISAMPLER2DARRAY { }
                       | ISAMPLERCUBEARRAY { }
                       | USAMPLER1D { }
                       | USAMPLER2D { }
                       | USAMPLER3D { }
                       | USAMPLERCUBE { }
                       | USAMPLER1DARRAY { }
                       | USAMPLER2DARRAY { }
                       | USAMPLERCUBEARRAY { }
                       | SAMPLER2DRECT { }
                       | SAMPLER2DRECTSHADOW { }
                       | ISAMPLER2DRECT { }
                       | USAMPLER2DRECT { }
                       | SAMPLERBUFFER { }
                       | ISAMPLERBUFFER { }
                       | USAMPLERBUFFER { }
                       | SAMPLER2DMS { }
                       | ISAMPLER2DMS { }
                       | USAMPLER2DMS { }
                       | SAMPLER2DMSARRAY { }
                       | ISAMPLER2DMSARRAY { }
                       | USAMPLER2DMSARRAY { }
                       | struct_specifier { }
                       | TYPE_NAME { }
                       ;

precision_qualifier: HIGH_PRECISION { }
                   | MEDIUM_PRECISION { }
                   | LOW_PRECISION { }
                   ;

struct_specifier: STRUCT IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE { }
                | STRUCT LEFT_BRACE struct_declaration_list RIGHT_BRACE { }
                ;

struct_declaration_list: struct_declaration { }
                       | struct_declaration_list struct_declaration { }
                       ;

struct_declaration: type_specifier struct_declarator_list SEMICOLON { }
                  | type_qualifier type_specifier struct_declarator_list SEMICOLON { }
                  ;

struct_declarator_list: struct_declarator { }
                      | struct_declarator_list COMMA struct_declarator { }
                      ;

struct_declarator: IDENTIFIER { }
                 | IDENTIFIER LEFT_BRACKET RIGHT_BRACKET { }
                 | IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET { }
                 ;

initializer: assignment_expression { }
           ;

declaration_statement: declaration { }
                     ;

statement: compound_statement { }
         | simple_statement { }
         ;

simple_statement: declaration_statement { }
                | expression_statement { }
                | selection_statement { }
                | switch_statement { }
                | case_label { }
                | iteration_statement { }
                | jump_statement { }
                ;

compound_statement: LEFT_BRACE RIGHT_BRACE { }
                  | LEFT_BRACE statement_list RIGHT_BRACE { }
                  ;

statement_no_new_scope: compound_statement_no_new_scope { }
                      | simple_statement { }
                      ;

compound_statement_no_new_scope: LEFT_BRACE RIGHT_BRACE { }
                               | LEFT_BRACE statement_list RIGHT_BRACE { }
                               ;

statement_list: statement { }
              | statement_list statement { }
              ;

expression_statement: SEMICOLON { }
                    | expression SEMICOLON { }
                    ;

selection_statement: IF LEFT_PAREN expression RIGHT_PAREN selection_rest_statement { }
                   ;

selection_rest_statement: statement ELSE statement { }
                        | statement { }
                        ;

condition: expression { }
         | fully_specified_type IDENTIFIER EQUAL initializer { }
         ;

switch_statement: SWITCH LEFT_PAREN expression RIGHT_PAREN LEFT_BRACE switch_statement_list RIGHT_BRACE { }
                ;

switch_statement_list: /* empty */ { }
                     | statement_list { }
                     ;

case_label: CASE expression COLON { }
          | DEFAULT COLON { }
          ;

iteration_statement: WHILE LEFT_PAREN condition RIGHT_PAREN statement_no_new_scope { }
                   | DO statement WHILE LEFT_PAREN expression RIGHT_PAREN SEMICOLON { }
                   | FOR LEFT_PAREN for_init_statement for_rest_statement RIGHT_PAREN statement_no_new_scope { }
                   ;

for_init_statement: expression_statement { }
                  | declaration_statement { }
                  ;

conditionopt: /* empty */ { }
            | condition { }
            ;

for_rest_statement: conditionopt SEMICOLON { }
                  | conditionopt SEMICOLON expression { }
                  ;

jump_statement: CONTINUE SEMICOLON { }
              | BREAK SEMICOLON { } 
              | RETURN SEMICOLON { } 
              | RETURN expression SEMICOLON { }
              | DISCARD SEMICOLON { }
              ;

translation_unit : /* empty */ { }
                 | preprocessor_line { }
                 | external_declaration { }
                 | translation_unit external_declaration  { } 
                 ;

external_declaration: function_definition { }
                    | declaration { }
                    ;

function_definition: function_prototype compound_statement_no_new_scope { }
                   ;

tokenlistopt: /* empty */
            | tokenlist
            ; 

tokenlist: token
         | tokenlist token;

token: PPDEFINED
     | PPPASTING
     | IDENTIFIER
     | LEFT_OP
     | RIGHT_OP
     | INC_OP
     | DEC_OP
     | LE_OP
     | GE_OP
     | EQ_OP
     | NE_OP
     | AND_OP
     | OR_OP
     | XOR_OP
     | MUL_ASSIGN
     | DIV_ASSIGN
     | ADD_ASSIGN
     | MOD_ASSIGN
     | LEFT_ASSIGN
     | RIGHT_ASSIGN
     | AND_ASSIGN
     | XOR_ASSIGN
     | OR_ASSIGN
     | SUB_ASSIGN
     | LEFT_PAREN
     | RIGHT_PAREN
     | LEFT_BRACKET
     | RIGHT_BRACKET
     | LEFT_BRACE
     | RIGHT_BRACE
     | DOT
     | COMMA
     | COLON
     | EQUAL
     | SEMICOLON
     | BANG
     | DASH
     | TILDE
     | PLUS
     | STAR
     | SLASH
     | PERCENT
     | LEFT_ANGLE
     | RIGHT_ANGLE
     | VERTICAL_BAR
     | CARET
     | AMPERSAND
     | QUESTION
     | FLOATCONSTANT
     | UINTCONSTANT
     | INTCONSTANT
     | FIELD_SELECTION
     ;

%%