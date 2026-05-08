#include "PriorityQueueHeap.h"
#include <iostream>

int main() {
    BinaryHeapPQ pq;
    pq.insert(10, 3);
    pq.insert(12, 4);
    pq.print();
    return 0;
}