#pragma once

#include <regex>

// A single entry in a table of lexical rules for how to parse a string into tokens. Pattern 
// should be a <regex> conformant string specifying the rule for a single token.
// Tokens are specified in decreasing priority.
struct LexicalEntry
{
	const char* Pattern;
	const int Token;
};

// The sentinel rule, which must always end a list of LexicalEntry's.
extern const LexicalEntry SentinelRule;

class StateObject;
class Token { };

class Lexer
{
public:
	// Rules must end with a SentinelRule (otherwise you will crash).
	// The string is the string which is to be lexed, and must not be NULL.
	// The StateObject is an optional state object which carries the state of the whole
	// lex and parse result, for reentrancy. It is optional.
	Lexer(const LexicalEntry* _rules, const char* _stringToLex, StateObject* _state);
	~Lexer();

	// Peek at the next token and return.
	Token Peek() const;

	// Pop the next token and return.
	Token Pop();

private:
    void ConstructRules(const LexicalEntry* _rules);
    Token FindNextToken();

    struct RuleEntry 
    {
        std::regex Regex;
        const int Token;
    };

private:
    std::vector<RuleEntry> mRules;
    std::string mStringToLex;
    std::string::const_iterator mSrcPos;
	long long mLineNum;
	long long mColNum;
	StateObject* mState;
    Token mLookaheadToken;
};

// This token id indicates that the lexer should grab this token, then ignore it and move to the next
// without raising an error.
extern const int IGNORETOKEN;

// This token id can be returned to indicate that a token has been rejected, and other alternatives should
// be sought.
extern const int REJECTTOKEN;

// This token indicates that a lexer has hit the end of the stream.
extern const int EOFTOKEN;

