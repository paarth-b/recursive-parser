#include "parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

// SYMBOL TABLE
/*
    Scope Vector:
        default - "::" (global)
        size - 1 (global)
    Symbol props:
    |----|------------|
    | id | visibility |
    |----|------------|
    
    Symbol Table Enter(string scope): - call after lbrace
        push onto vector [scope name]
    Symbol Table Exit():
        pop from vector [scope name] - call after rbrace
    Symbol Table Lookup():
        in current scope, lookup by ID
            if found, cout "scope" + . + "id"
        else if not found, iterate -1 in scope (while size > 1)
            lookup ID in current scope
            if visibility is private, continue
            else if visibility is public, cout "scope" + . + "id"
            [RECURSIVE till global]
        else if scope is global (size = 1), 
            if ID find in global,
                cout "scope" + "id"
        else cout "?" + . + "id"
     
    Ex: 
    init symbolTable (global default)
    global (a, public); (b, public); (c, public)
        enter scope TEST
        test (a, public); (b, public); (hello, public); (x, private); (y, private)
     enter statement_list
        get an ID -> Lookup()
        get EQUAL -> cout " = "
        get ID -> Lookup()
        get SemiColon (exit statement)
*/

class SymbolTable {
private:
    struct SymbolProps {
        std::string id;
        bool isPublic;
    };

    std::vector<std::string> scopeStack;
    std::unordered_map<std::string, std::unordered_map<std::string, SymbolProps>> table;

public:
    SymbolTable() {
        scopeStack.push_back("::"); // global scope
    }

    void enterScope(const std::string& scope) {
        scopeStack.push_back(scope);
    }

    void exitScope() {
        if (scopeStack.size() > 1) {
            scopeStack.pop_back();
        } else {
            std::cerr << "Error: Cannot exit global scope." << std::endl;
        }
    }

    void addSymbol(const std::string& scope, const std::string& id, bool isPublic) {
        table[scope][id] = { id, isPublic };
    }

    void lookup(const std::string& id) {
        for (int i = scopeStack.size() - 1; i >= 0; --i) {
            const std::string& scope = scopeStack[i];
            if (table[scope].find(id) != table[scope].end()) {
                // Found in current scope or is public
                if (table[scope][id].isPublic || i == scopeStack.size() - 1) {
                    std::cout << scope << "." << id << std::endl;
                    return;
                }
            }
        }
        if (table["::"].find(id) != table["::"].end()) {
            std::cout << "::" << id << std::endl;
        } else {
            std::cout << "?." << id << std::endl;
        }
    }

    void printSymbolTable() {
        for (const auto& scope : table) {
            std::cout << "Scope: " << scope.first << std::endl;
            for (const auto& symbol : scope.second) {
                std::cout << "  ID: " << symbol.second.id 
                          << " Visibility: " << (symbol.second.isPublic ? "Public" : "Private") 
                          << std::endl;
            }
        }
    }

    friend class Parser;
};

Parser::Parser(LexicalAnalyzer& lex) : lexer(lex) {}

void Parser::syntax_error() {
    std::cout << "Syntax Error\n";
    exit(1);
}

void Parser::Parse() {
    SymbolTable symbolTable;
    printf("symbolTable first scope: %s\n", symbolTable.scopeStack[0].c_str());
    parse_program();
    symbolTable.printSymbolTable();
}

void Parser::parse_program() {
    std::cout << "parse_program started" << std::endl;
    parse_global_vars();
    parse_scope();
    std::cout << "parse_program ended" << std::endl;
}

void Parser::parse_global_vars() { // DONE : No Symbol Table
    std::cout << "parse_global_vars started" << std::endl;
    Token global = lexer.GetToken();
    if (global.token_type == ID) {
        global = lexer.GetToken();
        if(global.token_type == LBRACE || global.token_type == EQUAL) {
            global = lexer.UngetToken(global);
            lexer.UngetToken(global);
            std::cout << "entered scope at: " << global.lexeme << "\n";
            return;
        }

        global = lexer.UngetToken(global);
        std::cout << "global: " << global.lexeme << std::endl;
        global = lexer.GetToken();
        printf("global: %s\n", global.lexeme.c_str());
        parse_var_list();
        global = lexer.GetToken();
        if (global.token_type != SEMICOLON) {
            syntax_error();
        }
    }
    std::cout << "parse_global_vars ended" << std::endl;
}

void Parser::parse_var_list() { // DONE : No Symbol Table
    std::cout << "parsed_var started" << std::endl;
    Token var_list = lexer.GetToken();
    printf("var_list: %s\n", var_list.lexeme.c_str());
    if (var_list.token_type != ID) {
        syntax_error();
    }
    var_list = lexer.GetToken();
    if (var_list.token_type == COMMA) parse_var_list();
    else {
        var_list = lexer.UngetToken(var_list);
        std::cout << "scope found at: " << var_list.lexeme << std::endl;
    }
    std::cout << "parsed_var ended" << std::endl;
}

void Parser::parse_scope() {
    std::cout << "parse_scope started" << std::endl;
    Token id_var = lexer.GetToken();
    std::cout << "id_var: " << id_var.lexeme << std::endl;
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
    std::cout << "this is my var: " << id_var.lexeme << std::endl;
    if (id_var.lexeme != "}") {
        syntax_error();
    }
}

void Parser::parse_public_vars() {
    Token parse = lexer.GetToken();
    std::cout << "parse token: " << parse.lexeme << std::endl;
    if (parse.token_type == PUBLIC) {
        if (lexer.GetToken().token_type != COLON) {
            syntax_error();
        }
        parse_var_list();
        if (lexer.GetToken().token_type != SEMICOLON) {
            syntax_error();
        }
    } else {
        parse = lexer.UngetToken(parse);
        std::cout << "parse token: " << parse.lexeme << std::endl;
    }
}

void Parser::parse_private_vars() {
    Token parse = lexer.GetToken();
    std::cout << "parse token: " << parse.lexeme << std::endl;
    if (parse.token_type == PRIVATE) {
        if (lexer.GetToken().token_type != COLON) {
            syntax_error();
        }
        parse_var_list();
        if (lexer.GetToken().token_type != SEMICOLON) {
            syntax_error();
        }
    } else {
        parse = lexer.UngetToken(parse);
        std::cout << "parse token: " << parse.lexeme << std::endl;
    }
}

void Parser::parse_stmt_list() {
    Token stmt_list = lexer.GetToken();
    std::cout << "stmt_list: " << stmt_list.lexeme << std::endl;
    
    if (stmt_list.token_type == RBRACE)
    {
        std::cout << "rbrace found" << std::endl;
        stmt_list = lexer.UngetToken(stmt_list);
        std::cout << "stmt_list: " << stmt_list.lexeme << std::endl;
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
        parse_stmt();
        parse_stmt_list();
        std::cout << "parsed stmt: " << stmt_list.lexeme << std::endl;
    }
    std::cout << "stmt_list ended" << std::endl;
}

void Parser::parse_stmt() {
    std::cout << "parse_stmt started" << std::endl;
    Token stmt = lexer.GetToken();
    if (stmt.token_type != EQUAL) {
        syntax_error();
    }
    stmt = lexer.GetToken();
    std::cout << "stmt token: " << stmt.lexeme << std::endl;
    if (stmt.token_type != ID) {
        syntax_error();
    }   
    stmt = lexer.GetToken();
    if (stmt.token_type != SEMICOLON) {
        syntax_error();
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    InputBuffer inputBuffer;
    inputBuffer.UngetString(content);

    LexicalAnalyzer lexer(inputBuffer);
    Parser parser(lexer);
    parser.Parse();

    return 0;
}