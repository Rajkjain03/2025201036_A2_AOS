#include "myshell.h"

using namespace std;

//to handle  zombie process 
void methodSigchld(int signum) {
    // Use a loop with WNOHANG to clean up all terminated children
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

//For ctrl-c -> signal handler for ctrl C
//if there is a foreground process running send the SIGINT signal, else if no foreground process, then ctrl0-c does nothing
void methodSigint(int signum){
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
void methodSigstp(int signum){
    if(fgPid != 0){
        //implies it is a foreground process so sent it the signal
        kill(fgPid,SIGTSTP);
        //reset the foreground process pid
        fgPid = 0;
    }
}
