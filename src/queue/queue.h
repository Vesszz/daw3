#pragma once

#include <atomic>
#include <cstddef>

/**
* @brief Lock free queue, SPSC
*/
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

        /**
        * @param item перемещение T
        * @return bool: true если получилось пушнуть, false если не получилось 
        */
        bool try_push(T&& item) noexcept;
        /**
        * @param item перемещение T
        */
        void push(T&& item) noexcept;
        /**
        * @param item ссылка куда класть
        * @return в передаваемый объект кладёт значение
        */
        bool try_pop(T& item) noexcept;
        /**
        * @return объект по значению
        */
        T pop() noexcept;
        bool empty() const noexcept;
        bool full() const noexcept;
        size_t size() const noexcept;
        constexpr size_t capacity() const noexcept { return Capacity - 1; }
};
