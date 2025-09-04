#include "myshell.h"

using namespace std;

// To print file permisions
void printPerms(mode_t m)
{   
    //print the drwxr-xr-x string
    cout << (S_ISDIR(m) ? 'd' : (S_ISLNK(m) ? 'l' : '-'));
    cout << ((m & S_IRUSR) ? 'r' : '-');
    cout << ((m & S_IWUSR) ? 'w' : '-');
    cout << ((m & S_IXUSR) ? 'x' : '-');
    cout << ((m & S_IRGRP) ? 'r' : '-');
    cout << ((m & S_IWGRP) ? 'w' : '-');
    cout << ((m & S_IXGRP) ? 'x' : '-');
    cout << ((m & S_IROTH) ? 'r' : '-');
    cout << ((m & S_IWOTH) ? 'w' : '-');
    cout << ((m & S_IXOTH) ? 'x' : '-');
}

// History
void cmndHistory(const vector<string>& tkns) {
    // Default number of commands to show
    int nShow = 10; 
    if (tkns.size() > 1) {
        try {
            //stoi -> string to integer 
            nShow = stoi(tkns[1]);
        } catch (const invalid_argument& e) {
            cout << "Error -> History" << endl;
            return;
        }
    }

    // Calculates the correct starting point in the history vector
    int startInd = max(0, (int)cmndHist.size() - nShow);
    for (size_t i = startInd; i < cmndHist.size(); ++i) {
        cout << "\t" << cmndHist[i] << endl;
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
void cmndPinfo(const vector<string>& tkns) {
    //determine the target pid
    string pStr;
    //user provided an argument
    if(tkns.size() > 1){
        //case : pinfo <pid>
        //Use the pid provided by the user
        pStr = tkns[1];
    }
        // no argument is given
    else{
        // getpid() -> Process ID of your shell program 
        // use shell's own PID
        pStr = to_string(getpid());
    }

    // constructs the file path to a specific process's status file within Linux's /proc filesystem
    // procfs -> a special virtual filesystem in Linux that provides real-time information about running processes.
    // /stat -> stores name of a specific file inside each process's directory that contains detailed status information.
    string stPth = "/proc/" + pStr + "/stat";

    // Open the file using open() system call
    //open() -> on success Returns a small, non-negative integer—the file descriptor.
    int fd = open(stPth.c_str(), O_RDONLY);
    if (fd == -1) {
        perror(("Error -> pinfo: cannot open" ));
        return;
    }

    // Read the entire file into a buff
    char buff[2048];
    //read() -> read raw bytes from a file descriptor
    ssize_t bRead = read(fd, buff, sizeof(buff) - 1);
    close(fd);
    if (bRead <= 0) return;
    // terminate the string with null
    buff[bRead] = '\0'; 

    // Parse the buff using strtok()
    // strtok() ->used to split a string into a series of smaller strings, or "tokens," based on a set of delimiter characters.
    char *token = strtok(buff, " ");
    int cnt = 1;
    string state, pgrp, tpgid, memory;

    while (token != NULL) {
        if (cnt == 3) state = token;
        else if (cnt == 5) pgrp = token;
        else if (cnt == 8) tpgid = token;
        else if (cnt == 23) memory = token;
        
        token = strtok(NULL, " ");
        cnt++;
    }

    // exePthStr -> it's a link that points to the actual executable
    string exePthStr = "/proc/" + pStr + "/exe";
    char exePth[4096];
    // readlink() -> read the destination of a link
    ssize_t len = readlink(exePthStr.c_str(), exePth, sizeof(exePth) - 1);
    
    //formating -.> checking if its inside shell's home directory and replacing that part with a ~ 
    string exePthDisp;
    if (len != -1) {
        exePth[len] = '\0';
        exePthDisp = exePth;
        string hPth = shellHomeDir;
        if (exePthDisp.rfind(hPth, 0) == 0) {
            exePthDisp.replace(0, hPth.length(), "~");
        }
    } else {
        exePthDisp = "Path not found";
    }

    //Printing Process Status,memory ,"Executable Pat
    cout << "Process Status -- " << state;
    if (pgrp == tpgid) {
        cout << "+";
    }
    cout << endl;
    cout << "memory -- " << memory << " {Virtual Memory}" << endl;
    cout << "Executable Path -- " << exePthDisp << endl;
}

//recursive function to look through a directory and calling itself on any subdirectories it finds. 
bool recFun(const string& basePath, const char* trgtName){
    //opendir -> opens a directory and returns a pointer to a directory stream
    DIR* dir = opendir(basePath.c_str());
    if (dir == nullptr) {
        return false;
    }

    struct dirent* e;
    //  readdir(dir) -> reads the next entry from a directory stream, which you get by calling opendir()
    while ((e = readdir(dir)) != nullptr) {
        //check if we find the file
        if (strcmp(e->d_name, trgtName) == 0) {
            closedir(dir);
            return true;
        }
        //recursion check if it is subdirectry
        if (e->d_type == DT_DIR) {
            // Check if '.' and '..' ignore it.
            if (strcmp(e->d_name, ".") != 0 && strcmp(e->d_name, "..") != 0) {
                string nPath = basePath + "/" + e->d_name;
                if (recFun(nPath, trgtName)) {
                    closedir(dir);
                    return true;
                }
            }
        }
    }
    //found nothing in thsi directory.
    closedir(dir);
    return false;
}

//search command
void cmndSearch(const vector<string>& tkns) {
    //check for the argument it should be 2. e.g - (search xyz.txt)
    if (tkns.size() != 2) {
        cout << "Error -> in search: incorrect number of arguments." << endl;
        return;
    }

    const char* trgtName = tkns[1].c_str();
    // recursive search from the current directory "."
    bool found = recFun(".", trgtName);
    if (found) {
        cout << "True" << endl;
    } else {
        cout << "False" << endl;
    }
}


void cmndEcho(const vector<string>& cmdToken){
    //traverse 
    for (auto it = cmdToken.begin() + 1; it != cmdToken.end(); ++it)
        {
            string tknToPrnt = *it;

            // Case : Double Qoutes -> Check if the token starts and ends with a double quote.
            if (!tknToPrnt.empty() && tknToPrnt.front() == '"' && tknToPrnt.back() == '"') {
                // Reomove the double quotees
                // substring excluding double quotees
                tknToPrnt = tknToPrnt.substr(1, tknToPrnt.length() - 2);
            }
            
            //print the token.
            cout << tknToPrnt;

            //add spaces if not lst elements
            if (next(it) != cmdToken.end()) {
                cout << " ";
            }
        }
        cout << endl;
        exit(EXIT_SUCCESS);
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
