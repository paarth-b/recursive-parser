// Paarth Batra, CSE340 Summer 2024 - ASU

#include "lexer.h"
#include <cctype>
#include <iostream>

LexicalAnalyzer::LexicalAnalyzer(InputBuffer &inputBuffer) : input(inputBuffer) {
    tmp.lexeme = "";
    tmp.token_type = ERROR;
}


void LexicalAnalyzer::skipWhitespaceAndComments() {
    char currentChar;
    while (!input.EndOfInput()) {
        input.GetChar(currentChar);

        if (isspace(currentChar)) {
            continue;
        } else if (currentChar == '/') {
            char nextChar;
            input.GetChar(nextChar);
            if (nextChar == '/') {
                while (currentChar != '\n') {
                    input.GetChar(currentChar);
                }
            } else {
                input.UngetChar(nextChar);
                input.UngetChar(currentChar);
                break;
            }
        } else {
            input.UngetChar(currentChar);
            break;
        }
    }
}

Token LexicalAnalyzer::createToken(TokenType type, const std::string &lexeme) {
    Token tkn;
    tkn.token_type = type;
    tkn.lexeme = lexeme;
    tokens.push_back(tkn);
    return tkn;
}

Token LexicalAnalyzer::GetToken() {
    skipWhitespaceAndComments();

    char currentChar;
    if (input.EndOfInput()) {
        return createToken(END_OF_FILE, "");
    }

    input.GetChar(currentChar);
    printf("currentChar: %c\n", currentChar);

    if (isalpha(currentChar)) {
        std::string lexeme;
        while (isalnum(currentChar)) {
            lexeme += currentChar;
            input.GetChar(currentChar);
        }
        input.UngetChar(currentChar);
        if (lexeme == "public") return createToken(PUBLIC, lexeme);
        if (lexeme == "private") return createToken(PRIVATE, lexeme);
        return createToken(ID, lexeme);
    }

    switch (currentChar) {
        case '=': return createToken(EQUAL, "=");
        case ':': return createToken(COLON, ":");
        case ',': return createToken(COMMA, ",");
        case '{': return createToken(LBRACE, "{");
        case '}': return createToken(RBRACE, "}");
        case ';': return createToken(SEMICOLON, ";");
        default : return createToken(ERROR, std::string(1, currentChar));
    }
}

Token LexicalAnalyzer::UngetToken(Token token) {
    if (!tokens.empty()) {
        tokens.pop_back();
        for (int i = token.lexeme.size() - 1; i >= 0; --i) {
            input.UngetChar(token.lexeme[i]);
        }
    }
    token = tokens.back();
    return token;
}