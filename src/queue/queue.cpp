#include <thread>
#include "queue.h"
#include "midi_command.h"

template<typename T, size_t Capacity>
Queue<T, Capacity>::Queue() {
    head.value.store(0, std::memory_order_relaxed);
    tail.value.store(0, std::memory_order_relaxed);
}

template<typename T, size_t Capacity>
bool Queue<T, Capacity>::try_push(T&& item) noexcept {
    size_t current_tail = tail.value.load(std::memory_order_relaxed);
    size_t next_tail = (current_tail + 1) % Capacity;
    
    if (next_tail == head.value.load(std::memory_order_acquire)) {
        return false;
    }
    
    buffer[current_tail] = std::move(item);
    tail.value.store(next_tail, std::memory_order_release);
    return true;
}

template<typename T, size_t Capacity>
void Queue<T, Capacity>::push(T&& item) noexcept {
    while (!try_push(std::move(item))) {
        std::this_thread::yield();
    }
}

template<typename T, size_t Capacity>
bool Queue<T, Capacity>::try_pop(T& item) noexcept {
    size_t current_head = head.value.load(std::memory_order_relaxed);
    
    if (current_head == tail.value.load(std::memory_order_acquire)) {
        return false;
    }
    
    item = std::move(buffer[current_head]);
    buffer[current_head] = T{};
    head.value.store((current_head + 1) % Capacity, std::memory_order_release);
    return true;
}

template<typename T, size_t Capacity>
T Queue<T, Capacity>::pop() noexcept {
    T item;
    while (!try_pop(item)) {
        std::this_thread::yield();
    }
    return item;
}

template<typename T, size_t Capacity>
bool Queue<T, Capacity>::empty() const noexcept {
    return head.value.load(std::memory_order_acquire) == tail.value.load(std::memory_order_acquire);
}

template<typename T, size_t Capacity>
bool Queue<T, Capacity>::full() const noexcept {
    size_t next_tail = (tail.value.load(std::memory_order_relaxed) + 1) % Capacity;
    return next_tail == head.value.load(std::memory_order_acquire);
}

template<typename T, size_t Capacity>
size_t Queue<T, Capacity>::size() const noexcept {
    size_t h = head.value.load(std::memory_order_acquire);
    size_t t = tail.value.load(std::memory_order_acquire);
    
    if (t >= h) {
        return t - h;
    } else {
        return (Capacity - h) + t;
    }
}

template class Queue<int, 1024>;
template class Queue<float, 1024>;
template class Queue<double, 1024>;
template class Queue<MidiCommand, 1024>;
