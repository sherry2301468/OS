#include <iostream>
#include <fstream>
using namespace std;

int main() {
    ifstream file("input.txt");

    if (!file) {
        cout << "Error: input file not found\n";
        return 1;
    }

    int p, r;
    int E[10], A[10];
    int C[10][10], R[10][10];
    bool finish[10] = {false};

    file >> p >> r;

    if (p > 10 || r > 10) {
        cout << "Error: maximum size is 10\n";
        return 1;
    }

    // read existing resources
    for (int j = 0; j < r; j++) {
        file >> E[j];
    }

    // read current allocation matrix
    for (int i = 0; i < p; i++) {
        for (int j = 0; j < r; j++) {
            file >> C[i][j];
        }
    }

    // read request matrix
    for (int i = 0; i < p; i++) {
        for (int j = 0; j < r; j++) {
            file >> R[i][j];
        }
    }

    // calculate available resources
    for (int j = 0; j < r; j++) {
        int totalAllocated = 0;

        for (int i = 0; i < p; i++) {
            totalAllocated += C[i][j];
        }

        A[j] = E[j] - totalAllocated;

        if (A[j] < 0) {
            cout << "Error: invalid input data\n";
            return 1;
        }
    }

    bool progress = true;

    while (progress) {
        progress = false;

        for (int i = 0; i < p; i++) {
            if (finish[i] == false) {
                bool canRun = true;

                for (int j = 0; j < r; j++) {
                    if (R[i][j] > A[j]) {
                        canRun = false;
                        break;
                    }
                }

                if (canRun) {
                    for (int j = 0; j < r; j++) {
                        A[j] += C[i][j];
                    }

                    finish[i] = true;
                    progress = true;
                }
            }
        }
    }

    bool deadlock = false;

    for (int i = 0; i < p; i++) {
        if (finish[i] == false) {
            deadlock = true;
        }
    }

    if (deadlock == false) {
        cout << "No deadlock detected\n";
    } else {
        cout << "Deadlock detected\n";
        cout << "Deadlocked processes: ";

        for (int i = 0; i < p; i++) {
            if (finish[i] == false) {
                cout << "P" << i << " ";
            }
        }

        cout << "\n";
    }

    return 0;
}
