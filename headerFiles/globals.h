#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <vector>
#include <termios.h>
#include <unistd.h>

using namespace std;

// Global variables are declared with 'extern' so they are shared across files.
extern char shellHomeDir[4096];
extern char shellCurrDir[4096];
extern char shellhostName[1024];
extern char shellprevDir[4096];
extern vector<std::string> command_history;
extern string history_file_path;
extern struct termios orig_termios;
extern pid_t fgPid;
extern size_t history_index;

#endif // GLOBALS_H