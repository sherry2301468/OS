#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

/*
    Simple paging simulation using the Aging algorithm.

    Idea:
    - each frame stores a page number
    - each frame also has an "age" counter
    - every memory reference:
        * shift ages right (older over time)
        * if page is used -> set MSB bit
        * if page not in memory -> page fault
          replace frame with smallest age
*/

struct Frame {
    int page;
    unsigned int age;
};

// read page references from file
vector<int> readFile(string filename) {
    vector<int> refs;
    ifstream in(filename);

    if (!in) {
        cout << "Could not open file\n";
        return refs;
    }

    int x;
    while (in >> x) {
        refs.push_back(x);
    }

    return refs;
}

// run aging simulation for a given number of frames
double simulate(vector<int>& refs, int frameCount) {

    const int AGE_BITS = 8;          // 8-bit aging register
    unsigned int MSB = 1 << (AGE_BITS - 1);
    unsigned int MASK = (1 << AGE_BITS) - 1;

    vector<Frame> frames(frameCount);

    // initialize frames
    for (int i = 0; i < frameCount; i++) {
        frames[i].page = -1;
        frames[i].age = 0;
    }

    int faults = 0;

    // go through each memory reference
    for (int page : refs) {

        // shift all ages (aging effect)
        for (auto &f : frames) {
            f.age = (f.age >> 1) & MASK;
        }

        // check if page already exists
        int hit = -1;
        for (int i = 0; i < frameCount; i++) {
            if (frames[i].page == page) {
                hit = i;
                break;
            }
        }

        if (hit != -1) {
            // page hit -> mark as recently used
            frames[hit].age |= MSB;
            continue;
        }

        // page fault happens
        faults++;

        // try to find empty frame first
        int place = -1;
        for (int i = 0; i < frameCount; i++) {
            if (frames[i].page == -1) {
                place = i;
                break;
            }
        }

        // if no empty frame -> replace smallest age
        if (place == -1) {
            unsigned int smallest = frames[0].age;
            place = 0;

            for (int i = 1; i < frameCount; i++) {
                if (frames[i].age < smallest) {
                    smallest = frames[i].age;
                    place = i;
                }
            }
        }

        // load new page
        frames[place].page = page;
        frames[place].age = MSB;
    }

    // faults per 1000 references
    double result = (1000.0 * faults) / refs.size();
    return result;
}

int main(int argc, char* argv[]) {

    // expected:
    // program refs.txt maxFrames

    if (argc < 3) {
        cout << "Usage: program <file> <maxFrames>\n";
        return 1;
    }

    string filename = argv[1];
    int maxFrames = stoi(argv[2]);

    vector<int> refs = readFile(filename);

    if (refs.empty()) {
        cout << "No references found.\n";
        return 1;
    }

    cout << "Total references: " << refs.size() << "\n\n";

    // save results for plotting
    ofstream csv("results.csv");
    csv << "frames,faults_per_1000\n";

    cout << left << setw(10) << "Frames"
         << "Faults/1000\n";

    for (int f = 1; f <= maxFrames; f++) {

        double faultsPer1000 = simulate(refs, f);

        cout << left << setw(10) << f
             << fixed << setprecision(3)
             << faultsPer1000 << "\n";

        csv << f << "," << faultsPer1000 << "\n";
    }

    cout << "\nresults.csv created (use it for plotting)\n";

    return 0;
}