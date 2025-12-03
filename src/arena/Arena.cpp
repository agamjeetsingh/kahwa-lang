//
// Created by Agamjeet Singh on 03/12/25.
//

#include "../../include/arena/Arena.h"

void *Arena::allocate(const size_t size, const size_t alignment) {
    auto cur = reinterpret_cast<std::uintptr_t>(current);
    uintptr_t aligned = align_up(cur, alignment);
    size_t padding = aligned - cur;

    if (padding + size <= remaining) {
        auto ptr = reinterpret_cast<char*>(aligned);
        current = ptr + size;
        remaining -= padding + size;
        return ptr;
    }

    addBlock(std::max(block_size, size + alignment));

    cur = reinterpret_cast<uintptr_t>(current);
    aligned = align_up(cur, alignment);
    padding = aligned - cur;

    const auto ptr = reinterpret_cast<char*>(aligned);
    current = ptr + size;
    remaining -= padding + size;

    return ptr;
}

void Arena::clear() noexcept {
    for (auto& [data, size] : blocks) {
        std::free(data);
    }
    blocks.clear();

    current = nullptr;
    remaining = 0;

    addBlock(block_size);
}

void Arena::addBlock(size_t size) {
    size = align_up(size, alignof(std::max_align_t));
    const auto data = static_cast<char*>(std::malloc(size));
    if (!data) throw std::bad_alloc();

    blocks.push_back({ data, size });

    current = data;
    remaining = size;
}

