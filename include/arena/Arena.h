//
// Created by Agamjeet Singh on 03/12/25.
//

#ifndef ARENA_H
#define ARENA_H
#include <vector>

class Arena {
public:
    explicit Arena(size_t block_size = DEFAULT_BLOCK_SIZE) : block_size(block_size) { addBlock(block_size); }

    ~Arena() { clear(); }

    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));

    template <typename T, typename... Args>
    requires std::constructible_from<T, Args...>
    T* make(Args&&... args) {
        void* mem = allocate(sizeof(T), alignof(T));
        return new (mem) T(std::forward<Args>(args)...);
    }

    void clear() noexcept;

private:
    struct Block {
        char* data;
        std::size_t size;
    };

    std::vector<Block> blocks;
    std::size_t block_size;

    char* current = nullptr;   // bump pointer inside latest block
    size_t remaining = 0;      // bytes left in latest block

    void addBlock(size_t size);

    static uintptr_t align_up(const std::uintptr_t n, const std::size_t alignment) {
        return (n + (alignment - 1)) & ~(alignment - 1);
    }

    static constexpr std::size_t DEFAULT_BLOCK_SIZE = 64 * 1024;
};


#endif //ARENA_H
