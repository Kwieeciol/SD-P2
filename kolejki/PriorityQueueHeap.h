#pragma once
#include <vector>
#include <stdexcept>

struct Element {
    int value;
    int priority;
};

class BinaryHeapPQ {
private:
    std::vector<Element> heap;

    int parent(int i) const;
    int leftChild(int i) const;
    int rightChild(int i) const;
    void siftUp(int i);
    void siftDown(int i);

public:
    BinaryHeapPQ() = default;

    void insert(int value, int priority);
    Element extractMax();
    Element findMax() const;
    void modifyKey(int value, int newPriority);
    void increaseKey(int index, int newPriority);
    void decreaseKey(int index, int newPriority);
    int returnSize() const;
    bool isEmpty() const;
    void print() const;
};
