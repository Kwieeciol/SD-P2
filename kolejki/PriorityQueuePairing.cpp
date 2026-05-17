#include "PriorityQueuePairing.h"

PairingHeapPQ::PairingHeapPQ() : root(nullptr), size(0) {}

PairingHeapPQ::~PairingHeapPQ() {
    clear(root);
}

PairingNode* PairingHeapPQ::merge(PairingNode* h1, PairingNode* h2) {
    if (!h1) return h2;
    if (!h2) return h1;
    // gwarancja najwiekszego priorytetu dla h1
    if (h1->priority < h2->priority) {
        PairingNode* temp = h1;
        h1 = h2;
        h2 = temp;
    }

    h2->sibling = h1->child;
    if (h1->child) {
        h1->child->prev = h2;
    }
    h2->prev = h1;
    h1->child = h2;

    return h1;
}

PairingNode* PairingHeapPQ::combineSiblings(PairingNode* firstSibling) {
    if (firstSibling == nullptr || firstSibling->sibling == nullptr) {
        return firstSibling;
    }

    PairingNode* head = nullptr;
    PairingNode* tail = nullptr;
    PairingNode* curr = firstSibling;
    // łączenie w pary od lewej do prawej
    while (curr != nullptr) {
        PairingNode* a = curr;
        PairingNode* b = a->sibling;

        if (b != nullptr) {
            PairingNode* next = b->sibling;
            a->sibling = b->sibling = a->prev = b->prev = nullptr;
            PairingNode* paired = merge(a, b);
            // dodanie nowej pary na koniec tymczasowej listy
            if (head == nullptr) head = paired;
            else tail->sibling = paired;
            tail = paired;

            curr = next;
        }
        else { // ostatni pojedynczy element bez pary
            a->sibling = a->prev = nullptr;
            if (head == nullptr) head = a;
            else tail->sibling = a;
            curr = nullptr;
        }
    }
    // odwracanie listy
    PairingNode* prevNode = nullptr;
    curr = head;
    while (curr != nullptr) {
        PairingNode* next = curr->sibling;
        curr->sibling = prevNode;
        prevNode = curr;
        curr = next;
    }
    // łączenie wszystkiego w jeden kopiec
    PairingNode* result = prevNode;
    curr = prevNode->sibling;
    while (curr != nullptr) {
        PairingNode* next = curr->sibling;
        result->sibling = result->prev = nullptr;
        curr->sibling = curr->prev = nullptr;

        result = merge(result, curr);
        curr = next;
    }

    return result;
}
// wstawienie nowego elementu
PairingNode* PairingHeapPQ::insert(int value, int priority) {
    PairingNode* newNode = new PairingNode(value, priority);
    root = merge(root, newNode);
    size++;
    return newNode;
}
// zwracanie elementu z najwiekszym priorytetem
Element PairingHeapPQ::findMax() const {
    if (isEmpty()) throw std::runtime_error("Heap is empty");
    return { root->value, root->priority };
}
// usuwanie i zwracanie elementu z najwiekszym priorytetem 
Element PairingHeapPQ::extractMax() {
    if (isEmpty()) throw std::runtime_error("Heap is empty");

    Element maxElement = { root->value, root->priority };
    PairingNode* oldRoot = root;
    // łączenie dzieci usuwanego korzenia w nowy kopiec
    root = combineSiblings(root->child);
    if (root) root->prev = nullptr;

    delete oldRoot;
    size--;
    return maxElement;
}
// pomocnicza metoda odpinająca węzeł od jego rodzeństwa i rodzica
void PairingHeapPQ::detachNode(PairingNode* node) {
    if (!node || node == root) return;

    if (node->sibling != nullptr) {
        node->sibling->prev = node->prev;
    }
    if (node->prev->child == node) {
        node->prev->child = node->sibling;
    }
    else {
        node->prev->sibling = node->sibling;
    }

    node->sibling = nullptr;
    node->prev = nullptr;
}
// zwiekszenie priorytetu elementu
void PairingHeapPQ::increaseKey(int value, int newPriority) {
    PairingNode* node = findNode(root, value);
    if (!node) return;
    if (newPriority < node->priority) return;

    node->priority = newPriority;
    if (node == root) return;
    // odpinanie wezła od obecnego rodzica/rodzenstwa
    if (node->sibling) node->sibling->prev = node->prev;
    if (node->prev->child == node) node->prev->child = node->sibling;
    else node->prev->sibling = node->sibling;

    node->sibling = nullptr;
    node->prev = nullptr;

    root = merge(root, node);
}
// zmiejszenie priorytetu elementu
void PairingHeapPQ::decreaseKey(int value, int newPriority) {
    PairingNode* node = findNode(root, value);
    if (!node) return;
    if (newPriority > node->priority) return;

    if (node != root) {
        if (node->sibling) node->sibling->prev = node->prev;
        if (node->prev->child == node) node->prev->child = node->sibling;
        else node->prev->sibling = node->sibling;
    }

    PairingNode* childrenHeap = combineSiblings(node->child);

    node->priority = newPriority;
    node->child = nullptr;
    node->sibling = nullptr;
    node->prev = nullptr;

    if (node == root) {
        root = childrenHeap; 
        root = merge(root, node); 
    }
    else {
            root = merge(root, childrenHeap);
        root = merge(root, node);
    }
}
// modyfikacja priorytetu
void PairingHeapPQ::modifyKey(int value, int newPriority) {
    PairingNode* node = findNode(root, value);
    if (!node) return;

    if (newPriority > node->priority) increaseKey(value, newPriority);
    else if (newPriority < node->priority) decreaseKey(value, newPriority);
}
// szukanie wartosci
PairingNode* PairingHeapPQ::findNode(PairingNode* startNode, int value) const {
    PairingNode* curr = startNode;
    while (curr != nullptr) {
        if (curr->value == value) return curr;

        if (curr->child != nullptr) {
            curr = curr->child; 
        }
        else if (curr->sibling != nullptr) {
            curr = curr->sibling;
        }
        else {
            PairingNode* p = curr->prev;

            while (p != nullptr && (p->sibling == nullptr || p->child == nullptr)) {
                p = p->prev;
            }
            if (p == nullptr) break;
            curr = p->sibling;
        }
    }
    return nullptr;
}

int PairingHeapPQ::returnSize() const {
    return size;
}

bool PairingHeapPQ::isEmpty() const {
    return root == nullptr;
}
// czyszczenie pamieci kopca
void PairingHeapPQ::clear(PairingNode* node) {
    while (node != nullptr) {
        if (node->child != nullptr) {
            PairingNode* temp = node->child;
            node->child = temp->sibling;
            temp->sibling = node->sibling;
            node->sibling = temp;
        }
        else {
            PairingNode* next = node->sibling;
            delete node;
            node = next;
        }
    }
}

void PairingHeapPQ::print() const {
    if (!root) std::cout << "Heap empty.";
    std::cout << std::endl;
}
