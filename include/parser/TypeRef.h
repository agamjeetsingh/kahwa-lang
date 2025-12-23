//
// Created by Agamjeet Singh on 29/11/25.
//

#ifndef TYPEREF_H
#define TYPEREF_H
#include <ranges>
#include <utility>
#include <vector>
#include "ASTBuilder.h"
#include "../types/Variance.h"

struct TypeRef {
    explicit TypeRef(std::string identifier,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange,
    const std::vector<TypeRef*> &args,
    const std::vector<Variance> &variances):
    identifier(std::move(identifier)),
    args(args),
    variances(variances),
    nameSourceRange(nameSourceRange),
    bodyRange(bodyRange) {}

    const std::string identifier;
    const std::vector<TypeRef*> args;
    const std::vector<Variance> variances;
    const SourceRange nameSourceRange;
    const SourceRange bodyRange;

    bool isVariantLess() const {
        return std::ranges::all_of(variances, [](const Variance variance) {
            return variance == Variance::INVARIANT;
        }) && std::ranges::all_of(args, [](const TypeRef* arg) {
            return arg->isVariantLess();
        });
    }

    bool operator==(const TypeRef &other) const {
        if (identifier != other.identifier || args.size() != other.args.size() || variances.size() != other.variances.size()) {
            return false;
        }

        if (variances != other.variances) return false;
        
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == nullptr && other.args[i] == nullptr) continue;
            if (args[i] == nullptr || other.args[i] == nullptr) return false;
            if (*args[i] != *other.args[i]) return false;
        }
        
        return true;
    }

    [[nodiscard]] std::string toString() const {
        std::string str = identifier;
        if (!args.empty()) {
            str += "<";

            for (int i = 0; i < args.size(); i++) {
                str += args[i]->toString();
                if (i != args.size() - 1) {
                    str += ", ";
                }
            }

            str += ">";
        }

        return str;
    }
};

class TypeRefBuilder : public ASTBuilder {
public:
    explicit TypeRefBuilder(std::string  name): name(std::move(name)) {}

    [[nodiscard]] TypeRef* build() const {
        return arena->make<TypeRef>(
            name,
            nameSourceRange.has_value() ? nameSourceRange.value() : dummy_source,
            bodyRange.has_value() ? bodyRange.value() : dummy_source,
            typeRefs,
            variances);
    }

    TypeRefBuilder& with(TypeRef* typeRef, Variance variance = Variance::INVARIANT) {
        typeRefs.push_back(typeRef);
        variances.push_back(variance);
        return *this;
    }

    TypeRefBuilder& with(const std::vector<TypeRef*>& typeRefs) {
        return with(typeRefs, std::vector(typeRefs.size(), Variance::INVARIANT));
    }

    TypeRefBuilder& with(const std::vector<TypeRef*>& typeRefs, const std::vector<Variance>& variances) {
        assert(typeRefs.size() == variances.size());
        this->typeRefs.insert(this->typeRefs.end(), typeRefs.begin(), typeRefs.end());
        this->variances.insert(this->variances.end(), variances.begin(), variances.end());
        return *this;
    }

    TypeRefBuilder& withNameSourceRange(const SourceRange& nameSourceRange) {
        this->nameSourceRange.emplace(nameSourceRange);
        return *this;
    }

    TypeRefBuilder& withBodyRange(const SourceRange& bodyRange) {
        this->bodyRange.emplace(bodyRange);
        return *this;
    }

private:
    std::string name;
    std::vector<TypeRef*> typeRefs;
    std::vector<Variance> variances;
    std::optional<SourceRange> nameSourceRange;
    std::optional<SourceRange> bodyRange;
};



#endif //TYPEREF_H
