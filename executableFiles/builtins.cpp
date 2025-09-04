#include <iostream>
#include <iomanip>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include "builtins.h"
#include "globals.h"

using namespace std;

// To print file permisions
void printPerms(mode_t mode)
{   
    //print the drwxr-xr-x string
    cout << (S_ISDIR(mode) ? 'd' : (S_ISLNK(mode) ? 'l' : '-'));
    cout << ((mode & S_IRUSR) ? 'r' : '-');
    cout << ((mode & S_IWUSR) ? 'w' : '-');
    cout << ((mode & S_IXUSR) ? 'x' : '-');
    cout << ((mode & S_IRGRP) ? 'r' : '-');
    cout << ((mode & S_IWGRP) ? 'w' : '-');
    cout << ((mode & S_IXGRP) ? 'x' : '-');
    cout << ((mode & S_IROTH) ? 'r' : '-');
    cout << ((mode & S_IWOTH) ? 'w' : '-');
    cout << ((mode & S_IXOTH) ? 'x' : '-');
}

// History
void cmndHistory(const vector<string>& tkns) {
    // Default number of commands to show
    int num_to_show = 10; 
    if (tkns.size() > 1) {
        try {
            //stoi -> string to integer 
            num_to_show = stoi(tkns[1]);
        } catch (const invalid_argument& e) {
            cout << "Error -> History" << endl;
            return;
        }
    }

    // Calculates the correct starting point in the history vector
    int start_index = max(0, (int)command_history.size() - num_to_show);
    for (size_t i = start_index; i < command_history.size(); ++i) {
        cout << " " << i + 1 << "\t" << command_history[i] << endl;
    }
}


// this function to handle ls command
void cmndLs(const vector<string> &tkns) 
{   
    //checks if -a flag
    bool shAll = false;
    //checks if -l flag
    bool lFrmt = false;
    // to store all paths/directries/file arguments
    vector<string> pths;

    // Handling flags and paths
    // Iterate through all tokdens 
    for (size_t i = 1; i < tkns.size(); ++i)
    {
        string t = tkns[i];
        // if its flags
        if (t[0] == '-')
        {
            // flag check for 'a' and 'l'
            for (size_t j = 1; j < t.length(); j++)
            {
                if (t[j] == 'a')
                {   //its 'a'
                    shAll = true;
                }
                else if (t[j] == 'l')
                {   //its 'l'
                    lFrmt = true;
                }
            }
        }
        // other then flags like filess and directory.
        else
        {   
            // Case : ls ~   
            if (t == "~") {
                pths.push_back(shellHomeDir);
            } else {
                pths.push_back(t);
            }
        }
    }

    // If pths vector is empty, default to the current directory "."
    if (pths.empty())
    {   
        pths.push_back(".");
    }

    // opendir() ->  opens a directory to read its contents.
    // Traverse through pths vector 
    for (size_t i = 0; i < pths.size(); i++)
    {
        const string &path = pths[i];

        // If listing multiple directories, print the name first
        if (pths.size() > 1)
        {
            cout << path << ":" << endl;
        }
            
        //opendir() -> opens a directory and prepares it for reading its contents.
        DIR *dir = opendir(path.c_str());
        if (dir == nullptr)
        {
            perror(("Error -> ls: cannot access '" + path + "'").c_str());
            // Move to the next path if this one is invalid
            continue;
        }
        // read all entries into this vector
        vector<string> entries;
        struct dirent *e;
        // readdir(dir) -> reads the next entry (like a file or subdirectory) from an open directory stream.
        while ((e = readdir(dir)) != nullptr)
        {
            string name = e->d_name;
            if (!shAll && name[0] == '.')
            {   
                //  if -a is not specified Skip hidden files
                continue; 
            }
            entries.push_back(e->d_name);
        }

        //clean up.
        closedir(dir);

        // Sort entries alphabetically
        sort(entries.begin(), entries.end());

        // generating output
        if (lFrmt)
        {
            
            long long tBlcks = 0;
            // calculate total block size
            for (const auto &name : entries)
            {
                string fullPth = path;
                if (fullPth.back() != '/')
                {
                    fullPth += '/';
                }
                fullPth += name;
                struct stat fStat;
                if (stat(fullPth.c_str(), &fStat) == 0)
                {
                    tBlcks += fStat.st_blocks;
                }
            }

            // The st_blocks are usually 512B, ls reports in 1024B (1K)
            cout << "total " << tBlcks / 2 << endl;

            // detailed info for each e
            for (const auto &name : entries)
            {
                string fullPth = path;  
                if (fullPth.back() != '/')
                {
                    fullPth += '/';
                }
                fullPth += name;
                struct stat fStat;
                //lstat() -> get the file metadata    
                // Use lstat to get info about the link itself
                if (lstat(fullPth.c_str(), &fStat) == -1)
                {
                    perror(("Error -> lstat error for " + name).c_str());
                    continue;
                }

                //Permissions
                printPerms(fStat.st_mode);

                // Number of Links
                cout << " " << setw(2) << fStat.st_nlink;

                //Owner Name
                struct passwd *pw = getpwuid(fStat.st_uid);
                cout << " " << (pw ? pw->pw_name : to_string(fStat.st_uid));

                // Group Name
                struct group *gr = getgrgid(fStat.st_gid);
                cout << " " << (gr ? gr->gr_name : to_string(fStat.st_gid));

                // File Size
                cout << " " << setw(6) << fStat.st_size;

                // Timestamp formatting (recent vs. old)
                char time_buf[80];
                time_t current_time;
                time(&current_time);
                double seconds_diff = difftime(current_time, fStat.st_mtime);
                // More than 6 months (approx value)
                if (seconds_diff > (60.0 * 60.0 * 24.0 * 182.5))
                {
                    strftime(time_buf, sizeof(time_buf), "%b %d  %Y", localtime(&fStat.st_mtime));
                }
                else
                {
                    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&fStat.st_mtime));
                }
                cout << " " << time_buf;

                cout << " " << name;

                // Handle symbolic links
                if (S_ISLNK(fStat.st_mode))
                {
                    char link_target[1024];
                    ssize_t len = readlink(fullPth.c_str(), link_target, sizeof(link_target) - 1);
                    if (len != -1)
                    {
                        link_target[len] = '\0';
                        cout << " -> " << link_target;
                    }
                }
                cout << endl;
            }
        }
        else
        {
           
            for (const auto &name : entries)
            {
                cout << name << " ";
            }
            cout << endl;
        }

        // Add a newline between listings of multiple directories
        if (pths.size() > 1 && i < pths.size() - 1)
        {
            cout << endl;
        }
    }
}

//pinfo
void cmndPinfo(const vector<string> &tkns){
    string pStr;
    if(tkns.size() > 1){
        pStr = tkns[1];
    }else{

        pStr = to_string(getpid());
    }
    // 1. Construct the path to the stat file
    string stat_path = "/proc/" + pStr + "/stat";

    // 2. Open the file using C-style fopen
    FILE* stat_file = fopen(stat_path.c_str(), "r");
    if (stat_file == NULL) {
        perror("Error -> perror, while opening the file");
        return;
    }

    // 3. Read the required fields using fscanf
    char state;
    long pgrp, tpgid;
    long long memory;

        // The format string uses %* to read and discard unneeded fields.
    // We need fields: 3(state), 5(pgrp), 8(tpgid), and 23(memory).
    fscanf(stat_file, "%*d %*s %c %*d %ld %*d %*d %ld %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lld",
           &state, &pgrp, &tpgid, &memory);
    
    // 4. Close the file
    fclose(stat_file);
    
    string exe_path_str = "/proc/" + pStr + "/exe";
    char exe_path[4096];
    ssize_t len = readlink(exe_path_str.c_str(), exe_path, sizeof(exe_path) - 1);
    
    string exe_path_display;
    if (len != -1) {
        exe_path[len] = '\0';
        exe_path_display = exe_path;
        string home_path = shellHomeDir;
        if (exe_path_display.rfind(home_path, 0) == 0) {
            exe_path_display.replace(0, home_path.length(), "~");
        }
    } else {
        exe_path_display = "Path not found";
    }

    // 5. Print the formatted output
    // cout << "pid -- " << pStr << endl;
    cout << "Process Status -- " << state;
    if (pgrp == tpgid) {
        cout << "+";   
    }
    cout << endl;
    cout << "memory -- " << memory << " {Virtual Memory}" << endl;
    cout << "Executable Path -- " << exe_path_display << endl;
}

//Search
bool recFun(const string& base_path, const char* target_name){
    DIR* dir = opendir(base_path.c_str());
    if (dir == nullptr) {
        return false;
    }

    struct dirent* e;
    while ((e = readdir(dir)) != nullptr) {
        if (strcmp(e->d_name, target_name) == 0) {
            closedir(dir);
            return true;
        }
        if (e->d_type == DT_DIR) {
            if (strcmp(e->d_name, ".") != 0 && strcmp(e->d_name, "..") != 0) {
                string new_path = base_path + "/" + e->d_name;
                if (recFun(new_path, target_name)) {
                    closedir(dir);
                    return true;
                }
            }
        }
    }
    closedir(dir);
    return false;
}

//search command
void cmndSearch(const vector<string>& tkns) {
    if (tkns.size() != 2) {
        cout << "Error -> in search: incorrect number of arguments." << endl;
        return;
    }
    const char* target_name = tkns[1].c_str();
    bool found = recFun(".", target_name);
    if (found) {
        cout << "True" << endl;
    } else {
        cout << "False" << endl;
    }
}

//for "cd" command
void cmndCd(const vector<string>& cmdToken) {
    // Check for the correct number of arguments.
    if (cmdToken.size() > 2) {
        cout << "Error -> cd: too many arguments" << endl;
        return;
    }

    string pthStr;

    // Determine the target directory path.
    // Case: If only 'cd' or 'cd ~' ->  handles changing to the previous directory
    if (cmdToken.size() == 1 || cmdToken[1] == "~") {
        pthStr = shellHomeDir;
    }
    // Case: If `cd -`
    else if (cmdToken[1] == "-") {
        // if there is no path set for any previous directory.
        if (strlen(shellprevDir) == 0) {
            cout << "There is no path set for old PWD" << endl;
            return;
        }
        pthStr = shellprevDir;
        cout << pthStr << endl; 
    } 
    // Case: If 'cd .', 'cd ..' ,'cd <path>', 'cd ~/path'
    else {
        pthStr = cmdToken[1];
        // Check if the path starts with "~/"
        if (pthStr.rfind("~/", 0) == 0) {
            // Replace the "~" with the home directory path.
            pthStr.replace(0, 1, shellHomeDir);
        }
    }

    // Before changing, get the current directory to update `shellprevDir`.
    char currentDir[4096];
    if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
        perror("Error -> cd getting current directory");
        return;
    }

    // change the directory.
    // chdir -> change the current working directory of a process.
    if (chdir(pthStr.c_str()) != 0) {
        perror(("Error -> cd" ));
    } else {
        // On success
        // update the previous directory variable.
        strcpy(shellprevDir, currentDir);
    }
}
