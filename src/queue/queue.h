#pragma once

#include <atomic>
#include <cstddef>

template<typename T, size_t Capacity>
class Queue {
    private:
        struct PaddedAtomic {
            std::atomic<size_t> value{0};
            char padding[64 - sizeof(std::atomic<size_t>)];
        };
        
        PaddedAtomic head;
        PaddedAtomic tail;
        T buffer[Capacity];
    
    public:
        Queue();
        ~Queue() = default;
        
        Queue(const Queue&) = delete;
        Queue& operator=(const Queue&) = delete;
        
        bool try_push(T&& item) noexcept;
        void push(T&& item) noexcept;
        bool try_pop(T& item) noexcept;
        T pop() noexcept;
        bool empty() const noexcept;
        bool full() const noexcept;
        size_t size() const noexcept;
        constexpr size_t capacity() const noexcept { return Capacity - 1; }
};
