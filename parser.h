// Paarth Batra, CSE340 Summer 2024 - ASU

#ifndef __PARSER__H__
#define __PARSER__H__

#include "lexer.h"
#include "symbolTable.h"
#include <string>
#include <vector>

class Parser {
public:
    Parser(LexicalAnalyzer& lexer, SymbolTable& symbolTable);
    void Parse();

private:
    LexicalAnalyzer& lexer;
    SymbolTable& symbolTable;

    Token token;

    void syntax_error();
    void parse_program();
    void parse_global_vars();
    void parse_var_list(bool is_pub_list);
    void parse_scope();
    void parse_public_vars();
    void parse_private_vars();
    void parse_stmt_list();
    void parse_stmt();
    void check_semicolon();
};

#endif
