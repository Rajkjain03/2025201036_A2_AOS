#ifndef commandsBuiltins_H
#define commandsBuiltins_H

#include <string>
#include <vector>
using namespace std;

void cmndPinfo(const vector<string>& tokens);
void cmndHistory(const vector<string>& tokens);
void cmndLs(const vector<string>& tokens);
void cmndSearch(const vector<string>& tokens);
void cmndCd(const vector<string>& cmdToken);
void cmndEcho(const vector<string>& cmdToken);

#endif // commandsBuiltins_H