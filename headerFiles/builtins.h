#ifndef BUILTINS_H
#define BUILTINS_H

#include <string>
#include <vector>

void cmndHistory(const std::vector<std::string>& tokens);
void cmndLs(const std::vector<std::string>& tokens);
void cmndPinfo(const std::vector<std::string>& tokens);
void cmndSearch(const std::vector<std::string>& tokens);
void cmndCd(const std::vector<std::string>& cmdToken);

#endif // BUILTINS_H