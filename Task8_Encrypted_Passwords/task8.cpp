#include <iostream>
#include <openssl/des.h>
#include <cstring>
#include <vector>
#include <iomanip>
#include <sstream>
#include <random>

using namespace std;

struct PasswordEntry {
    string password;
    string storedValue;   // salt$hash
};

string toHex(const unsigned char* data, int len) {
    stringstream ss;
    ss << hex << setfill('0');
    for (int i = 0; i < len; i++) {
        ss << setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

string saltToHex(unsigned short salt) {
    stringstream ss;
    ss << hex << setfill('0') << setw(4) << salt;
    return ss.str();
}

// Fill DES key from first 8 chars of password
void passwordToKey(const string& password, DES_cblock &key) {
    unsigned char raw[8] = {0};

    for (size_t i = 0; i < password.size() && i < 8; i++) {
        raw[i] = static_cast<unsigned char>(password[i]) & 0x7F;
    }

    for (int i = 0; i < 8; i++) {
        key[i] = raw[i] << 1;
    }

    DES_set_odd_parity(&key);
}

string encryptPassword(const string& password, unsigned short salt) {
    DES_cblock key;
    passwordToKey(password, key);

    DES_key_schedule schedule;
    DES_set_key_unchecked(&key, &schedule);

    DES_cblock block{};
    DES_cblock output{};

    unsigned char saltHi = static_cast<unsigned char>((salt >> 8) & 0xFF);
    unsigned char saltLo = static_cast<unsigned char>(salt & 0xFF);

    memset(block, 0, sizeof(block));

    for (int i = 0; i < 25; i++) {
        DES_ecb_encrypt(&block, &output, &schedule, DES_ENCRYPT);

        // 16-bit salt adaptation
        output[0] ^= saltHi;
        output[1] ^= saltLo;

        memcpy(block, output, sizeof(block));
    }

    return saltToHex(salt) + "$" + toHex(output, 8);
}

bool verifyPassword(const string& inputPassword, const string& storedValue) {
    size_t pos = storedValue.find('$');
    if (pos == string::npos) return false;

    string saltHex = storedValue.substr(0, pos);

    unsigned short salt;
    try {
        salt = static_cast<unsigned short>(stoul(saltHex, nullptr, 16));
    } catch (...) {
        return false;
    }

    string recomputed = encryptPassword(inputPassword, salt);
    return recomputed == storedValue;
}

int main() {
    vector<string> passwords = {
        "pass123", "hello", "qwerty", "admin",
        "letmein", "123456", "secret", "welcome",
        "dragon", "trustno1"
    };

    vector<PasswordEntry> entries;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<unsigned short> dist(0, 65535);

    cout << "Generated Encrypted Passwords:\n\n";

    for (int i = 0; i < 10; i++) {
        unsigned short salt = dist(gen);
        string stored = encryptPassword(passwords[i], salt);

        entries.push_back({passwords[i], stored});

        cout << i + 1 << ". Password: " << passwords[i]
             << " | Stored: " << stored << endl;
    }

    cout << "\n--- Password Verification ---\n";
    int entryNumber;
    string testPassword;

    cout << "Choose entry number to test (1-10): ";
    cin >> entryNumber;
    cin.ignore();

    if (entryNumber < 1 || entryNumber > 10) {
        cout << "Invalid entry number.\n";
        return 0;
    }

    cout << "Enter password to check: ";
    getline(cin, testPassword);

    if (verifyPassword(testPassword, entries[entryNumber - 1].storedValue)) {
        cout << "Password is VALID\n";
    } else {
        cout << "Password is INVALID\n";
    }

    return 0;
}