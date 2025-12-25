//
// Created by Agamjeet Singh on 22/12/25.
//

#ifndef SCOPE_H
#define SCOPE_H
#include <unordered_map>
#include <vector>

#include "Symbol.h"

typedef std::unordered_map<std::string, std::vector<Symbol*>> SymbolTable;

struct Scope {
    [[nodiscard]] const std::vector<Symbol*>& searchCurrent(const std::string& name) {
        return table[name];
    }

    [[nodiscard]] std::optional<Symbol*> searchCurrentUnique(const std::string& name) {
        const auto& symbols = table[name];
        return symbols.size() == 1 ? std::optional(symbols[0]) : std::nullopt;
    }

    [[nodiscard]] std::optional<Symbol*> searchUnique(const std::string& name) const {
        const auto& symbols = search(name);
        return symbols.size() == 1 ? std::optional(symbols[0]) : std::nullopt;
    }

    [[nodiscard]] std::vector<Symbol*> search(const std::string& name) const {
        if (table.contains(name)) {
            return table.at(name);
        }

        for (auto scope: outerScopes) {
            if (const auto& symbols = scope->search(name); !symbols.empty()) {
                return symbols;
            }
        }

        return {};
    }

    void define(Symbol* symbol) {
        table[symbol->name].push_back(symbol);
    }

    template <typename T>
    requires std::derived_from<T, Symbol>
    void defineAll(const std::vector<T*>& symbols) {
        std::ranges::for_each(symbols, [this](auto* t) {
            table[t->name].push_back(t);
        });
    }

    void addOuterScope(Scope* outerScope) {
        outerScopes.push_back(outerScope);
    }

    void addOuterScopes(const std::vector<Scope*>& outerScopes) {
        this->outerScopes.insert(this->outerScopes.begin(), outerScopes.begin(), outerScopes.end());
    }
private:
    SymbolTable table;

    std::vector<Scope*> outerScopes;
};



#endif //SCOPE_H
