// Paarth Batra, CSE340 Summer 2024 - ASU

#include "parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

Parser::Parser(LexicalAnalyzer& lex, SymbolTable& sym) : lexer(lex), symbolTable(sym) {
    // nextToken();
}

void Parser::syntax_error() {
    std::cout << "Syntax Error\n";
    exit(1);
}

void Parser::Parse() {
    parse_program();
}

void Parser::parse_program() {
    // std::cout << "parse_program started" << std::endl;
    parse_global_vars();
    parse_scope();
    // std::cout << "parse_program ended" << std::endl;
}

void Parser::parse_global_vars() { // DONE : No Symbol Table
    // std::cout << "parse_global_vars started" << std::endl;
    Token global = lexer.GetToken();
    if (global.token_type == ID) {
        global = lexer.GetToken();
        if(global.token_type == LBRACE || global.token_type == EQUAL) {
            global = lexer.UngetToken(global);
            global = lexer.UngetToken(global);
            return;
        }
        else if (global.token_type != COMMA && global.token_type != SEMICOLON) {
            syntax_error();
        }
        // global = lexer.UngetToken(global);

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
    // std::cout << "parse_global_vars ended" << std::endl;
}

void Parser::parse_var_list(bool is_pub_list) { // DONE : No Symbol Table
    // std::cout << "parsed_var started" << std::endl;
    
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
    // std::cout << "parsed_var ended" << std::endl;
}

void Parser::parse_scope() {
    // std::cout << "parse_scope started" << std::endl;

    Token id_var = lexer.GetToken();
    symbolTable.enterScope(id_var.lexeme);

    // std::cout << "id_var: " << id_var.lexeme << std::endl;
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
    // std::cout << "this is my var: " << id_var.lexeme << std::endl;
    if (id_var.lexeme != "}") {
        syntax_error();
    }
    symbolTable.exitScope();
}

void Parser::parse_public_vars() {
    Token parse = lexer.GetToken();
    // std::cout << "parse token: " << parse.lexeme << std::endl;
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
        // std::cout << "parse token: " << parse.lexeme << std::endl;
    }
}

void Parser::parse_private_vars() {
    Token parse = lexer.GetToken();
    // std::cout << "parse token: " << parse.lexeme << std::endl;
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
        // std::cout << "parse token: " << parse.lexeme << std::endl;
    }
}

void Parser::parse_stmt_list() {
    Token stmt_list = lexer.GetToken();
    // std::cout << "stmt_list: " << stmt_list.lexeme << std::endl;
    
    if (stmt_list.token_type == RBRACE)
    {
        // std::cout << "rbrace found" << std::endl;
        stmt_list = lexer.UngetToken(stmt_list);
        // std::cout << "stmt_list: " << stmt_list.lexeme << std::endl;
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
        // Only goes back to starting stmt id, not before it
        stmt_list = lexer.UngetToken(stmt_list);
        check_semicolon();

        symbolTable.lookup(stmt_list.lexeme);
        std :: cout << " = ";
        parse_stmt();
        parse_stmt_list();
        // std::cout << "parsed stmt: " << stmt_list.lexeme << std::endl;
    }
    // std::cout << "stmt_list ended" << std::endl;
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
    // std::cout << "parse_stmt started" << std::endl;
    Token stmt = lexer.GetToken();
    if (stmt.token_type != EQUAL) {
        syntax_error();
    }
    stmt = lexer.GetToken();
    symbolTable.lookup(stmt.lexeme);
    std::cout << std::endl;
    // std::cout << "stmt token: " << stmt.lexeme << std::endl;
    if (stmt.token_type != ID) {
        syntax_error();
    }   
    stmt = lexer.GetToken();
    if (stmt.token_type != SEMICOLON) {
        syntax_error();
    }
}

int main(int argc, char *argv[]) {
    // if (argc != 2) {
    //     std::cerr << "Usage: " << argv[0] << " <input-file>" << std::endl;
    //     return 1;
    // }

    // std::ifstream file(argv[1]);
    // if (!file.is_open()) {
    //     std::cerr << "Error: could not open file " << argv[1] << std::endl;
    //     return 1;
    // }

    std::stringstream buffer;
    buffer << std::cin.rdbuf();
    std::string content = buffer.str();
    //file.close();

    InputBuffer inputBuffer;
    inputBuffer.UngetString(content);

    LexicalAnalyzer lexer(inputBuffer);
    SymbolTable symbolTable;

    Parser parser(lexer, symbolTable);
    parser.Parse();

    return 0;
}