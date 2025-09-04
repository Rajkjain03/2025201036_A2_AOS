#ifndef HISTORY_H
#define HISTORY_H

#include <string>

void load_history();
void save_history();
void add_to_history(const std::string& command);

#endif // HISTORY_H