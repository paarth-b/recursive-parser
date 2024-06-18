#ifndef __PARSER__H__
#define __PARSER__H__

#include "lexer.h"
#include <string>
#include <vector>

class Parser {
public:
    Parser(LexicalAnalyzer& lexer);
    void Parse();

private:
    LexicalAnalyzer& lexer;

    Token token;

    void syntax_error();
    void parse_program();
    void parse_global_vars();
    void parse_var_list();
    void parse_scope();
    void parse_public_vars();
    void parse_private_vars();
    void parse_stmt_list();
    void parse_stmt();

    void add_scope(const std::string& scope);
};

#endif
