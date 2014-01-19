#pragma once

#include "common/lexer.h"

class Node;
class Token;

template <typename Terminal>
using TokenList = std::list<TokenT<Terminal>>;

template <typename Terminal> class ParserT;
template <typename Terminal> class ProductionT;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
/** 
    The base class of all other nodes. Most other nodes are parser-specific, so not defined here. 
*/
class Node
{
public:
    virtual Node* GetLeft() { return mLeft; }
    virtual Node* GetRight() { return mRight; }

private:
    Node* mLeft;
    Node* mRight;
};

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
class TerminalNode : public Node
{
public:
    TerminalNode(TokenT<Terminal> _tok) : mToken(_tok) { }
    virtual Node* GetLeft() { return nullptr; }
    virtual Node* GetRight() { return nullptr; }

    const TokenT<Terminal> mToken;
};

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
template <typename Terminal>
class ParserT
{
public:
    Node* Parse()
    {
        return Accept(this, (TokenList<Terminal>*) 0, StartSymbol());
    }

    void SetCheckpoint() { }
    void RestoreCheckpoint() { }
    void ClearCheckpoint() { }

    TokenT<Terminal> Peek() { return mLexer.Peek(); }
    TokenT<Terminal> Pop() { return mLexer.Pop(); }

protected:
    virtual ProductionT<Terminal> StartSymbol() const = 0;

    ParserT(const LexicalEntry<Terminal>* _rules, const char* _stringToLex, void* _state)
    : mLexer(_rules, _stringToLex, _state)
    { }

private:
    LexerT<Terminal> mLexer;
};

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
template <typename Terminal>
class ProductionT
{ 
public:
    typedef ParserT<Terminal> Parser;
    typedef Node* (*Nonterminal)(Parser*, TokenList<Terminal>*);
    // const int is unused--just to differentiate between nonterminals and SemanticActions
    typedef Node* (*SemanticAction)(Parser*, TokenList<Terminal>*, const int); 

    inline ProductionT()                                     { NewAlternative(); }
    inline ProductionT(Nonterminal _nt)                      { NewAlternative(_nt); }
    inline ProductionT(Terminal _t)                          { NewAlternative(_t); }
    inline ProductionT(SemanticAction _sa)                   { NewAlternative(_sa); }
    inline ~ProductionT()                                    { Clear(); }

    inline ProductionT& operator=(Nonterminal _nt)           { Clear(); NewAlternative(_nt); return *this; }
    inline ProductionT& operator=(Terminal _t)               { Clear(); NewAlternative(_t); return *this; }
    inline ProductionT& operator=(SemanticAction _sa)        { Clear(); NewAlternative(_sa); return *this; }

    inline ProductionT& operator|(Nonterminal _nt)           { NewRule(_nt); return *this; }
    inline ProductionT& operator|(Terminal _t)               { NewRule(_t); return *this; }
    inline ProductionT& operator|(SemanticAction _sa)        { NewRule(_sa); return *this; }
    inline ProductionT& operator|(const ProductionT &_prod)   { NewRule(_prod); return *this; }

    inline ProductionT& operator&(Nonterminal _nt)           { NewAlternative(_nt); return *this; }
    inline ProductionT& operator&(Terminal _t)               { NewAlternative(_t); return *this; }
    inline ProductionT& operator&(SemanticAction _sa)        { NewAlternative(_sa); return *this; }

    Node* Accept(Parser* _p) const
    {
        Node* retVal = nullptr;
        for (auto alternative = mAlternatives.cbegin(); alternative != mAlternatives.cend(); ++alternative) {
            TokenList<Terminal> matchedTokens;

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
        virtual Node* Accept(Parser* _parser, TokenList<Terminal>* _tl) const = 0;
        virtual Rule* Copy() const = 0;
        virtual ~Rule() { }
    };

    // --------------------------------------------------------------------------------------------
    struct RuleTerminal : public Rule
    {
        RuleTerminal(Terminal _t) : mTerminal(_t) { }

        virtual Node* Accept(Parser* _parser, TokenList<Terminal>* _tl) const {
            if (_parser->Peek().GetType() == mTerminal) {
                TokenT<Terminal> tok = _parser->Pop();
                _tl->push_back(tok);
                return new TerminalNode<Terminal>(tok);
            }

            return nullptr;
        }

        virtual Rule* Copy() const {
            return new RuleTerminal(*this);
        }

        const Terminal mTerminal;
    };

    // --------------------------------------------------------------------------------------------
    struct RuleNonterminal : public Rule
    {
        RuleNonterminal(Nonterminal _nt) : mNonterminal(_nt) { }

        virtual Node* Accept(Parser* _parser, TokenList<Terminal>* _tl) const {
            return mNonterminal(_parser, _tl);
        }

        virtual Rule* Copy() const {
            return new RuleNonterminal(*this);
        }        

        const Nonterminal mNonterminal;
    };

    // --------------------------------------------------------------------------------------------
    struct RuleSemanticAction : public Rule
    {
        RuleSemanticAction(SemanticAction _sa) : mSemanticAction(_sa) { }

        virtual Node* Accept(Parser* _parser, TokenList<Terminal>* _tl) const {
            return mSemanticAction(_parser, _tl, 0);
        }

        virtual Rule* Copy() const {
            return new RuleSemanticAction(*this);
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
    void NewRule(const ProductionT &_prod) 
    { 
        // Only ever expect one alternative in the incoming production--would need to decide 
        // what it even means to merge in many rules.
        assert(_prod.mAlternatives.size() == 1); 
        for (auto rule = _prod.mAlternatives.front().begin(); rule != _prod.mAlternatives.front().end(); ++rule) {
            mAlternatives.back().push_back((*rule)->Copy());
        }
    }

    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    std::vector<std::vector<Rule*>> mAlternatives;
};

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
template <typename Terminal>
inline Node* Accept(ParserT<Terminal>* _p, TokenList<Terminal>* _tl, typename ProductionT<Terminal>::Nonterminal _nt)
{
    return _nt(_p, _tl);
}

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
inline Node* Accept(ParserT<Terminal>* _p, TokenList<Terminal>* _tl, const Terminal& _rs) 
{ 
    return nullptr; 
}

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
inline Node* Accept(ParserT<Terminal>* _p, TokenList<Terminal>* _tl, const ProductionT<Terminal>& _rs) 
{ 
    return nullptr; 
}

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
inline ProductionT<Terminal> operator|(typename ProductionT<Terminal>::Nonterminal _func, Terminal _tok)
{
    ProductionT<Terminal> rs(_func);
    return rs | _tok;
}

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
inline ProductionT<Terminal> operator&(Terminal _tok, typename ProductionT<Terminal>::Nonterminal _rule)
{
    ProductionT<Terminal> rs(_tok);
    return rs & _rule;
}

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
inline ProductionT<Terminal> operator&(Terminal _tok, typename ProductionT<Terminal>::SemanticAction _func)
{
    ProductionT<Terminal> rs(_tok);
    return rs & _func;
}

