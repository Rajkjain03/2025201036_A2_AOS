#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <string>
#include <vector>
using namespace std;

//declaring functions
string findLcsPrefix(const vector<string>& matches);
void autoCmpltFunctionHandler(string& line);

#endif // AUTOCOMPLETE_H