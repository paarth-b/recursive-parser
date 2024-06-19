// Paarth Batra, CSE340 Summer 2024 - ASU

#include "symbolTable.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <map>

void SymbolTable::enterScope(const std::string& scope) {
    scopeStack.push_back({ scope, {} });
}

void SymbolTable::exitScope() {
    if (scopeStack.size() > 1) {
        scopeStack.pop_back();
    } else {
        std::cerr << "Error: Cannot exit global scope." << std::endl;
    }
}

void SymbolTable::addSymbol(const std::string& scope, const std::string& id, bool isPublic) {
    // printf("Adding symbol %s to scope %s\n", id.c_str(), scope.c_str());
    scopeStack.back().symbols.push_back({id, isPublic});
}

void SymbolTable::lookup(const std::string& id) {
    for (int i = scopeStack.size()-1; i > 0; --i) {
        auto scope = scopeStack[i];
        for (const auto& symbol : scope.symbols) {
            if (symbol.first == id) {
                if (symbol.second == true || i == scopeStack.size() - 1) {
                    std::cout << scope.scopeName << "." << id;
                    return;
                }
            }
        }
    }
    // if (scopeStack[0].symbols.size() == 0) {
    //     std::cout << "Syntax Error" << std::endl;
    //     exit(0);
    // }
    for (const auto& symbol : scopeStack[0].symbols) {
        if (symbol.first == id) {
            std::cout << "::" << id;
            return;
        }
        }
    std::cout << "?." << id;
}