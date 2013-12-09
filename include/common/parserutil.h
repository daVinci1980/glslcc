#pragma once

#include <regex>
#include <set>
#include <vector>


class StateObject;

typedef long long StreamOffset;
typedef int (*CheckFunc)(const std::string& _match, const int _initialToken, StateObject* _state);

// A single entry in a table of lexical rules for how to parse a string into tokens. Pattern 
// should be a <regex> conformant string specifying the rule for a single token.
// Tokens are specified in decreasing priority.
struct LexicalEntry
{
	const char* Pattern;
	const int Token;
    CheckFunc TokenCallback;
};

// A position within the stream. Useful for identifying the location of tokens, errors, etc.
struct StreamPosition
{
    StreamPosition(StreamOffset _lineNum = 1, StreamOffset _colNum = 1);

    StreamOffset mLineNum;
    StreamOffset mColNum;

    friend std::ostream& operator<<(std::ostream& _os, const StreamPosition& _streamPos);
};

// The sentinel rule, which must always end a list of LexicalEntry's.
extern const LexicalEntry SentinelRule;

class StateObject
{
public:
    // ReservedTypes is a list of strings with nullptr as the last element.
    StateObject(const char** _reservedTypes);

    bool IsValidType(const std::string& _identifier) const;

private:
    std::set<std::string> mTypes;
};

class Token 
{
public:
    // The default constructor sets parameters to REJECTTOKEN, "", StreamPosition(1, 1)
    Token();
    Token(int _streamTok, const std::string& _tokenString, StreamPosition _streamPos); 

    bool IsEOF() const;

private:
    int mType;
    std::string mString;
    StreamPosition mLocation;

    friend std::ostream& operator<<(std::ostream& _os, const Token& _token);
};

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
        CheckFunc TokenCallback;    
    };

private:
    std::vector<RuleEntry> mRules;
    std::string mStringToLex;
    std::string::const_iterator mSrcPos;
    StreamPosition mStreamPosition;
	StateObject* mState;
    Token mLookaheadToken;

    std::set<std::string> mTypes;
};

// This token id indicates that the lexer should grab this token, then ignore it and move to the next
// without raising an error.
extern const int IGNORETOKEN;

// This token id can be returned to indicate that a token has been rejected, and other alternatives should
// be sought.
extern const int REJECTTOKEN;

// This token indicates that a lexer has hit the end of the stream.
extern const int EOFTOKEN;

