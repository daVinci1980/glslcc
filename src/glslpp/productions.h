#pragma once

#include <vector>
#include "tokens.h"

class Node;
class ProductionBase;
class TokenList;

typedef Node* (*ProductionFunc)();
typedef Node* (*ProcessFunc)(TokenList*);

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
template <typename TokenType>
class RuleSet
{
public:
    inline RuleSet() { }
    inline RuleSet(ProductionFunc _rule) { mRules.push_back(_rule); }
    inline RuleSet(TokenType _tok) { mRules.push_back([]() -> Node* { return nullptr; }); }
    inline RuleSet(ProcessFunc _proc) { mProcs.push_back(_proc); }

    inline RuleSet& operator=(ProductionFunc _rule) { mRules.push_back(_rule); return *this; }
    inline RuleSet& operator=(TokenType _tok) { mRules.push_back([]() -> Node* { return nullptr; }); return *this; }
    inline RuleSet& operator=(ProcessFunc _proc) { mProcs.push_back(_proc); return *this; }    

    inline RuleSet& operator|(ProductionFunc _rule) { mRules.push_back(_rule); return *this; }
    inline RuleSet& operator|(TokenType _tok) { mRules.push_back([]() -> Node* { return nullptr; }); return *this; }
    inline RuleSet& operator|(ProcessFunc _proc) { mProcs.push_back(_proc); return *this; }    
    inline RuleSet& operator|(const RuleSet &_prebuiltRule) { return *this; }

    inline RuleSet& operator&(ProductionFunc _rule) { mRules.push_back(_rule); return *this; }
    inline RuleSet& operator&(TokenType _tok) { mRules.push_back([]() -> Node* { return nullptr; }); return *this; }
    inline RuleSet& operator&(ProcessFunc _proc) { mProcs.push_back(_proc); return *this; }    

    inline void Append(ProductionFunc _rule)
    {
        assert(_rule);
        mRules.push_back(_rule);
    }

private:

    std::vector<ProductionFunc> mRules;
    std::vector<ProcessFunc> mProcs;
};

template<typename TokenType>
Node* Accept(const TokenType& _rs) { return nullptr; }

template<typename TokenType>
Node* Accept(const RuleSet<TokenType>& _rs) { return nullptr; }


template <typename TokenType>
inline RuleSet<TokenType> operator|(ProductionFunc _func, TokenType _tok)
{
    RuleSet<TokenType> rs(_func);
    return rs | _tok;
}

template <typename TokenType>
inline RuleSet<TokenType> operator&(TokenType _tok, ProductionFunc _rule)
{
    RuleSet<TokenType> rs(_tok);
    return rs & _rule;
}

template <typename TokenType>
inline RuleSet<TokenType> operator&(TokenType _tok, ProcessFunc _func)
{
    RuleSet<TokenType> rs(_tok);
    return rs & _func;
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
class Node
{
public:

private:
#if 0
    Node* mFirstChild;
    Node* mNextSibling;
#endif
};

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
class ProductionBase
{
public:
    virtual Node* operator()() = 0;
};

// ------------------------------------------------------------------------------------------------
#define DeclareProduction(_name) \
        extern Node* _name()

// ------------------------------------------------------------------------------------------------
#define DefineProduction(_name) \
    Node* _name()

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
DeclareProduction(variable_identifier);
DeclareProduction(primary_expression);
DeclareProduction(postfix_expression);
DeclareProduction(integer_expression);
DeclareProduction(function_call);
DeclareProduction(function_call_or_method);
DeclareProduction(function_call_generic);
DeclareProduction(function_call_header_no_parameters);
DeclareProduction(function_call_header_with_parameters);
DeclareProduction(function_call_header);
DeclareProduction(function_identifier);
DeclareProduction(unary_expression);
DeclareProduction(unary_operator);
DeclareProduction(multiplicative_expression);
DeclareProduction(additive_expression);
DeclareProduction(shift_expression);
DeclareProduction(relational_expression);
DeclareProduction(equality_expression);
DeclareProduction(and_expression);
DeclareProduction(exclusive_or_expression);
DeclareProduction(inclusive_or_expression);
DeclareProduction(logical_and_expression);
DeclareProduction(logical_xor_expression);
DeclareProduction(logical_or_expression);
DeclareProduction(conditional_expression);
DeclareProduction(assignment_expression);
DeclareProduction(assignment_operator);
DeclareProduction(expression);
DeclareProduction(constant_expression);
DeclareProduction(declaration);
DeclareProduction(function_prototype);
DeclareProduction(function_declarator);
DeclareProduction(function_header_with_parameters);
DeclareProduction(function_header);
DeclareProduction(parameter_declarator);
DeclareProduction(parameter_declaration);
DeclareProduction(parameter_qualifier);
DeclareProduction(parameter_type_specifier);
DeclareProduction(init_declarator_list);
DeclareProduction(single_declaration);
DeclareProduction(fully_specified_type);
DeclareProduction(invariant_qualifier);
DeclareProduction(interpolation_qualifier);
DeclareProduction(layout_qualifier);
DeclareProduction(layout_qualifier_id_list);
DeclareProduction(layout_qualifier_id);
DeclareProduction(parameter_type_qualifier);
DeclareProduction(type_qualifier);
DeclareProduction(storage_qualifier);
DeclareProduction(type_specifier);
DeclareProduction(type_specifier_no_prec);
DeclareProduction(type_specifier_nonarray);
DeclareProduction(precision_qualifier);
DeclareProduction(struct_specifier);
DeclareProduction(struct_declaration_list);
DeclareProduction(struct_declaration);
DeclareProduction(struct_declarator_list);
DeclareProduction(struct_declarator);
DeclareProduction(initializer);
DeclareProduction(declaration_statement);
DeclareProduction(statement);
DeclareProduction(simple_statement);
DeclareProduction(compound_statement);
DeclareProduction(statement_no_new_scope);
DeclareProduction(compound_statement_no_new_scope);
DeclareProduction(statement_list);
DeclareProduction(expression_statement);
DeclareProduction(selection_statement);
DeclareProduction(selection_rest_statement);
DeclareProduction(condition);
DeclareProduction(switch_statement);
DeclareProduction(switch_statement_list);
DeclareProduction(case_label);
DeclareProduction(iteration_statement);
DeclareProduction(for_init_statement);
DeclareProduction(conditionopt);
DeclareProduction(for_rest_statement);
DeclareProduction(jump_statement);
DeclareProduction(translation_unit);
DeclareProduction(external_declaration);
DeclareProduction(function_definition);
