#include "myshell.h"

using namespace std;

// findLcsPrefix -> Function to find the longest common preFix for multiple mtchs
string findLcsPrefix(const vector<string>& mtchs) {
    if (mtchs.empty()) return "";
    string ft = mtchs[0];
    for (size_t i = 0; i < ft.length(); ++i) {
        for (size_t j = 1; j < mtchs.size(); ++j) {
            if (i >= mtchs[j].length() || mtchs[j][i] != ft[i]) {
                return ft.substr(0, i);
            }
        }   
    }
    return ft;
}

void autoCmpltFunctionHandler(string& l) {
    size_t wrdStrt = l.find_last_of(" \t\n|;&<>");
    wrdStrt = (wrdStrt == string::npos) ? 0 : wrdStrt + 1;

    string currWrd = l.substr(wrdStrt);
    bool isCmnd = (l.find_first_of(" \t") == string::npos && wrdStrt == 0);

    vector<string> mtchs;
    
    // Generate potential mtchs 
    if (isCmnd) {
        // Match Commands from PATH
        string pthEnv = getenv("PATH");
        stringstream ss(pthEnv);
        string path_dir;
        while (getline(ss, path_dir, ':')) {
            DIR* dir = opendir(path_dir.c_str());
            if (dir) {
                struct dirent* e;
                while ((e = readdir(dir)) != nullptr) {
                    string name = e->d_name;
                    if (name.rfind(currWrd, 0) == 0) mtchs.push_back(name);
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
                if (name.rfind(currWrd, 0) == 0) mtchs.push_back(name);
            }
            closedir(dir);
        }
    }
    sort(mtchs.begin(), mtchs.end());

    if (mtchs.empty()) return;

    // Handle the matches
    if (mtchs.size() == 1) {
        // case : only single match 
        // if its only single match them complete it
        string completion = mtchs[0].substr(currWrd.length());
        l += completion;
        struct stat st;
        if (stat(mtchs[0].c_str(), &st) == 0 && !S_ISDIR(st.st_mode)) {
            // if its file add sapce
             l += " "; 
        }
    } else {
        // case : Multiple matches
        // In case of multiple matches find common preFix
        string preFix = findLcsPrefix(mtchs);
        if (preFix.length() > currWrd.length()) {
            l += preFix.substr(currWrd.length());
        } else {
            // If TAB pressed again, show all options
            cout << endl;
            for (const auto& match : mtchs) {
                cout << match << "\t";
            }
            cout << endl;
            promptDisplay();
            cout << l;
            fflush(stdout);
        }
    }
}

