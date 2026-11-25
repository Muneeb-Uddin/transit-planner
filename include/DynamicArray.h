#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
#include <iostream>
#include <stdexcept>

using namespace std;

template <typename T>
class DynamicArray {
private:
    T* data;
    int count;
    int capacity;
    void reserve(int newCapacity) {
        if (newCapacity <= capacity) return;
        T* fresh = new T[newCapacity];
        for (int i = 0; i < count; ++i) fresh[i] = data[i];
        delete[] data;
        data = fresh;
        capacity = newCapacity;
    }
public:
    explicit DynamicArray(int initialCapacity = 4)
        : data(0), count(0), capacity(initialCapacity > 0 ? initialCapacity : 4) {
        data = new T[capacity];
    }
    ~DynamicArray() { delete[] data; }
    DynamicArray(const DynamicArray& other)
        : data(new T[other.capacity]), count(other.count), capacity(other.capacity) {
        for (int i = 0; i < count; ++i) data[i] = other.data[i];
    }
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            T* fresh = new T[other.capacity];
            for (int i = 0; i < other.count; ++i) fresh[i] = other.data[i];
            delete[] data;
            data = fresh; count = other.count; capacity = other.capacity;
        }
        return *this;
    }
    void push_back(const T& value) {
        if (count == capacity) reserve(capacity * 2);
        data[count++] = value;
    }
    void pop_back() { if (count == 0) throw out_of_range("DynamicArray is empty"); --count; }
    T& operator[](int index) { if (index < 0 || index >= count) throw out_of_range("Index out of range"); return data[index]; }
    const T& operator[](int index) const { if (index < 0 || index >= count) throw out_of_range("Index out of range"); return data[index]; }
    int size() const { return count; }
    int getCapacity() const { return capacity; }
    bool isEmpty() const { return count == 0; }
    void clear() { count = 0; }
};
#endif
