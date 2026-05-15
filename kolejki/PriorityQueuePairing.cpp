#include "PriorityQueuePairing.h"

PairingHeapPQ::PairingHeapPQ() : root(nullptr), size(0) {}

PairingHeapPQ::~PairingHeapPQ() {
    clear(root);
}

// £¹czy dwa poddrzewa: mniejszy priorytet staje siê dzieckiem wiêkszego
PairingNode* PairingHeapPQ::merge(PairingNode* h1, PairingNode* h2) {
    if (!h1) return h2;
    if (!h2) return h1;

    if (h1->priority < h2->priority) {
        // Zamiana, aby h1 zawsze by³ tym z wiêkszym priorytetem
        PairingNode* temp = h1;
        h1 = h2;
        h2 = temp;
    }

    // Podczepianie h2 jako pierwsze dziecko h1
    h2->sibling = h1->child;
    if (h1->child) {
        h1->child->prev = h2;
    }
    h2->prev = h1;
    h1->child = h2;

    return h1;
}

// Kluczowa operacja: parowanie rodzeñstwa (od lewej, potem od prawej)
PairingNode* PairingHeapPQ::combineSiblings(PairingNode* firstSibling) {
    if (firstSibling == nullptr || firstSibling->sibling == nullptr) {
        return firstSibling;
    }

    // Pierwszy przebieg: parowanie od lewej do prawej
    PairingNode* head = nullptr;
    PairingNode* tail = nullptr;
    PairingNode* curr = firstSibling;

    while (curr != nullptr) {
        PairingNode* a = curr;
        PairingNode* b = a->sibling;

        if (b != nullptr) {
            PairingNode* next = b->sibling;
            // Izolujemy parê
            a->sibling = b->sibling = a->prev = b->prev = nullptr;
            PairingNode* paired = merge(a, b);

            // Dodajemy do tymczasowej listy
            if (head == nullptr) head = paired;
            else tail->sibling = paired;
            tail = paired;

            curr = next;
        }
        else {
            a->sibling = a->prev = nullptr;
            if (head == nullptr) head = a;
            else tail->sibling = a;
            curr = nullptr;
        }
    }

    // Drugi przebieg: ³¹czenie od prawej do lewej (akumulacja w jeden kopiec)
    // Poniewa¿ mamy tylko listê jednokierunkow¹, musimy to zrobiæ ostro¿nie
    // lub odwróciæ listê. Najproœciej: ³¹czymy wszystko do pierwszego elementu.
    PairingNode* result = head;
    curr = head->sibling;
    while (curr != nullptr) {
        PairingNode* next = curr->sibling;
        curr->sibling = curr->prev = nullptr;
        result = merge(result, curr);
        curr = next;
    }

    return result;
}

void PairingHeapPQ::insert(int value, int priority) {
    PairingNode* newNode = new PairingNode(value, priority);
    root = merge(root, newNode);
    size++;
}

Element PairingHeapPQ::findMax() const {
    if (isEmpty()) throw std::runtime_error("Heap is empty");
    return { root->value, root->priority };
}

Element PairingHeapPQ::extractMax() {
    if (isEmpty()) throw std::runtime_error("Heap is empty");

    Element maxElement = { root->value, root->priority };
    PairingNode* oldRoot = root;

    root = combineSiblings(root->child);
    if (root) root->prev = nullptr;

    delete oldRoot;
    size--;
    return maxElement;
}

void PairingHeapPQ::increaseKey(int value, int newPriority) {
    PairingNode* node = findNode(root, value);
    if (!node) return;
    if (newPriority < node->priority) return; // To nie jest increase

    node->priority = newPriority;
    if (node == root) return;

    // Wycinamy wêze³ z obecnego miejsca
    if (node->sibling) node->sibling->prev = node->prev;
    if (node->prev->child == node) node->prev->child = node->sibling;
    else node->prev->sibling = node->sibling;

    node->sibling = nullptr;
    node->prev = nullptr;

    // £¹czymy z korzeniem
    root = merge(root, node);
}

void PairingHeapPQ::decreaseKey(int value, int newPriority) {
    PairingNode* node = findNode(root, value);
    if (!node) return;
    if (newPriority > node->priority) return; // To nie jest decrease

    // W Pairing Heap najprostsza implementacja decreaseKey to:
    // Wyci¹æ wêze³, po³¹czyæ jego dzieci, a go samego wstawiæ ponownie.

    // 1. Odcinamy wêze³ od rodzica/rodzeñstwa
    if (node != root) {
        if (node->sibling) node->sibling->prev = node->prev;
        if (node->prev->child == node) node->prev->child = node->sibling;
        else node->prev->sibling = node->sibling;
    }

    // 2. £¹czymy dzieci tego wêz³a w oddzielny kopiec
    PairingNode* childrenHeap = combineSiblings(node->child);

    // 3. Aktualizujemy dane wêz³a (staje siê "nowym" liœciem)
    node->priority = newPriority;
    node->child = nullptr;
    node->sibling = nullptr;
    node->prev = nullptr;

    // 4. £¹czymy wszystko z powrotem do g³ównego korzenia
    if (node == root) {
        root = childrenHeap; // Jeœli to by³ korzeñ, nowym korzeniem s¹ dzieci
        root = merge(root, node); // A stary korzeñ (z mniejszym prio) wraca do gry
    }
    else {
        root = merge(root, childrenHeap);
        root = merge(root, node);
    }
}

void PairingHeapPQ::modifyKey(int value, int newPriority) {
    PairingNode* node = findNode(root, value);
    if (!node) return;

    if (newPriority > node->priority) increaseKey(value, newPriority);
    else if (newPriority < node->priority) decreaseKey(value, newPriority);
}

PairingNode* PairingHeapPQ::findNode(PairingNode* startNode, int value) const {
    PairingNode* curr = startNode;
    while (curr != nullptr) {
        if (curr->value == value) return curr;

        if (curr->child != nullptr) {
            curr = curr->child; // Idziemy w g³¹b
        }
        else if (curr->sibling != nullptr) {
            curr = curr->sibling; // Idziemy do brata
        }
        else {
            // Wracamy do góry i szukamy rodzeñstwa rodzica
            PairingNode* p = curr->prev;
            // Musimy sprawdziæ, czy prev to rodzic (wtedy p->child == curr)
            // czy rodzeñstwo (wtedy p->sibling == curr)
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

void PairingHeapPQ::clear(PairingNode* node) {
    while (node != nullptr) {
        if (node->child != nullptr) {
            // "Wyci¹gamy" dziecko na poziom rodzeñstwa, aby unikn¹æ rekurencji w g³¹b
            PairingNode* temp = node->child;
            node->child = temp->sibling;
            temp->sibling = node->sibling;
            node->sibling = temp;
        }
        else {
            // Jeœli wêze³ nie ma dzieci, usuwamy go i idziemy do rodzeñstwa
            PairingNode* next = node->sibling;
            delete node;
            node = next;
        }
    }
}

void PairingHeapPQ::print() const {
    if (!root) std::cout << "Heap empty.";
    // Tu mo¿na dodaæ prost¹ rekurencyjn¹ wizualizacjê
    std::cout << std::endl;
}