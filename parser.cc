// Paarth Batra, CSE340 Summer 2024 - ASU

#include "parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

Parser::Parser(LexicalAnalyzer& lex, SymbolTable& sym) : lexer(lex), symbolTable(sym) {}

void Parser::syntax_error() {
    std::cout << "Syntax Error\n";
    exit(1);
}

void Parser::Parse() {
    parse_program();
    symbolTable.printTable();
}

void Parser::parse_program() {
    parse_global_vars();
    parse_scope();
}

void Parser::parse_global_vars() {
    Token global = lexer.GetToken();
    if (global.token_type == ID) {
        global = lexer.GetToken();
        if(global.token_type == LBRACE || global.token_type == EQUAL) {
            global = lexer.UngetToken(global);
            symbolTable.enterScope(lexer.CheckHeadToken().lexeme);
            return;
        }
        else if (global.token_type != COMMA && global.token_type != SEMICOLON) {
            syntax_error();
        }

        global = lexer.UngetToken(global);
        symbolTable.addSymbol(symbolTable.scopeStack.back().scopeName, global.lexeme, true);
        global = lexer.GetToken();
        if(global.token_type == SEMICOLON) {} 
        else if (global.token_type == COMMA) {
            parse_var_list(true);
            global = lexer.GetToken();
        }
        if (global.token_type != SEMICOLON) {
            syntax_error();
        }
    }
}

void Parser::parse_var_list(bool is_pub_list) {
    Token var_list = lexer.GetToken();
    var_list = lexer.GetToken();
    if(var_list.token_type == LBRACE || var_list.token_type == EQUAL) {
        var_list = lexer.UngetToken(var_list);
        lexer.UngetToken(var_list);
        return;
    }
    var_list = lexer.UngetToken(var_list);
    symbolTable.addSymbol(symbolTable.scopeStack.back().scopeName, var_list.lexeme, is_pub_list);

    if (var_list.token_type != ID) {
        syntax_error();
    }
    var_list = lexer.GetToken();
    if (var_list.token_type == COMMA) parse_var_list(is_pub_list);
    else var_list = lexer.UngetToken(var_list);
}

void Parser::parse_scope() {
    Token id_var;
    if (lexer.GetSize() == 1){
        id_var = lexer.CheckHeadToken();
    }
    else {
        id_var = lexer.GetToken();
        symbolTable.enterScope(id_var.lexeme);
    }

    if (id_var.token_type != ID) {
        syntax_error();
    }
    id_var = lexer.GetToken();
    if (id_var.token_type != LBRACE) {
        syntax_error();
    }
    parse_public_vars();
    parse_private_vars();
    parse_stmt_list();
    id_var = lexer.GetToken();
    if (id_var.lexeme != "}") {
        syntax_error();
    }
    symbolTable.exitScope();
}

void Parser::parse_public_vars() {
    Token parse = lexer.GetToken();
    if (parse.token_type == PUBLIC) {
        if (lexer.GetToken().token_type != COLON) {
            syntax_error();
        }
        parse_var_list(true);
        if (lexer.GetToken().token_type != SEMICOLON) {
            syntax_error();
        }
    } else {
        parse = lexer.UngetToken(parse);
    }
}

void Parser::parse_private_vars() {
    Token parse = lexer.GetToken();
    if (parse.token_type == PRIVATE) {
        if (lexer.GetToken().token_type != COLON) {
            syntax_error();
        }
        parse_var_list(false);
        if (lexer.GetToken().token_type != SEMICOLON) {
            syntax_error();
        }
    } else {
        parse = lexer.UngetToken(parse);
    }
}

void Parser::parse_stmt_list() {
    Token stmt_list = lexer.GetToken();
    
    if (stmt_list.token_type == RBRACE)
    {
        stmt_list = lexer.UngetToken(stmt_list);
        return;
    } else if (stmt_list.token_type != ID) {
        syntax_error();
    }

    stmt_list = lexer.GetToken();
    if (stmt_list.token_type == LBRACE) {
        stmt_list = lexer.UngetToken(stmt_list);
        stmt_list = lexer.UngetToken(stmt_list);
        parse_scope();
        parse_stmt_list();
    } else {
        stmt_list = lexer.UngetToken(stmt_list);
        check_semicolon();

        symbolTable.lookup(stmt_list.lexeme);
        parse_stmt();
        parse_stmt_list();
    }
}

void Parser::check_semicolon() { //edge case
    Token stmt = lexer.GetToken();
    stmt = lexer.GetToken();
    stmt = lexer.GetToken();
    if(stmt.token_type != SEMICOLON) {
        syntax_error();
    }
    stmt = lexer.UngetToken(stmt);
    stmt = lexer.UngetToken(stmt);
    stmt = lexer.UngetToken(stmt);
}

void Parser::parse_stmt() {
    Token stmt = lexer.GetToken();
    if (stmt.token_type != EQUAL) {
        syntax_error();
    }
    stmt = lexer.GetToken();
    symbolTable.lookup(stmt.lexeme);
    if (stmt.token_type != ID) {
        syntax_error();
    }   
    stmt = lexer.GetToken();
    if (stmt.token_type != SEMICOLON) {
        syntax_error();
    }
}

int main(int argc, char *argv[]) {
    std::stringstream buffer;
    buffer << std::cin.rdbuf();
    std::string content = buffer.str();

    InputBuffer inputBuffer;
    inputBuffer.UngetString(content);

    LexicalAnalyzer lexer(inputBuffer);
    SymbolTable symbolTable;

    Parser parser(lexer, symbolTable);
    parser.Parse();

    return 0;
}