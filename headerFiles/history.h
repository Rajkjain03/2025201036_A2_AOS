#ifndef HISTORY_H
#define HISTORY_H

#include <string>
using namespace std;

void loaddHist();
void saveHist();
void addToHis(const string& command);

#endif // HISTORY_H