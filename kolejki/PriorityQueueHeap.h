#pragma once
#include <iostream>
#include <stdexcept>
#include "Element.h"

class HeapArray {
private:
    Element* data;
    int      sz;
    int      cap;

    void grow();

public:
    HeapArray();
    ~HeapArray();

    HeapArray(const HeapArray&)            = delete;
    HeapArray& operator=(const HeapArray&) = delete;

    void push_back(const Element& e);
    void pop_back();

    Element&       operator[](int i);
    const Element& operator[](int i) const;

    Element&       back();
    const Element& back() const;

    void swap_at(int i, int j);

    int  size()  const;
    bool empty() const;
};

class BinaryHeapPQ {
private:
    HeapArray heap;

    int parent(int i)     const;
    int leftChild(int i)  const;
    int rightChild(int i) const;

    void siftUp(int i);
    void siftDown(int i);

public:
    BinaryHeapPQ() = default;

    void    insert(int value, int priority);
    Element extractMax();
    Element findMax() const;
    void    modifyKey(int value, int newPriority);
    void    increaseKey(int index, int newPriority);
    void    decreaseKey(int index, int newPriority);
    int     returnSize() const;
    bool    isEmpty() const;
    void    print() const;
};