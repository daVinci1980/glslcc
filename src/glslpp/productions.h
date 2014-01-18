#pragma once

#include <list>
#include <vector>
#include "tokens.h"

class Node;
class Parser;
class ProductionBase;
class Token;
typedef std::list<Token*> TokenList;

typedef Node* (*Nonterminal)(Parser*, TokenList*);
// const int is unused--just to differentiate between nonterminals and SemanticActions
typedef Node* (*SemanticAction)(Parser*, TokenList*, const int); 

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
template <typename Terminal>
class Production
{
public:
    inline Production()                                     { NewAlternative(); }
    inline Production(Nonterminal _nt)                      { NewAlternative(_nt); }
    inline Production(Terminal _t)                          { NewAlternative(_t); }
    inline Production(SemanticAction _sa)                   { NewAlternative(_sa); }
    inline ~Production()                                    { Clear(); }

    inline Production& operator=(Nonterminal _nt)           { Clear(); NewAlternative(_nt); return *this; }
    inline Production& operator=(Terminal _t)               { Clear(); NewAlternative(_t); return *this; }
    inline Production& operator=(SemanticAction _sa)        { Clear(); NewAlternative(_sa); return *this; }

    inline Production& operator|(Nonterminal _nt)           { NewRule(_nt); return *this; }
    inline Production& operator|(Terminal _t)               { NewRule(_t); return *this; }
    inline Production& operator|(SemanticAction _sa)        { NewRule(_sa); return *this; }
    inline Production& operator|(const Production &_prod)   { NewRule(_prod); return *this; }

    inline Production& operator&(Nonterminal _nt)           { NewAlternative(_nt); return *this; }
    inline Production& operator&(Terminal _t)               { NewAlternative(_t); return *this; }
    inline Production& operator&(SemanticAction _sa)        { NewAlternative(_sa); return *this; }

    Node* Accept(Parser* _p) const
    {
        Node* retVal = nullptr;
        for (auto alternative = mAlternatives.cbegin(); alternative != mAlternatives.cend(); ++alternative) {
            TokenList matchedTokens;

            // Right now, just using backtracking. Should do something better.
            _p->SetCheckpoint();

            for (auto rule = alternative->cbegin(); rule != alternative->cend(); ++rule) {
                retVal = (*rule)->Accept(_p, matchedTokens);
                if (retVal) {
                    break;
                }
            }

            if (retVal) { 
                _p->ClearCheckpoint();
                break;
            }

            _p->RestoreCheckpoint();
        }

        return retVal;
    }

private:
    // --------------------------------------------------------------------------------------------
    struct Rule
    {
        virtual Node* Accept(Parser* _parser, TokenList* _tl) const = 0;
    };

    // --------------------------------------------------------------------------------------------
    struct RuleTerminal : public Rule
    {
        RuleTerminal(Terminal _t) : mTerminal(_t) { }

        virtual Node* Accept(Parser* _parser, TokenList* _tl) const {
        #if 0
            if (_parser->Peek().GetType() == mTerminal) {
                Token tok = _parser->Pop();
                _tl->push_back(tok);
                return new Node(tok);
            }
        #endif

            return nullptr;
        }

        const Terminal mTerminal;
    };

    // --------------------------------------------------------------------------------------------
    struct RuleNonterminal : public Rule
    {
        RuleNonterminal(Nonterminal _nt) : mNonterminal(_nt) { }

        virtual Node* Accept(Parser* _parser, TokenList* _tl) const {
            return mNonterminal(_parser, _tl);
        }

        const Nonterminal mNonterminal;
    };

    // --------------------------------------------------------------------------------------------
    struct RuleSemanticAction : public Rule
    {
        RuleSemanticAction(SemanticAction _sa) : mSemanticAction(_sa) { }

        virtual Node* Accept(Parser* _parser, TokenList* _tl) const {
            return mSemanticAction(_parser, _tl, 0);
        }

        const SemanticAction mSemanticAction;

    };

    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    void Clear()
    {
        for (auto alternative = mAlternatives.begin(); alternative != mAlternatives.end(); ++alternative) {
            for (auto rule = alternative->begin(); rule != alternative->end(); ++rule) {
                delete (*rule);
            }
        }

        mAlternatives.clear();
    }

    void NewAlternative()                       { mAlternatives.push_back(std::vector<Rule*>()); }
    void NewAlternative(Nonterminal _nt)        { NewAlternative(); NewRule(_nt); }
    void NewAlternative(Terminal _t)            { NewAlternative(); NewRule(_t); }
    void NewAlternative(SemanticAction _sa)     { NewAlternative(); NewRule(_sa); }

    void NewRule(Nonterminal _nt)               { mAlternatives.back().push_back(new RuleNonterminal(_nt)); }
    void NewRule(Terminal _t)                   { mAlternatives.back().push_back(new RuleTerminal(_t)); }
    void NewRule(SemanticAction _sa)            { mAlternatives.back().push_back(new RuleSemanticAction(_sa)); }
    void NewRule(const Production &_prod) { 
        // Only ever expect one alternative in the incoming production--would need to decide 
        // what it even means to merge in many rules.
        assert(_prod.mAlternatives.size() == 1); 
        for (auto rule = _prod.mAlternatives.front().begin(); rule != _prod.mAlternatives.front().end(); ++rule) {
            
        }
    }

    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    std::vector<std::vector<Rule*>> mAlternatives;
};


inline Node* Accept(Parser* _p, TokenList* _tl, Nonterminal _nt)
{
    return _nt(_p, _tl);
}

template<typename Terminal>
inline Node* Accept(Parser* _p, TokenList* _tl, const Terminal& _rs) { return nullptr; }

template<typename Terminal>
inline Node* Accept(Parser* _p, TokenList* _tl, const Production<Terminal>& _rs) { return nullptr; }


template <typename Terminal>
inline Production<Terminal> operator|(Nonterminal _func, Terminal _tok)
{
    Production<Terminal> rs(_func);
    return rs | _tok;
}

template <typename Terminal>
inline Production<Terminal> operator&(Terminal _tok, Nonterminal _rule)
{
    Production<Terminal> rs(_tok);
    return rs & _rule;
}

template <typename Terminal>
inline Production<Terminal> operator&(Terminal _tok, SemanticAction _func)
{
    Production<Terminal> rs(_tok);
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
        extern Node* _name(Parser* _p, TokenList* _tl)

// ------------------------------------------------------------------------------------------------
#define DefineProduction(_name) \
    Node* _name(Parser* _p, TokenList* _tl)
    

#define SemanticAction [](Parser* _p, TokenList* _tl, const int) -> Node*

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
