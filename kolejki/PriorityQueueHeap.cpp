#include "PriorityQueueHeap.h"

void HeapArray::grow() {
    int newCap   = (cap == 0) ? 4 : cap * 2;
    Element* buf = new Element[newCap];
    for (int i = 0; i < sz; i++)
        buf[i] = data[i];
    delete[] data;
    data = buf;
    cap  = newCap;
}

HeapArray::HeapArray() : data(nullptr), sz(0), cap(0) {}

HeapArray::~HeapArray() { delete[] data; }

void HeapArray::push_back(const Element& e) {
    if (sz == cap) grow();
    data[sz++] = e;
}

void HeapArray::pop_back() {
    if (sz == 0)
        throw std::underflow_error("HeapArray: pop_back na pustej tablicy");
    --sz;
}

Element& HeapArray::operator[](int i)             { return data[i]; }
const Element& HeapArray::operator[](int i) const { return data[i]; }

Element&       HeapArray::back()       { return data[sz - 1]; }
const Element& HeapArray::back() const { return data[sz - 1]; }

void HeapArray::swap_at(int i, int j) {
    Element tmp = data[i];
    data[i]     = data[j];
    data[j]     = tmp;
}

int  HeapArray::size()  const { return sz; }
bool HeapArray::empty() const { return sz == 0; }

int BinaryHeapPQ::parent(int i)     const { return (i - 1) / 2; }
int BinaryHeapPQ::leftChild(int i)  const { return 2 * i + 1; }
int BinaryHeapPQ::rightChild(int i) const { return 2 * i + 2; }

// Przesiewanie w górę – po wstawieniu elementu na koniec tablicy.
void BinaryHeapPQ::siftUp(int i) {
    while (i > 0 && heap[parent(i)].priority < heap[i].priority) {
        heap.swap_at(i, parent(i));
        i = parent(i);
    }
}

// Przesiewanie w dół – po usunięciu korzenia lub zmniejszeniu priorytetu.
void BinaryHeapPQ::siftDown(int i) {
    int n       = heap.size();
    int largest = i;
    int l       = leftChild(i);
    int r       = rightChild(i);

    if (l < n && heap[l].priority > heap[largest].priority)
        largest = l;
    if (r < n && heap[r].priority > heap[largest].priority)
        largest = r;

    if (largest != i) {
        heap.swap_at(i, largest);
        siftDown(largest);
    }
}

// Dodaje element na koniec i naprawia kopiec przez sift-up.
void BinaryHeapPQ::insert(int value, int priority) {
    heap.push_back({value, priority});
    siftUp(heap.size() - 1);
}

// Usuwa i zwraca element o najwyższym priorytecie (korzeń).
// Ostatni element trafia na korzeń, następnie sift-down.
Element BinaryHeapPQ::extractMax() {
    if (isEmpty())
        throw std::underflow_error("Kolejka jest pusta");

    Element maxElem = heap[0];
    heap[0]         = heap.back();
    heap.pop_back();

    if (!isEmpty())
        siftDown(0);

    return maxElem;
}

// Zwraca korzeń bez usuwania.
Element BinaryHeapPQ::findMax() const {
    if (isEmpty())
        throw std::underflow_error("Kolejka jest pusta");
    return heap[0];
}

// Zmienia priorytet pierwszego elementu o podanej wartości.
// Strategia przy duplikatach: modyfikowany jest element
// o najniższym indeksie (wstawiony najwcześniej – FIFO).
void BinaryHeapPQ::modifyKey(int value, int newPriority) {
    for (int i = 0; i < heap.size(); i++) {
        if (heap[i].value == value) {
            int old          = heap[i].priority;
            heap[i].priority = newPriority;
            if      (newPriority > old) siftUp(i);
            else if (newPriority < old) siftDown(i);
            return;
        }
    }
    throw std::invalid_argument("Element o podanej wartosci nie istnieje");
}

// Zwiększa priorytet elementu pod podanym indeksem → sift-up.
void BinaryHeapPQ::increaseKey(int index, int newPriority) {
    if (index < 0 || index >= heap.size())
        throw std::out_of_range("Nieprawidlowy indeks");
    if (newPriority < heap[index].priority)
        throw std::invalid_argument("Nowy priorytet musi byc wiekszy lub rowny");
    heap[index].priority = newPriority;
    siftUp(index);
}

// Zmniejsza priorytet elementu pod podanym indeksem → sift-down.
void BinaryHeapPQ::decreaseKey(int index, int newPriority) {
    if (index < 0 || index >= heap.size())
        throw std::out_of_range("Nieprawidlowy indeks");
    if (newPriority > heap[index].priority)
        throw std::invalid_argument("Nowy priorytet musi byc mniejszy lub rowny");
    heap[index].priority = newPriority;
    siftDown(index);
}

int  BinaryHeapPQ::returnSize() const { return heap.size(); }
bool BinaryHeapPQ::isEmpty()    const { return heap.empty(); }

void BinaryHeapPQ::print() const {
    std::cout << "{ ";
    for (int i = 0; i < heap.size(); i++)
        std::cout << "(" << heap[i].value << ", p=" << heap[i].priority << ") ";
    std::cout << "}\n";
}