#include "PriorityQueueHeap.h"
#include <iostream>

int BinaryHeapPQ::parent(int i) const {
    return (i - 1) / 2;
}

int BinaryHeapPQ::leftChild(int i) const {
    return 2 * i + 1;
}

int BinaryHeapPQ::rightChild(int i) const {
    return 2 * i + 2;
}

// Przesiewanie w górę – po wstawieniu elementu na koniec tablicy
void BinaryHeapPQ::siftUp(int i) {
    while (i > 0 && heap[parent(i)].priority < heap[i].priority) {
        std::swap(heap[i], heap[parent(i)]);
        i = parent(i);
    }
}

// Przesiewanie w dół – po usunięciu korzenia
void BinaryHeapPQ::siftDown(int i) {
    int size = heap.size();
    int largest = i;

    int l = leftChild(i);
    int r = rightChild(i);

    if (l < size && heap[l].priority > heap[largest].priority)
        largest = l;
    if (r < size && heap[r].priority > heap[largest].priority)
        largest = r;

    if (largest != i) {
        std::swap(heap[i], heap[largest]);
        siftDown(largest);
    }
}

// Dodaje element na koniec i naprawia kopiec przez sift-up
// O(log n) pesymistycznie, O(1) optymistycznie
void BinaryHeapPQ::insert(int value, int priority) {
    heap.push_back({value, priority});
    siftUp(heap.size() - 1);
}

// Usuwa i zwraca element o najwyższym priorytecie (korzeń)
// Ostatni element trafia na korzeń, następnie sift-down
// O(log n) zawsze
Element BinaryHeapPQ::extractMax() {
    if (isEmpty())
        throw std::underflow_error("Kolejka jest pusta");

    Element max = heap[0];
    heap[0] = heap.back();
    heap.pop_back();

    if (!isEmpty())
        siftDown(0);

    return max;
}

// Zwraca korzeń bez usuwania
// O(1)
Element BinaryHeapPQ::findMax() const {
    if (isEmpty())
        throw std::underflow_error("Kolejka jest pusta");
    return heap[0];
}

// Zmienia priorytet elementu o podanej wartości
// O(n) szukanie + O(log n) naprawa
void BinaryHeapPQ::modifyKey(int value, int newPriority) {
    for (int i = 0; i < (int)heap.size(); i++) {
        if (heap[i].value == value) {
            int old = heap[i].priority;
            heap[i].priority = newPriority;
            if (newPriority > old)
                siftUp(i);
            else
                siftDown(i);
            return;
        }
    }
    throw std::invalid_argument("Element o podanej wartosci nie istnieje");
}

// Zwiększa priorytet elementu o podanym indeksie → sift-up
// O(log n)
void BinaryHeapPQ::increaseKey(int index, int newPriority) {
    if (index < 0 || index >= (int)heap.size())
        throw std::out_of_range("Nieprawidlowy indeks");
    if (newPriority < heap[index].priority)
        throw std::invalid_argument("Nowy priorytet musi byc wiekszy");

    heap[index].priority = newPriority;
    siftUp(index);
}

// Zmniejsza priorytet elementu o podanym indeksie → sift-down
// O(log n)
void BinaryHeapPQ::decreaseKey(int index, int newPriority) {
    if (index < 0 || index >= (int)heap.size())
        throw std::out_of_range("Nieprawidlowy indeks");
    if (newPriority > heap[index].priority)
        throw std::invalid_argument("Nowy priorytet musi byc mniejszy");

    heap[index].priority = newPriority;
    siftDown(index);
}

// O(1)
int BinaryHeapPQ::returnSize() const {
    return heap.size();
}

bool BinaryHeapPQ::isEmpty() const {
    return heap.empty();
}

void BinaryHeapPQ::print() const {
    std::cout << "{ ";
    for (const auto& e : heap)
        std::cout << "(" << e.value << ", p=" << e.priority << "), ";
    std::cout << "}\n";
}