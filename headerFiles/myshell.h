#ifndef MYSHELL_H
#define MYSHELL_H

#include <iostream>         //  input/output (cout, cin)
#include <vector>           //  for using the vector 
#include <string>           //  for using the string clas
#include <sstream>          //  using using stringstream 
#include <algorithm>        //  sort (used in ls) and max (used in history)
#include <fstream>          //  file input/output streams (ifstream, ofstream) used in the history command
#include <iomanip>          //  I/O manipulators like setw() to format 'ls -l' output
#include <string.h>         //  strcpy()
#include <stdlib.h>         //  getenv() (to find PATH) and atexit() (to save history)
#include <time.h>           //  formatting timestamps in 'ls -l'
#include <unistd.h>         //  fork, execvp, getcwd, chdir, pipe, dup2, read, etc.
#include <sys/wait.h>       //  for waitpid() 
#include <dirent.h>         //  opendir(), readdir(), closedir() used in 'ls' and 'search'
#include <sys/stat.h>       //  for geting file metadata with stat() and lstat(), essential for 'ls -l'
#include <pwd.h>            //  converting user IDs to usernames (getpwuid) in 'ls -l'
#include <grp.h>            //  converting group IDs to group names (getgrgid) in 'ls -l'
#include <fcntl.h>          //  open() ->  I/O redirection
#include <signal.h>         //  all signal handling functions: signal() and kill()
#include <termios.h>        //  terminal I/O settings (tcgetattr, tcsetattr) to enable raw mode for autocomplete
#include "globalsVariables.h"            //  declaring shared global variables
#include "displayPromp.h"             //  display the shell prompt
#include "signalHandler.h"      //  signal handling
#include "handleHistory.h"            //  handle history
#include "autocomplete.h"       //  TAB autocompete logic
#include "commandsBuiltins.h"   //  implementations of all built-in commands (ls, cd, pinfo, etc.)
#include "commandExecute.h"            //  handles pipes, redirection, and backgrounding

#endif // MYSHELL_H