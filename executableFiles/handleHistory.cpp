#include "myshell.h"

using namespace std;


// Loads command history from the history file into the global vector.
void loaddHist() {
    histFilePth = string(getenv("HOME")) + "/.myshell_history";
    ifstream history_file(histFilePth);
    if (history_file.is_open()) {
        string line;
        while (getline(history_file, line)) {
            cmndHist.push_back(line);
        }
        history_file.close();
    }
}

// Saves the current command history from the vector to the file.
void saveHist() {
    ofstream history_file(histFilePth, ios::trunc); // Overwrite the file
    if (history_file.is_open()) {
        for (const auto& cmd : cmndHist) {
            history_file << cmd << endl;
        }
        history_file.close();
    }
}

// Adds a new command to the history, managing the size limit.
void addToHis(const string& command) {
    if (command.empty()) return;
    // Prevent adding consecutive duplicate commands
    if (!cmndHist.empty() && cmndHist.back() == command) {
        return;
    }
    // If history is full, remove the oldest command
    if (cmndHist.size() >= 20) {
        cmndHist.erase(cmndHist.begin());
    }
    cmndHist.push_back(command);
}

