#include "globalsVariables.h"

using namespace std;

// Define the global variables here. This is the only place they are defined.
char shellHomeDir[4096];
char shellCurrDir[4096];
char shellhostName[1024];
char shellprevDir[4096];
vector<string> cmndHist;
string histFilePth;
struct termios ogTermios;
pid_t fgPid = 0;
size_t histInd = 0;