//
// Created by Agamjeet Singh on 22/12/25.
//

#ifndef SCOPE_H
#define SCOPE_H
#include <unordered_map>
#include <vector>


struct Symbol;
typedef std::unordered_map<std::string, std::vector<Symbol*>> SymbolTable;

struct Scope {
    [[nodiscard]] const std::vector<Symbol*>& searchCurrent(const std::string& name);

    [[nodiscard]] std::optional<Symbol*> searchCurrentUnique(const std::string& name) const;

    [[nodiscard]] std::optional<Symbol*> searchUnique(const std::string& name) const;

    [[nodiscard]] std::vector<Symbol*> search(const std::string& name) const;

    void define(Symbol* symbol);

    template <typename T>
    requires std::derived_from<T, Symbol>
    void defineAll(const std::vector<T*>& symbols);

    void addOuterScope(Scope* outerScope);

    void addOuterScopes(const std::vector<Scope*>& outerScopes);

private:
    SymbolTable table;

    std::vector<Scope*> outerScopes;
};



#endif //SCOPE_H
