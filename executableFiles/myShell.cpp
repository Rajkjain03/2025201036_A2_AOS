#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "prompt.h"
#include "signals.h"
#include "history.h"
#include "autocomplete.h"
#include "execute.h"

using namespace std;

void dRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// Function to enable non-canonical ("raw") input mode ---
void eRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    // atexit() ensures dRawMode is called automatically when the program exits
    atexit(dRawMode);
    struct termios raw = orig_termios;
    // Turn off echo (so we can print characters manually) and
    // canonical mode (so we get input char-by-char)
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


int main()
{
    // Register the signal handlers at startup ---
    // The main shell process will now run our custom functions for these signals.
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    signal(SIGCHLD, sigchld_handler); // For reaping zombies

    //  Load history and register the save function to run on exit 
    load_history();
    atexit(save_history);

    //display some cool text on execution of shell
    customShellDisp();
    //setting up the home directory i.e directory where shell is launched becomes its "home" and should be represented by ~
    //getcwd() -> gets the current working directory.
    if (getcwd(shellHomeDir, sizeof(shellHomeDir)) == NULL) {
        perror("Error: Failed to get shell's home directory");
        return 1;
    }
    strcpy(shellprevDir, "");

    eRawMode();
    string ip;

    while (true)
    {
        promptDisplay();
        ip.clear();
        history_index = command_history.size(); // Reset history navigation index

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
                        if (history_index > 0) {
                            history_index--;
                            ip = command_history[history_index];
                        }
                    } else if (seq[1] == 'B') { // Down Arrow
                        if (history_index < command_history.size()) {
                            history_index++;
                            if (history_index == command_history.size()) {
                                ip.clear(); // Go back to a blank line
                            } else {
                                ip = command_history[history_index];
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
                    handle_autocomplete(ip);
                    cout << "\r"; promptDisplay(); cout << ip <<  "\x1b[K"; fflush(stdout);
                } else if (c == 127 || c == 8) { // Backspace key
                    if (!ip.empty()) {
                        ip.pop_back();
                        cout << "\b \b"; fflush(stdout);
                    }
                }
                // --- THIS IS THE CORRECT CTRL-D FIX ---
                else if (c == 4) { // CTRL-D character
                    if (ip.empty()) {
                        // If the line is empty, it's our signal to exit.
                        dRawMode();
                        cout << "\n GG ! BYE." << endl;
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
                history_index = command_history.size();
            }
        }
        
        // --- Restore terminal before executing any command ---
        dRawMode();

        if (!ip.empty()) {
            // Add the successfully entered command to history ---
            add_to_history(ip);
            stringstream cmdSream(ip);
            string snglCmd;
            while (getline(cmdSream, snglCmd, ';')) {
                execute_command_line(snglCmd);
            }
        }
        
        // --- Re-enable raw mode to read the next command ---
        eRawMode();
    }

    // This is a fallback, but the atexit handler is the primary mechanism
    dRawMode();
    cout << "\n GG ! BYE ." << endl;
    return 0;

}