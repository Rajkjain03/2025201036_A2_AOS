# Assignment 2: POSIX Shell Implementation

Implemented custom interactive shell, with core functionalities of standard shells like bash 
    
Roll No. - 2025201036
Name - Raj k jain  

### TO EXECUTE -
1. Compile the Project :
```bash
#compile 
        make clean && make
```

2. Run the Shell:
```bash
#Run 
        ./myshell
```

3. Clean
```bash
#Clean
         make clean
```
        
### Project Structure
    2025201036_A2
        |___executableFiles
                |__autocomplete.cpp 
                |___commandsBuiltins.cpp 
                |___execute.cpp 
                |___globals.cpp 
                |___history.cpp 
                |___myShell.cpp 
                |___prompt.cpp 
                |___signalHandler.cpp 

         |___headerFiles   
                |__autocomplete.h 
                |___commandsBuiltins.h 
                |___execute.h 
                |___globals.h 
                |___history.h 
                |___myShell.h 
                |___prompt.h 
                |___signalHandler.h    

        |____obj

        |____README.md

### Complete Code Review
1. Prompt - Dynamic prompt updates dynamically:
Format - <user@system:path>
Approach - Using the functionality of getlogin(), gethostname(), and getcwd() and also
identifies the shell's home directory (shellHomeDir) and replaces the path with ~ when appropriate.
a. set up  the home directory
b. diplay prompt 

2. Built-ins commands - cd, echo, pwd, ls, pinfo, history, search, exit.
a. pwd -> using getcwd() gets the absolute path of the current working directory.
b. echo -> prints its arguments back to the screen by iteratating loop through the vector of tokens
c. cd -> in this multiple cases has been handled for different flags like “.”, “..”, “-” and “~”.
d. ls -> Handle the flags(-a, -l) and directory names
e. pinfo -> Displays process information by manually reading and parsing files from the /proc filesystem.
f. history ->
g. search -> 

3. Pipes and Redirection: Handles piping and I/O redirection.
 
4. Signal Handling: Handle the simple signals like CTRL-C (interrupts foreground job) and CTRL-Z (stops and backgrounds job) without terminating the shell, and logs out on CTRL-D.

5. History : Saving history which will be lloaded on startup. with up/down key navigation.

6. Autocomplete : Completion of command by pressing TAB by searching path and local files. 


### Code Structure Explained
The shell is broken down into several modular components. wtih each with specific responsibility.
myShell.cpp
Responsibility: The entry point and main loop of the shell.
Details: this file is responsible for initializing the shell ,handling the primary character-by-charactr input loop, and managing the overall progrm flow. 

src/globals.cpp
Responsibility: Defines all global variables.
Details: This file provides the actual storage for all global variables declared in include/globals.h 

src/prompt.cpp
Responsibility: Manages the display of the shell prompt.
Details: Contains the promptDisplay() function, which is called before the user enters each new comand. It dynamically gathers the username, system name, and current path.

src/signals.cpp
Responsibility: Handles all signal logic.
Details: This file contains the signal handler functions for (CTRL-C), (CTRL-Z), and (CTRL-D). These functions define the custom actions the shell takes when it recives a signal, such as interupting a foreground process or cleaning up terminated background ("zombie") processes.

src/history.cpp
Responsibility: Manages command history.
Details: Contains all logic related to the command history feature, including loaddHist() , saveHist() (to write back to the file on exit), and addToHis() (to add new commands to the in-memory list).

src/autocomplete.cpp
Responsibility: Implements the TAB key autocomplete functionality.
Details: This file contains the logic for autocompletion. The autoCmpltFunctionHandler function determines what the user is currently typing, searches either the system PATH for commands or the local directory for files, and then performs the completion or displays the available options.

src/builtins.cpp
Responsibility: Contains the implementation for all shell built-in commands.
Details: commands like ls, cd, pinfo, history, echo, pwd, and search implemented. 

src/execute.cpp
Responsibility: The core command execution engine.
Details: It contains the set of functions that work together to handle parsing, backgrounding (&), piping (|), I/O redirection (<, >), and the execution of both built-in and system commands.



