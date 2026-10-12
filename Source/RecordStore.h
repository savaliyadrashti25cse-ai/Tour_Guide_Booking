#ifndef RECORD_STORE_H
#define RECORD_STORE_H

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

// A small dynamic array. The project intentionally demonstrates new[]/delete[].
// Copying is disabled so two stores cannot accidentally own the same allocation.
template <class T>
class RecordStore {
    T* items;
    std::size_t count;
    std::size_t capacity;
public:
    explicit RecordStore(std::size_t initialCapacity = 4)
        : items(new T[initialCapacity ? initialCapacity : 4]), count(0),
          capacity(initialCapacity ? initialCapacity : 4) {}
    ~RecordStore() { delete[] items; }
    RecordStore(const RecordStore&) = delete;
    RecordStore& operator=(const RecordStore&) = delete;
    RecordStore(RecordStore&& other) noexcept : items(nullptr), count(0), capacity(0) {
        swap(other);
    }
    RecordStore& operator=(RecordStore&& other) noexcept {
        if (this != &other) { RecordStore moved(std::move(other)); swap(moved); }
        return *this;
    }
    void swap(RecordStore& other) noexcept {
        std::swap(items, other.items); std::swap(count, other.count);
        std::swap(capacity, other.capacity);
    }
    std::size_t size() const { return count; }
    std::size_t allocatedCapacity() const { return capacity; }
    T& operator[](std::size_t index) {
        if (index >= count) throw std::out_of_range("Record index out of range.");
        return items[index];
    }
    const T& operator[](std::size_t index) const {
        if (index >= count) throw std::out_of_range("Record index out of range.");
        return items[index];
    }
    T* find(int id) {
        for (std::size_t i = 0; i < count; ++i)
            if (items[i].getId() == id) return &items[i];
        return nullptr;
    }
    const T* find(int id) const {
        for (std::size_t i = 0; i < count; ++i)
            if (items[i].getId() == id) return &items[i];
        return nullptr;
    }
    void add(const T& record) {
        if (find(record.getId())) throw std::runtime_error("Duplicate record ID.");
        if (count == capacity) {
            const std::size_t newCapacity = capacity ? capacity * 2 : 4;
            std::unique_ptr<T[]> expanded(new T[newCapacity]);
            for (std::size_t i = 0; i < count; ++i) expanded[i] = items[i];
            delete[] items; items = expanded.release(); capacity = newCapacity;
        }
        items[count] = record;
        ++count;
    }
    bool remove(int id) {
        for (std::size_t i = 0; i < count; ++i) {
            if (items[i].getId() != id) continue;
            for (std::size_t j = i; j + 1 < count; ++j) items[j] = items[j + 1];
            items[--count] = T();
            return true;
        }
        return false;
    }
};
#endif
