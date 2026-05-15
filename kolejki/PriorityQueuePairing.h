#pragma once
#include <iostream>
#include <stdexcept>
#include "Element.h"

struct PairingNode {
    int value;
    int priority;
    PairingNode* child;
    PairingNode* sibling;
    PairingNode* prev; // WskaŸnik na rodzica LUB lewe rodzeñstwo

    PairingNode(int v, int p)
        : value(v), priority(p), child(nullptr), sibling(nullptr), prev(nullptr) {
    }
};

class PairingHeapPQ {
private:
    

    PairingNode* root;
    int size;

    PairingNode* merge(PairingNode* h1, PairingNode* h2);
    PairingNode* combineSiblings(PairingNode* firstSibling);
    void clear(PairingNode* node);
    PairingNode* findNode(PairingNode* startNode, int value) const;

public:

    PairingHeapPQ();
    ~PairingHeapPQ();

    // Dodanie elementu - O(1)
    void insert(int value, int priority);
    Element extractMax();
    Element findMax() const;
    void modifyKey(int value, int newPriority);
    void increaseKey(int value, int newPriority);
    void decreaseKey(int value, int newPriority);
    int returnSize() const;
    bool isEmpty() const;
    void print() const;
};