#include <iostream>
#include <sys/wait.h>
#include <signal.h>
#include "signals.h"
#include "globals.h"
#include "prompt.h"

using namespace std;

// Handler for reaping zombie processes ---
void sigchld_handler(int signum) {
    // Use a loop with WNOHANG to clean up all terminated children
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

//For ctrl-c -> signal handler for ctrl C
//if there is a foreground process running send the SIGINT signal, else if no foreground process, then ctrl0-c does nothing
void sigint_handler(int signum){
    //command is runnig a foreground process
    if(fgPid != 0){
        //implies it is a foreground process so sent it the signal
        kill(fgPid,SIGINT);
        cout << endl;
    }
    //if the prompt is empty i.e no foreground process
    else{
        cout << endl;
        // redisplay the terminale
        promptDisplay();
    }
}

//For ctrl-Z -> signal handler for ctrl z
//if there is a foreground process running send the SIGINT signal, else if no foreground process, then ctrl0-c does nothing
void sigtstp_handler(int signum){
    if(fgPid != 0){
        //implies it is a foreground process so sent it the signal
        kill(fgPid,SIGTSTP);
        //reset the foreground process pid
        fgPid = 0;
    }
}
