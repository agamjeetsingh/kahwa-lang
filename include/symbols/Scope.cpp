//
// Created by Agamjeet Singh on 22/12/25.
//

#include "Scope.h"

#include "Symbol.h"

const std::vector<Symbol *> &Scope::searchCurrent(const std::string &name) {
    return table[name];
}

std::optional<Symbol *> Scope::searchCurrentUnique(const std::string &name) {
    const auto& symbols = table[name];
    return symbols.size() == 1 ? std::optional(symbols[0]) : std::nullopt;
}

std::optional<Symbol *> Scope::searchUnique(const std::string &name) const {
    const auto& symbols = search(name);
    return symbols.size() == 1 ? std::optional(symbols[0]) : std::nullopt;
}

std::vector<Symbol *> Scope::search(const std::string &name) const {
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

void Scope::define(Symbol *symbol) {
    table[symbol->name].push_back(symbol);
}


void Scope::addOuterScope(Scope *outerScope) {
    outerScopes.push_back(outerScope);
}

template<typename T> requires std::derived_from<T, Symbol>
void Scope::defineAll(const std::vector<T *> &symbols) {
    std::ranges::for_each(symbols, [this](auto* t) {
        table[t->name].push_back(t);
    });
}

void Scope::addOuterScopes(const std::vector<Scope *> &outerScopes) {
    this->outerScopes.insert(this->outerScopes.begin(), outerScopes.begin(), outerScopes.end());
}
