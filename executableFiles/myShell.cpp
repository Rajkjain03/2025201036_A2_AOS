#include "myshell.h"

using namespace std;

void dRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ogTermios);
}

// Function to enable non-canonical ("raw") input mode 
void eRawMode() {
    tcgetattr(STDIN_FILENO, &ogTermios);
    // atexit() ensures dRawMode is called automatically when the program exits
    atexit(dRawMode);
    struct termios raw = ogTermios;
    // Turn off echo (so we can print characters manually) and
    // canonical mode (so we get input char-by-char)
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main()
{
    // Register the signal handlers at startup 
    // The main shell process will now run our custom functions for these signalHandler.
    signal(SIGINT, methodSigint);
    signal(SIGTSTP, methodSigstp);
    signal(SIGCHLD, methodSigchld); 

    //  Load history and register the save function to run on exit 
    loaddHist();
    //atexit() ->  takes a function pointer as an argument. It adds the specified function to a list of functins to be called upon normal program termination.
    atexit(saveHist);

    //display some cool text with shell name on execution of shell
    customShellDisp();
    //setting up the home directory i.e directory where shell is launched becomes its "home" and should be represented by ~
    //getcwd() -> gets the current working directory.
    if (getcwd(shellHomeDir, sizeof(shellHomeDir)) == NULL) {
        perror("Error ->  Failed to get shell's home directory");
        return 1;
    }
    strcpy(shellprevDir, "");

    eRawMode();
    string ip;

    while (true)
    {   
        //fuction to display prompt
        promptDisplay();
        ip.clear();
        histInd = cmndHist.size(); // Reset history navigation index

        char c;
        // This is the final, robust input loop for raw mode.
        while (read(STDIN_FILENO, &c, 1) == 1) {
            // Arrow key sequence detection 
            if (c == '\x1b') { // ESC character
                char seq[2];
                // Try to read the next two characters of the sequence
                if (read(STDIN_FILENO, &seq[0], 1) != 1) continue;
                if (read(STDIN_FILENO, &seq[1], 1) != 1) continue;

                if (seq[0] == '[') {
                    if (seq[1] == 'A') { // Up Arrow
                        if (histInd > 0) {
                            histInd--;
                            ip = cmndHist[histInd];
                        }
                    } else if (seq[1] == 'B') { // Down Arrow
                        if (histInd < cmndHist.size()) {
                            histInd++;
                            if (histInd == cmndHist.size()) {
                                ip.clear(); // Go back to a blank line
                            } else {
                                ip = cmndHist[histInd];
                            }
                        }
                    }
                    // Redraw the line after an arrow key press
                    // Move cursor to the beginning of the line.
                    cout << "\r"; 
                    //reprint
                    promptDisplay();
                    //Print the new command from history
                    cout << ip;
                    //ERASE from the cursor to the end of the line.
                    cout << "\x1b[K"; 
                    //Flush the output to make it appear instantly.
                    fflush(stdout);
                }
                continue; // Skip the rest of the loop for this sequence
            }

            if (iscntrl(c)) {
                if (c == '\n') { // Enter key
                    cout << "\r\n";
                    break; // Exit input loop to execute command.
                } else if (c == '\t') { // TAB key
                    autoCmpltFunctionHandler(ip);
                    cout << "\r"; promptDisplay(); cout << ip <<  "\x1b[K"; fflush(stdout);
                } else if (c == 127 || c == 8) { // Backspace key
                    if (!ip.empty()) {
                        ip.pop_back();
                        cout << "\b \b"; fflush(stdout);
                    }
                }
                //  THIS IS THE CORRECT CTRL-D FIX 
                else if (c == 4) { // CTRL-D character
                    if (ip.empty()) {
                        // If the line is empty, it's our signal to exit.
                        dRawMode();
                        cout << "\n GG ! Adiós." << endl;
                        exit(EXIT_SUCCESS);
                    }
                    // If line is not empty, CTRL-D does nothing.
                }
            } else { // It's a regular, printable character
                ip += c;
                cout << c;
                fflush(stdout);
                // As soon as the user types, reset history navigation for this line
                // starts from the most recent command.
                histInd = cmndHist.size();
            }
        }
        
        //  Restore terminal before executing any command 
        dRawMode();

        if (!ip.empty()) {
            // Add the successfully entered command to history 
            addToHis(ip);
            stringstream cmdSream(ip);
            string snglCmd;
            while (getline(cmdSream, snglCmd, ';')) {
                execCmndLn(snglCmd);
            }
        }
        
        // Re-enable raw mode to read the next command 
        eRawMode();
    }

    // This is a fallback, but the atexit handler is the primary mechanism
    dRawMode();
    cout << "\n GG ! Adiós ." << endl;
    return 0;

}