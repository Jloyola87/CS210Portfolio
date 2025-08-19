//============================================================================
// Name        : CornerGrocer.cpp
// Author      : Jose Oyola
// Description : A program to track item frequency for the Corner Grocer,
//               per CS 210 Project Three requirements.
//============================================================================






#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

// Helper: trim whitespace from both ends
static inline string trim(const string& s) {
    size_t start = s.find_first_not_of("\t\n\r ");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of("\t\n\r ");
    return s.substr(start, end - start + 1);
}

// Helper: lowercased copy (for case-insensitive counting/search)
static inline string toLowerCopy(const string& s) {
    string out;
    out.reserve(s.size());
    for (unsigned char ch : s) out.push_back(static_cast<char>(tolower(ch)));
    return out;
}

class GroceryTracker {
private:
    map<string, int> frequencyMap; // item (lowercase) -> count
    string inputFilePath;
    string backupFilePath; // frequency.dat

public:
    GroceryTracker(const string& inputFile, const string& backupFile)
        : inputFilePath(inputFile), backupFilePath(backupFile) {
    }

    // Load items from input file and immediately write backup (frequency.dat)
    bool initialize() {
        if (!loadFromInputFile()) {
            cerr << "Error: Could not open input file '" << inputFilePath << "'\n";
            return false;
        }
        if (!writeBackupFile()) {
            cerr << "Error: Could not create backup file '" << backupFilePath << "'\n";
            return false;
        }
        return true;
    }

    // Return frequency for a single item (case-insensitive)
    int getItemFrequency(const string& rawItem) const {
        string key = toLowerCopy(firstToken(rawItem));
        auto it = frequencyMap.find(key);
        if (it == frequencyMap.end()) return 0;
        return it->second;
    }

    // Print all items with numeric frequencies
    void printAllFrequencies() const {
        if (frequencyMap.empty()) {
            cout << "(No data loaded)\n";
            return;
        }
        for (const auto& kv : frequencyMap) {
            cout << kv.first << " " << kv.second << "\n";
        }
    }

    // Print text-based histogram using the provided symbol (default '*')
    void printHistogram(char symbol = '*') const {
        if (frequencyMap.empty()) {
            cout << "(No data loaded)\n";
            return;
        }
        for (const auto& kv : frequencyMap) {
            cout << kv.first << " ";
            for (int i = 0; i < kv.second; ++i) cout << symbol;
            cout << "\n";
        }
    }

private:
    // Load items from the input file into frequencyMap
    bool loadFromInputFile() {
        ifstream in(inputFilePath);
        if (!in.is_open()) return false;

        frequencyMap.clear();

        // The provided input file typically contains one item per line, but
        // we will read tokens so it also works with whitespace-separated items.
        string token;
        while (in >> token) {
            string key = toLowerCopy(token);
            ++frequencyMap[key];
        }
        return true;
    }

    // Write frequency.dat backup file with "item count" per line
    bool writeBackupFile() const {
        ofstream out(backupFilePath);
        if (!out.is_open()) return false;
        for (const auto& kv : frequencyMap) {
            out << kv.first << " " << kv.second << "\n";
        }
        return true;
    }

    // Extract the first token (word) from a potentially multi-word user input
    static string firstToken(const string& line) {
        istringstream iss(line);
        string word;
        iss >> word;
        return trim(word);
    }
};

// Robustly read an int choice in [minVal, maxVal]
bool readMenuChoice(int& outChoice, int minVal, int maxVal) {
    string line;
    if (!getline(cin, line)) return false;
    line = trim(line);
    if (line.empty()) return false;
    for (char c : line) {
        if (!isdigit(static_cast<unsigned char>(c)) && c != '-' && c != '+') {
            return false;
        }
    }
    try {
        int val = stoi(line);
        if (val < minVal || val > maxVal) return false;
        outChoice = val;
        return true;
    }
    catch (...) {
        return false;
    }
}

int main() {
    const string INPUT_FILE = "CS210_Project_Three_Input_File.txt";
    const string BACKUP_FILE = "frequency.dat";

    GroceryTracker tracker(INPUT_FILE, BACKUP_FILE);
    if (!tracker.initialize()) {
        cout << "\nPlease ensure the input file '" << INPUT_FILE << "' exists in the working directory.\n";
        cout << "Program will now exit.\n";
        return 1;
    }

    cout << "Corner Grocer Item Tracker\n";
    cout << "(Loaded input and created '" << BACKUP_FILE << "')\n\n";

    while (true) {
        cout << "==============================\n";
        cout << "1. Search item frequency\n";
        cout << "2. Print all item frequencies\n";
        cout << "3. Print histogram\n";
        cout << "4. Exit\n";
        cout << "Select an option (1-4): ";

        int choice = 0;
        if (!readMenuChoice(choice, 1, 4)) {
            cout << "\nInvalid input. Please enter a number 1-4.\n\n";
            continue;
        }

        if (choice == 4) {
            cout << "Goodbye!\n";
            break;
        }

        switch (choice) {
        case 1: {
            cout << "Enter the item to search: ";
            string line;
            if (!getline(cin, line)) line = ""; // handle EOF
            if (line.empty()) {
                cout << "No input detected. Try again.\n\n";
                break;
            }
            int count = tracker.getItemFrequency(line);
            cout << "Frequency of '" << trim(line) << "': " << count << "\n\n";
            break;
        }
        case 2: {
            cout << "\nItem Frequencies (item count):\n";
            tracker.printAllFrequencies();
            cout << "\n";
            break;
        }
        case 3: {
            cout << "\nHistogram (each * = 1):\n";
            tracker.printHistogram('*');
            cout << "\n";
            break;
        }
        default:
            // Should never hit due to validation
            cout << "Unexpected choice.\n\n";
            break;
        }
    }

    return 0;
}