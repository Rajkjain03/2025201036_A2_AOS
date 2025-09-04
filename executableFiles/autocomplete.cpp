#include <iostream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <stdlib.h>
#include "autocomplete.h"
#include "prompt.h"

using namespace std;

// Helper to find the longest common prefix for multiple matches ---
string find_longest_common_prefix(const vector<string>& matches) {
    if (matches.empty()) return "";
    string first = matches[0];
    for (size_t i = 0; i < first.length(); ++i) {
        for (size_t j = 1; j < matches.size(); ++j) {
            if (i >= matches[j].length() || matches[j][i] != first[i]) {
                return first.substr(0, i);
            }
        }   
    }
    return first;
}

// The core autocomplete logic function 
void handle_autocomplete(string& line) {
    size_t word_start = line.find_last_of(" \t\n|;&<>");
    word_start = (word_start == string::npos) ? 0 : word_start + 1;

    string current_word = line.substr(word_start);
    bool is_command = (line.find_first_of(" \t") == string::npos && word_start == 0);

    vector<string> matches;
    
    // --- Generate potential matches ---
    if (is_command) {
        // Match Commands from PATH
        string path_env = getenv("PATH");
        stringstream ss(path_env);
        string path_dir;
        while (getline(ss, path_dir, ':')) {
            DIR* dir = opendir(path_dir.c_str());
            if (dir) {
                struct dirent* e;
                while ((e = readdir(dir)) != nullptr) {
                    string name = e->d_name;
                    if (name.rfind(current_word, 0) == 0) matches.push_back(name);
                }
                closedir(dir);
            }
        }
    } else {
        // Match Files/Directories
        DIR* dir = opendir(".");
        if (dir) {
            struct dirent* e;
            while ((e = readdir(dir)) != nullptr) {
                string name = e->d_name;
                if (name.rfind(current_word, 0) == 0) matches.push_back(name);
            }
            closedir(dir);
        }
    }
    sort(matches.begin(), matches.end());

    if (matches.empty()) return;

    // --- Process the matches ---
    if (matches.size() == 1) {
        // Single match: complete it
        string completion = matches[0].substr(current_word.length());
        line += completion;
        struct stat st;
        if (stat(matches[0].c_str(), &st) == 0 && !S_ISDIR(st.st_mode)) {
             line += " "; // Add a space if it's a file
        }
    } else {
        // Multiple matches: find common prefix
        string prefix = find_longest_common_prefix(matches);
        if (prefix.length() > current_word.length()) {
            line += prefix.substr(current_word.length());
        } else {
            // If TAB pressed again, show all options
            cout << endl;
            for (const auto& match : matches) {
                cout << match << "\t";
            }
            cout << endl;
            promptDisplay();
            cout << line;
            fflush(stdout);
        }
    }
}

