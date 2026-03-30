#include <iostream>
#include <map>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdlib>

using namespace std;

// Function to scan a directory recursively
void scanDirectory(string path, map<long long, int>& histogram, long long binWidth) {
    DIR* dir = opendir(path.c_str());

    if (dir == NULL) {
        cerr << "Cannot open directory: " << path << endl;
        return;
    }

    dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        string name = entry->d_name;

        // Skip current and parent directory
        if (name == "." || name == "..") {
            continue;
        }

        string fullPath = path + "/" + name;

        struct stat fileInfo;
        if (lstat(fullPath.c_str(), &fileInfo) == -1) {
            cerr << "Error reading file: " << fullPath << endl;
            continue;
        }

        // If it is a directory, go inside it
        if (S_ISDIR(fileInfo.st_mode)) {
            scanDirectory(fullPath, histogram, binWidth);
        }
        // If it is a regular file, record its size
        else if (S_ISREG(fileInfo.st_mode)) {
            long long size = fileInfo.st_size;
            long long bin = (size / binWidth) * binWidth;
            histogram[bin]++;
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <directory> <bin_width>" << endl;
        return 1;
    }

    string startDir = argv[1];
    long long binWidth = atoll(argv[2]);

    if (binWidth <= 0) {
        cout << "Bin width must be greater than 0" << endl;
        return 1;
    }

    map<long long, int> histogram;

    scanDirectory(startDir, histogram, binWidth);

    cout << "\nHistogram of file sizes:\n";
    for (auto it = histogram.begin(); it != histogram.end(); it++) {
        long long start = it->first;
        long long end = start + binWidth - 1;
        cout << start << " - " << end << " bytes : " << it->second << " file(s)" << endl;
    }

    return 0;
}
