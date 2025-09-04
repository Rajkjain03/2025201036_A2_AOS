#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <vector>
#include <termios.h>
#include <unistd.h>

using namespace std;

// Global variables 
// declared with 'extern' so they are shared across files.
extern char shellHomeDir[4096];
extern char shellCurrDir[4096];
extern char shellhostName[1024];
extern char shellprevDir[4096];
extern vector<string> cmndHist;
extern string histFilePth;
extern struct termios ogTermios;
extern pid_t fgPid;
extern size_t histInd;

#endif // GLOBALS_H