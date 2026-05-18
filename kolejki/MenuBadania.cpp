#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include "PriorityQueueHeap.h"
#include "PriorityQueuePairing.h"

enum Operation {
    INSERT, EXTRACT_MAX, FIND_MAX, MODIFY_KEY, RETURN_SIZE
};

std::string getOpName(Operation op) {
    switch (op) {
    case INSERT: return "Insert";
    case EXTRACT_MAX: return "Extract_Max";
    case FIND_MAX: return "Find_Max";
    case MODIFY_KEY: return "Modify_Key";
    case RETURN_SIZE: return "Return_Size";
    default: return "Unknown";
    }
}

std::mt19937& getGenerator(unsigned int seed) {
    static std::mt19937 gen(seed);
    static unsigned int currentSeed = seed;
    if (seed != currentSeed) {
        gen.seed(seed);
        currentSeed = seed;
    }
    return gen;
}

void generatePriorityFile(unsigned int seed, int size, std::string fileName) {
    std::ofstream file(fileName);
    if (!file.is_open()) return;

    auto& gen = getGenerator(seed);
    std::uniform_int_distribution<int> valDist(0, 1000000);
    std::uniform_int_distribution<int> prioDist(0, size * 10); // Zakres priorytetów 10 razy wiekszy od rozmiaru

    file << size << "\n";
    for (int i = 0; i < size; i++) {
        file << valDist(gen) << " " << prioDist(gen) << "\n";
    }
    file.close();
    std::cout << "Wygenerowano plik '" << fileName << "' przy uzyciu mt19937." << std::endl;
}

template <typename T>
long long measurePQ(int size, Operation op, unsigned int seed, int repetitions, bool typ) {
    long long totalDuration = 0;
    auto& gen = getGenerator(seed); // Inicjalizacja generatora z danym seedem

    std::uniform_int_distribution<int> valDist(0, 1000000);
    std::uniform_int_distribution<int> prioDist(0, size * 10); // Zakres priorytetów 10 razy wiekszy od rozmiaru

    for (int r = 0; r < repetitions; r++) {
        T* pq = new T();
        std::vector<int> insertedValues;
        insertedValues.reserve(size); 
        // Wypełnienie struktury
        for (int j = 0; j < size; j++) {
            int v = valDist(gen);
            int p = prioDist(gen);
            pq->insert(v, p);
            insertedValues.push_back(v);
        }

        int v = valDist(gen);
        int p = prioDist(gen);
        int existingValue = 0;
        // Upewnienie się, ze przy modify key uzyjemy istniejacej wartosci
        if (op == MODIFY_KEY && !insertedValues.empty()) {
            std::uniform_int_distribution<int> indexDist(0, size - 1);
            existingValue = insertedValues[indexDist(gen)];
        }
        // Rozgerzanie struktury przed wlaściwym badaniem, dla pairing heapa po wstepnym wypełnieniu struktury
        // bez rozgrzewki wyniki są sztucznie zwiekszone, ponieważ jest bardzo słabo zorganizowana
        if (op == EXTRACT_MAX) {
            int warmup = (int)std::sqrt(size);
            for (int i = 0; i < warmup && !pq->isEmpty(); i++) {
                pq->extractMax();
            }
        }
        auto start = std::chrono::high_resolution_clock::now();
        // Wykonanie odpowiednich operacji
        if (op == INSERT) {
            pq->insert(v, p);
        }
        else if (op == EXTRACT_MAX) {
            if (!pq->isEmpty()) pq->extractMax();
        }
        else if (op == FIND_MAX) {
            if (!pq->isEmpty()) pq->findMax();
        }
        else if (op == MODIFY_KEY) {
            try {
                pq->modifyKey(existingValue, p);
            }
            catch (...) {
            }
        }
        else if (op == RETURN_SIZE) {
            pq->returnSize();
        }

        auto end = std::chrono::high_resolution_clock::now();

        totalDuration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        delete pq;
    }
    return totalDuration / repetitions;
}

void runPQTests(const unsigned int seeds[10]) {
    std::vector<Operation> ops = { INSERT, EXTRACT_MAX, FIND_MAX, MODIFY_KEY, RETURN_SIZE };
    for (auto op : ops) {
        std::string fileName = "test_" + getOpName(op) + ".csv";
        std::ofstream file(fileName);
        file << "Rozmiar;BinaryHeap [ns];PairingHeap [ns]\n";

        std::cout << "\nBadanie: " << getOpName(op) << "..." << std::endl;

        for (int i = 1; i <= 8; i++) { // Wykonanie badań dla 8 rozmiarów co 5k elementów od 5k do 40k
            int N = i * 5000;
            long long avgBinary = 0, avgPairing = 0;

            for (int s = 0; s < 10; s++) { // Wykonanie badan dla 10 roznych seedow dla kazdego rozmiaru
                if (s == 9) {
                    std::cout << "\rRozmiar: " << std::setw(6) << N
                        << " | Postep: [" << s + 1 << "/10] | Binary... " << std::flush;
                }
                else {
                    std::cout << "\rRozmiar: " << std::setw(6) << N
                        << " | Postep: [0" << s + 1 << "/10] | Binary... " << std::flush;
                }
                // Wykonywanie kazdego seeda 10 razy
                avgBinary += measurePQ<BinaryHeapPQ>(N, op, seeds[s], 10, 0);
                std::cout << "OK | Pairing... " << std::flush;
                avgPairing += measurePQ<PairingHeapPQ>(N, op, seeds[s], 10, 1);
                std::cout << "OK" << std::flush;
            }
            // Zapisanie czasu w pliku
            file << N << ";" << avgBinary / 10 << ";" << avgPairing / 10 << "\n";
        }
        file.close();
        system("cls");
    }
}

template <typename T>
void pqMenu(std::string name) {
    T pq;
    int choice = -1;
    std::random_device rd;
    std::mt19937 gen(rd());

    while (choice != 0) {
        std::cout << "\n--- " << name << " ---" << std::endl;
        std::cout << "1. Zbuduj z pliku\n2. Losowe wypelnienie\n3. Insert\n4. Find-Max\n5. Extract-Max\n6. Modify-Key\n7. Return-Size\n0. Powrot\nWybor: ";
        if (!(std::cin >> choice)) { std::cin.clear(); while (std::cin.get() != '\n'); continue; }
        std::system("cls");
        switch (choice) {
        case 1: {
            std::string path; std::cout << "Sciezka: "; std::cin >> path;
            std::ifstream f(path);
            if (f.is_open()) {
                int n, v, p; f >> n;
                while (f >> v >> p) pq.insert(v, p);
                std::cout << "Wczytano dane.\n";
            }
            break;
        }
        case 2: {
            int n; std::cout << "Ile elementow: "; std::cin >> n;
            std::uniform_int_distribution<int> d(0, n * 10);
            for (int i = 0; i < n; i++) pq.insert(i, d(gen));
            break;
        }
        case 3: {
            int v, p; 
            std::cout << "Wartosc: "; std::cin >> v ;
            std::cout << "\nPriorytet: "; std::cin >> p;
            pq.insert(v, p);
            break;
        }
        case 4: {
            try {
                Element e = pq.findMax();
                std::cout << "Max: " << e.value << " Prio: " << e.priority << "\n";
            }
            catch (...) { std::cout << "Blad!\n"; }
            break;
        }
        case 5: {
            try {
                Element e = pq.extractMax();
                std::cout << "Usunieto:\n";
                std::cout << "Max: " << e.value << " Prio: " << e.priority << "\n";
            }
            catch (...) { std::cout << "Blad!\n"; }
            break;
        }
        case 6: {
            int v, p; 
            std::cout << "Wartosc: "; std::cin >> v;
            std::cout << "\nNowy priorytet: "; std::cin >> p;
            pq.insert(v, p);
            pq.modifyKey(v, p);
            break;
        }
        case 7: {
            int s = pq.returnSize();
            std::cout << "Rozmiar: " << s << std::endl;
            break;
        }
        }
    }
}

int main() {
    const unsigned int seeds[10] = { 12345, 67890, 11223, 44556, 77889, 99001, 13579, 24680, 98765, 54321 };
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n=== BADANIE KOLEJEK PRIORYTETOWYCH ===\n";
        std::cout << "1. Binary Heap\n2. Pairing Heap\n3. Wygeneruj plik testowy\n4. Badania porownawcze\n0. Wyjscie\nWybor: ";
        if (!(std::cin >> choice)) { std::cin.clear(); while (std::cin.get() != '\n'); continue; }
        std::system("cls");
        switch (choice) {
        case 1: pqMenu<BinaryHeapPQ>("Binary Heap"); break;
        case 2: pqMenu<PairingHeapPQ>("Pairing Heap"); break;
        case 3: {
            int n; std::string name;
            std::cout << "N: "; std::cin >> n;
            std::cout << "Nazwa: "; std::cin >> name;
            generatePriorityFile(seeds[0], n, name);
            break;
        }
        case 4: runPQTests(seeds); break;
        }
    }
    return 0;
}
