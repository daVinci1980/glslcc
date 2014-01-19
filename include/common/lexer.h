#pragma once

#include <regex>
#include <set>
#include <vector>


typedef long long StreamOffset;

// A single entry in a table of lexical rules for how to parse a string into tokens. Pattern 
// should be a <regex> conformant string specifying the rule for a single token.
// Tokens are specified in decreasing priority.
template <typename Terminal> 
struct LexicalEntry
{
    typedef Terminal (*TCheckFunc)(const std::string& _match, const Terminal _initialToken, void* _state);

	const char* Pattern;
	const Terminal Token;
    TCheckFunc TokenCallback;
};

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// A position within the stream. Useful for identifying the location of tokens, errors, etc.
struct StreamPosition
{
    inline StreamPosition(StreamOffset _lineNum = 1, StreamOffset _colNum = 1)
    : mLineNum(_lineNum)
    , mColNum(_colNum)
    { }

    StreamOffset mLineNum;
    StreamOffset mColNum;

    inline friend std::ostream& operator<<(std::ostream& _os, const StreamPosition& _streamPos)
    {
        return _os << "StreamPosition(" << _streamPos.mLineNum
                   << ", " << _streamPos.mColNum
                   << ")";
    }
};

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
template <typename Terminal>
class TokenT
{
public:
    // The default constructor sets parameters to REJECTTOKEN, "", StreamPosition(1, 1)
    inline TokenT() 
    : mType(Terminal::REJECTTOKEN)
    , mString("")
    { }
    
    inline TokenT(Terminal _streamTok, const std::string& _tokenString, StreamPosition _streamPos)
    : mType(_streamTok)
    , mString(_tokenString)
    , mLocation(_streamPos)
    { }

    inline Terminal GetType() const { 
        return mType;
    }

    inline bool IsEOF() const {
        return mType == Terminal::EOFTOKEN;
    }

private:
    Terminal mType;
    std::string mString;
    StreamPosition mLocation;

    friend std::ostream& operator<<(std::ostream& _os, const TokenT& _token)
    {
        return _os << "TokenT(" 
                   << _token.mType << ", " 
                   << _token.mString << ", "
                   << _token.mLocation << ");";
    }
};

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
template <typename Terminal>
class LexerT
{
public:
    // _rules is expected to end with an entry that has a nullptr for a Pattern.
	// The string is the string which is to be lexed, and must not be NULL.
	// The state parameter is a state object. It cannot be specified by type, annoyingly. 
	LexerT(const LexicalEntry<Terminal>* _rules, const char* _stringToLex, void* _state);
	~LexerT();

	// Peek at the next token and return.
	TokenT<Terminal> Peek() const;

	// Pop the next token and return.
	TokenT<Terminal> Pop();

private:
    void ConstructRules(const LexicalEntry<Terminal>* _rules);
    TokenT<Terminal> FindNextToken();

    struct RuleEntry 
    {
        std::regex Regex;
        const Terminal Token;
        typename LexicalEntry<Terminal>::TCheckFunc TokenCallback;    
    };

private:
    std::vector<RuleEntry> mRules;
    std::string mStringToLex;
    std::string::const_iterator mSrcPos;
    StreamPosition mStreamPosition;
	void* mState;
    TokenT<Terminal> mLookaheadToken;

    std::set<std::string> mTypes;
};


// ------------------------------------------------------------------------------------------------
template <typename Terminal>
LexerT<Terminal>::LexerT(const LexicalEntry<Terminal>* _rules, const char* _stringToLex, void* _state)
: mStringToLex(_stringToLex)
, mSrcPos(mStringToLex.cbegin())
, mState(_state)
{
    ConstructRules(_rules);
    mLookaheadToken = FindNextToken();
}

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
LexerT<Terminal>::~LexerT()
{

}

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
TokenT<Terminal> LexerT<Terminal>::Peek() const 
{ 
    return mLookaheadToken; 
}

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
TokenT<Terminal> LexerT<Terminal>::Pop() 
{ 
    TokenT<Terminal> retToken = mLookaheadToken;
    mLookaheadToken = FindNextToken();
    return retToken; 
}

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
void LexerT<Terminal>::ConstructRules(const LexicalEntry<Terminal>* _rules)
{
    for (int i = 0; _rules[i].Pattern; ++i) {
        std::regex re = std::regex(_rules[i].Pattern, std::regex::optimize | std::regex::ECMAScript);
        mRules.push_back({ re, _rules[i].Token, _rules[i].TokenCallback });
    }
}

// ------------------------------------------------------------------------------------------------
template <typename Terminal>
TokenT<Terminal> LexerT<Terminal>::FindNextToken()
{
    TokenT<Terminal> retToken;

    Terminal tokenFound = Terminal::IGNORETOKEN;
    while (tokenFound == Terminal::IGNORETOKEN)
    {
        // Check if we're at the EOF. If so, bail.
        if (mSrcPos == mStringToLex.cend()) {
            retToken = TokenT<Terminal>(Terminal::EOFTOKEN, "\0", mStreamPosition);
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
                Terminal localTok = it->Token;

                // If there's a token callback function, call it and allow it to replace our 
                // default value.
                if (it->TokenCallback) {
                    localTok = it->TokenCallback(matchStr, localTok, mState);
                }

                if (localTok == Terminal::REJECTTOKEN) {
                    continue;
                }

                // Mark this as the found token.
                tokenFound = localTok;

                // Record the token for further processing.
                retToken = TokenT<Terminal>(tokenFound, matchStr, mStreamPosition);

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
            return TokenT<Terminal>();
        }
    }

    return retToken;
}

