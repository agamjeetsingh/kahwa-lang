//
// Created by Agamjeet Singh on 22/12/25.
//

#ifndef SCOPE_H
#define SCOPE_H
#include <unordered_map>
#include <vector>

#include "Symbol.h"

typedef std::unordered_map<std::string, Symbol*> SymbolTable;

struct Scope {
    SymbolTable table;

    std::vector<Scope*> outerScopes;

    Symbol* search(const std::string& name) const {
        if (table.contains(name)) {
            return table.at(name);
        }

        for (auto scope: outerScopes) {
            if (auto* symbol = scope->search(name)) {
                return symbol;
            }
        }

        return nullptr;
    }
};



#endif //SCOPE_H
