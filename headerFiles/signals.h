#ifndef SIGNALS_H
#define SIGNALS_H

void sigchld_handler(int signum);
void sigint_handler(int signum);
void sigtstp_handler(int signum);

#endif // SIGNALS_H