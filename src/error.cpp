#include <iostream>

void dieWithError(std::string s){
    std::cerr << s.c_str();
    throw s.c_str();
}
