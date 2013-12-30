#include "glslppafx.h"
#include "productions.h"
#include "tokens.h"

RuleSet operator|(ProductionFunc _func, GLSLTokenIDs _tok)
{
    RuleSet rs;
    rs.Append(_func);
    rs.Append([]() -> Node* { return nullptr; });
    return rs;
}

RuleSet operator|(GLSLTokenIDs _tok1, GLSLTokenIDs _tok2)
{
    RuleSet rs;
    rs.Append([]() -> Node* { return nullptr; });
    rs.Append([]() -> Node* { return nullptr; });
    return rs;
}

RuleSet& operator|(RuleSet& _rs, ProductionFunc _prod)
{
    _rs.Append(_prod);
    return _rs;
}

RuleSet& operator|(RuleSet& _rs, GLSLTokenIDs _tok)
{
    return _rs;
}

Node* Accept(const RuleSet& _rs) { return nullptr; }

DefineProduction(variable_identifier)
{
    return Accept(IDENTIFIER);
}

DefineProduction(primary_expression) 
{
    Accept(variable_identifier
         | INTCONSTANT
         | UINTCONSTANT
         | FLOATCONSTANT
         | BOOLCONSTANT
         | (LEFT_PAREN, expression, RIGHT_PAREN)
    );
}

DefineProduction(postfix_expression) { }
DefineProduction(integer_expression) { }
DefineProduction(function_call) { }
DefineProduction(function_call_or_method) { }
DefineProduction(function_call_generic) { }
DefineProduction(function_call_header_no_parameters) { }
DefineProduction(function_call_header_with_parameters) { }
DefineProduction(function_call_header) { }
DefineProduction(function_identifier) { }
DefineProduction(unary_expression) { }
DefineProduction(unary_operator) { }
DefineProduction(multiplicative_expression) { }
DefineProduction(additive_expression) { }
DefineProduction(shift_expression) { }
DefineProduction(relational_expression) { }
DefineProduction(equality_expression) { }
DefineProduction(and_expression) { }
DefineProduction(exclusive_or_expression) { }
DefineProduction(inclusive_or_expression) { }
DefineProduction(logical_and_expression) { }
DefineProduction(logical_xor_expression) { }
DefineProduction(logical_or_expression) { }
DefineProduction(conditional_expression) { }
DefineProduction(assignment_expression) { }
DefineProduction(assignment_operator) { }
DefineProduction(expression) { }
DefineProduction(constant_expression) { }
DefineProduction(declaration) { }
DefineProduction(function_prototype) { }
DefineProduction(function_declarator) { }
DefineProduction(function_header_with_parameters) { }
DefineProduction(function_header) { }
DefineProduction(parameter_declarator) { }
DefineProduction(parameter_declaration) { }
DefineProduction(parameter_qualifier) { }
DefineProduction(parameter_type_specifier) { }
DefineProduction(init_declarator_list) { }
DefineProduction(single_declaration) { }
DefineProduction(fully_specified_type) { }
DefineProduction(invariant_qualifier) { }
DefineProduction(interpolation_qualifier) { }
DefineProduction(layout_qualifier) { }
DefineProduction(layout_qualifier_id_list) { }
DefineProduction(layout_qualifier_id) { }
DefineProduction(parameter_type_qualifier) { }
DefineProduction(type_qualifier) { }
DefineProduction(storage_qualifier) { }
DefineProduction(type_specifier) { }
DefineProduction(type_specifier_no_prec) { }
DefineProduction(type_specifier_nonarray) { }
DefineProduction(precision_qualifier) { }
DefineProduction(struct_specifier) { }
DefineProduction(struct_declaration_list) { }
DefineProduction(struct_declaration) { }
DefineProduction(struct_declarator_list) { }
DefineProduction(struct_declarator) { }
DefineProduction(initializer) { }
DefineProduction(declaration_statement) { }
DefineProduction(statement) { }
DefineProduction(simple_statement) { }
DefineProduction(compound_statement) { }
DefineProduction(statement_no_new_scope) { }
DefineProduction(compound_statement_no_new_scope) { }
DefineProduction(statement_list) { }
DefineProduction(expression_statement) { }
DefineProduction(selection_statement) { }
DefineProduction(selection_rest_statement) { }
DefineProduction(condition) { }
DefineProduction(switch_statement) { }
DefineProduction(switch_statement_list) { }
DefineProduction(case_label) { }
DefineProduction(iteration_statement) { }
DefineProduction(for_init_statement) { }
DefineProduction(conditionopt) { }
DefineProduction(for_rest_statement) { }
DefineProduction(jump_statement) { }
DefineProduction(translation_unit) { }
DefineProduction(external_declaration) { }
DefineProduction(function_definition) { }
