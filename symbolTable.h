// Paarth Batra, CSE340 Summer 2024 - ASU

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

#ifndef __SYMBOL_TABLE__H__
#define __SYMBOL_TABLE__H__
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

class SymbolTable {
private:
    struct Scope {
        std::string scopeName;
        std::vector<std::pair<std::string, bool>> symbols;
    };
    Scope globalScope = { "::", {} };
    std::vector<Scope> scopeStack;

public:
    SymbolTable() {
        scopeStack.push_back(globalScope);
    }

    void enterScope(const std::string& scope);
    void exitScope();
    void addSymbol(const std::string& scope, const std::string& id, bool isPublic);
    void lookup(const std::string& id);

    friend class Parser;
};

#endif // __SYMBOL_TABLE__H__