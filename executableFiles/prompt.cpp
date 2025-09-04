#include <iostream>
#include <unistd.h>
#include "prompt.h"
#include "globals.h"


using namespace std;

//some cool fonts to display when first executed 
void customShellDisp()
{   //reset to default style
    const string RESET = "\033[0m";
    // Text Styles
    const string BOLD = "\033[1m";
    const string BOLD_CYAN = "\033[1;36m";

    cout << BOLD_CYAN << " ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒ " << BOLD_CYAN << BOLD << "MyShell" << BOLD_CYAN << "▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒ " << RESET << " " << endl;
    cout << endl;
}

//For displaying the  <_username_@_system_name_:_current_directory_>
void promptDisplay()
{
    // get Current Working directory.
    // getcwd -> Find the full path of the directory. stands for get curent working directory.
    if (getcwd(shellCurrDir, sizeof(shellCurrDir)) == NULL)
    {   
        // perro  -> prints a descriptive error message
        perror("Error -> getcwd");
        return;
    }

    // get System name and user name
    // getlogin() -> gets the username of the user logged in.
    char *usrName = getlogin();
    if (usrName == NULL)
    {   
        perror("Error -> getlogin");
        return;
    }

    // gethostname() -> retrve the hostname of computer.
    int res = gethostname(shellhostName, sizeof(shellhostName));
    if (res != 0)
    {
        perror("Error -> gethostname");
        return;
    }

    //temporary variables 
    string path = shellCurrDir;
    string homePath = shellHomeDir;

    // ~ logic
    // Check if Current directory(path) starts with homePath
    // shortens file path by replacing the user's home directory portion with a tilde (~) 
    // path.rfind(homePath, 0) -> search for the homePath string inside the path string
    if (path.rfind(homePath, 0) == 0 && !homePath.empty())
    {   
        //modifies the path i.e replace the no. of character equal to home path size with (~).
        path.replace(0, homePath.length(), "~");
    }

    // Displaying the complete prompt
    cout << usrName << "@" << shellhostName << ":" << path << ">";
    cout.flush();
}
