#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <string>
#include <vector>

std::string find_longest_common_prefix(const std::vector<std::string>& matches);
void handle_autocomplete(std::string& line);

#endif // AUTOCOMPLETE_H