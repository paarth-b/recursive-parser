/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */

// Paarth Batra, CSE340 Summer 2024 - ASU


#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum {
    END_OF_FILE = 0,
    PUBLIC, PRIVATE,
    EQUAL, COLON,
    COMMA, SEMICOLON,
    LBRACE, RBRACE,
    ID, ERROR
} TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    int GetSize();
    Token UngetToken(Token token);
    Token CheckHeadToken();
    LexicalAnalyzer(InputBuffer &input);

  private:
    std::vector<Token> tokens;
    Token tmp;
    InputBuffer input;

    void skipWhitespaceAndComments();
    Token createToken(TokenType, const std::string &);
};

#endif  //__LEXER__H__
