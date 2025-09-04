#include <fstream>
#include <algorithm>
#include "history.h"
#include "globals.h"

using namespace std;


// Loads command history from the history file into the global vector.
void load_history() {
    history_file_path = string(getenv("HOME")) + "/.myshell_history";
    ifstream history_file(history_file_path);
    if (history_file.is_open()) {
        string line;
        while (getline(history_file, line)) {
            command_history.push_back(line);
        }
        history_file.close();
    }
}

// Saves the current command history from the vector to the file.
void save_history() {
    ofstream history_file(history_file_path, ios::trunc); // Overwrite the file
    if (history_file.is_open()) {
        for (const auto& cmd : command_history) {
            history_file << cmd << endl;
        }
        history_file.close();
    }
}

// Adds a new command to the history, managing the size limit.
void add_to_history(const string& command) {
    if (command.empty()) return;
    // Prevent adding consecutive duplicate commands
    if (!command_history.empty() && command_history.back() == command) {
        return;
    }
    // If history is full, remove the oldest command
    if (command_history.size() >= 20) {
        command_history.erase(command_history.begin());
    }
    command_history.push_back(command);
}

