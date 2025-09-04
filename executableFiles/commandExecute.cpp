#include "myshell.h"

using namespace std;

void executeCommand(const vector<string>& cmdToken) {

    //  EXECUTE THE CLEANED COMMAND
    if (cmdToken.empty()) {
        exit(EXIT_SUCCESS);
    } // Nothing to execute after redirection parsing

    // pwd -> printing present working directory
    if (cmdToken[0] == "pwd")
    {
        char curDir[4096];
        // getcwd() ->  is a system call that gets the absolute path of the current working directory and copies it into the curDir buffer.
        if (getcwd(curDir, sizeof(curDir)) != NULL)
        {
            cout << curDir << endl;
        }
        else
        {
            perror("Error -> pwd");
            
        }
        //When shell runs a command in a pipeline pwd is executed in a child process. 
        // exit() -> call ensures that the child process terminates immediately after printing its output, 
        exit(EXIT_SUCCESS); 
    }

    // echo -> prints arguments to console.
     if (cmdToken[0] == "echo")
    {   
        cmndEcho(cmdToken);
        return ;
    }
    
    // cd -> change current working directory
    // cd or cd~ -> will take you back to your home directory.
    // cd . -> does nothing instructs your system to change the current directory to the current directory.
    // cd .. -> moves you one level up in the directory hierarchy.
    // cd - -> moves you to previous/last directory
     if (cmdToken[0] == "cd")
    {   

        cmndCd(cmdToken);
        
        return;
    }

    // ls -> list files and directories. list the content of the current directoruy.
    // ls -l -> provides detailed information, including permissions, number of links, owner, group, size, and last-modified time.
    // ls -a -> shows all files including hidden files
    // ls . -> same as ls
    // ls .. -> list all files in parent directory
    // ls ~ -> ists the files and directories home directory.
     if (cmdToken[0] == "ls")
    {
        cmndLs(cmdToken);
        exit(EXIT_SUCCESS); 
    }
    

    //pinfo
     if (cmdToken[0] == "pinfo"){
        cmndPinfo(cmdToken);
        exit(EXIT_SUCCESS); 
    }

    //search
     if (cmdToken[0] == "search")
    {
        cmndSearch(cmdToken);
        exit(EXIT_SUCCESS); 
    }

    //handleHistory
    if (cmdToken[0] == "history") {
        cmndHistory(cmdToken);
        exit(EXIT_SUCCESS);
    }

    // `exit` command
    if (cmdToken[0] == "exit")
    {   cout << "\n GG ! Adiós ." << endl;
        exit(0);
        
    }

    // Handle other cmnds -> using execvp()
    // child process creation
    vector<char *> c_args;
    for (const auto &t : cmdToken) {
        c_args.push_back(const_cast<char *>(t.c_str()));
    }
    c_args.push_back(nullptr);

    if (execvp(c_args[0], c_args.data()) == -1) {
        perror(("execvp failed for " + cmdToken[0]).c_str());
        exit(EXIT_FAILURE); // Critically, child must exit on failure
    }
}


//  Single command executor (handles redirection) 
void execSingCmnd(const vector<string>& tkns) {
    if (tkns.empty()) return;

    vector<string> cmdToken;
    string inputFile, outputFile;
    bool append_mode = false;

    for (size_t i = 0; i < tkns.size(); ++i) {
        if (tkns[i] == "<") { if (++i < tkns.size()) inputFile = tkns[i]; }
        else if (tkns[i] == ">") { if (++i < tkns.size()) { outputFile = tkns[i]; append_mode = false; } }
        else if (tkns[i] == ">>") { if (++i < tkns.size()) { outputFile = tkns[i]; append_mode = true; } }
        else { cmdToken.push_back(tkns[i]); }
    }
    if (cmdToken.empty()) return;

    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);

    if (!inputFile.empty()) {
        int in_fd = open(inputFile.c_str(), O_RDONLY);
        if (in_fd < 0) { perror(("input file error: " + inputFile).c_str()); return; }
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }
    if (!outputFile.empty()) {
        int flags = O_WRONLY | O_CREAT | (append_mode ? O_APPEND : O_TRUNC);
        int out_fd = open(outputFile.c_str(), flags, 0644);
        if (out_fd < 0) { perror(("output file error: " + outputFile).c_str()); return; }
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
    }

    // Call the final execution logic (your original function)
    executeCommand(cmdToken);

    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdin);
    close(saved_stdout);
}


//  Main execution function that handles pipes 
// Pipe handler 
void execPipedCmnd(const string& command_line) {
    stringstream cmndStrm(command_line);
    string pipeSgmnt;
    vector<vector<string>> cmnds;

    // Split the command line by the pipe '|' delimiter
    while (getline(cmndStrm, pipeSgmnt, '|')) {
        stringstream sgmntStrm(pipeSgmnt);
        vector<string> tkns;
        string token;
        while (sgmntStrm >> token) {
            tkns.push_back(token);
        }
        if (!tkns.empty()) {
            cmnds.push_back(tkns);
        }
    }

    if (cmnds.empty()) return;
    // If there's only one command, no piping is needed.
    // Execute it with I/O redirection handling.
    if (cmnds.size() == 1) {
        execSingCmnd(cmnds[0]);
        return;
    }

    // PIPING LOGIC 
    size_t num_pipes = cmnds.size() - 1;
    vector<int> pipeFds(2 * num_pipes);

    // creating all the pipes upfront
    for (size_t i = 0; i < num_pipes; ++i) {
        if (pipe(pipeFds.data() + i * 2) < 0) {
            perror("pipe");
            return;
        }
    }

    // Launch all the child processes
    for (size_t i = 0; i < cmnds.size(); ++i) {
        pid_t pid = fork();
        if (pid == 0) { // Child Process
            // If not the first command, redirect stdin from the previous pipe
            if (i > 0) {
                dup2(pipeFds[(i - 1) * 2], STDIN_FILENO);
            }
            // If not the last command, redirect stdout to the next pipe
            if (i < cmnds.size() - 1) {
                dup2(pipeFds[i * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipe fds in the child
            for (size_t j = 0; j < 2 * num_pipes; ++j) {
                close(pipeFds[j]);
            }

            // Execute the command (this function will handle I/O redirection for the specific command)
            execSingCmnd(cmnds[i]);
            exit(EXIT_FAILURE); // Should not be reached if execvp succeeds
        } else if (pid < 0) {
            perror("fork");
            return;
        }
    }

    // Parent closes all pipe fds
    for (size_t i = 0; i < 2 * num_pipes; ++i) {
        close(pipeFds[i]);
    }

    // Parent waits for all children to complete
    for (size_t i = 0; i < cmnds.size(); ++i) {
        wait(NULL);
    }
}


void execCmndLn(string& line) {
    if (line.empty()) return;

    // First, tokenize the line to check the command.
    stringstream temp_stream(line);
    vector<string> tokens;
    string token;
    while (temp_stream >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) return;

    //  
    if (tokens[0] == "exit") {
        cout << "\n GG ! Adios." << endl;
        exit(0);
    }

    // If the command is a simple 'cd' (not in a pipe), execute it directly.
    // We check for '|' in the original line to ensure it's not a pipeline.
    if (tokens[0] == "cd" && line.find('|') == string::npos) {
        cmndCd(tokens);
        return; 
    }   

    bool isBkgd = false;
    // Trim trailing whitespace and check for '&'
    size_t pos = line.find_last_not_of(" \t");
    if (pos != string::npos && line[pos] == '&') {
        isBkgd = true;
        line.erase(pos);
    }

    // Fork once to create a command manager process
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) { // Child Process
        // make child processes respond to signalHandler by defauld
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        // This child will set up pipes and run the command(s)
        execPipedCmnd(line);
        exit(EXIT_SUCCESS); // Child exits after its job is done
    } else { // Parent process
        if (!isBkgd) {
            // Foreground: wait for the command manager to finish
            fgPid = pid;
            int status;
            // WUNTRACED allows waitpid to return if child is stopped
            waitpid(pid, &status, WUNTRACED);
            //reset
            fgPid = 0;

        } else {
            // Background: print PID and continue
            cout << "[" << pid << "]" << endl;
        }
    }
}
