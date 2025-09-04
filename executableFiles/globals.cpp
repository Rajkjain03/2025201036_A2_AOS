#include "globals.h"

using namespace std;

// Define the global variables here. This is the only place they are defined.
char shellHomeDir[4096];
char shellCurrDir[4096];
char shellhostName[1024];
char shellprevDir[4096];
std::vector<std::string> command_history;
std::string history_file_path;
struct termios orig_termios;
pid_t fgPid = 0;
size_t history_index = 0;