#include<iostream>
#include<fstream>
#include<string>

/*
Useful class for working with files in C++:
1. fstreambase
2. ifstream -> derived from streambase
3. ofstream -> derived from streambase
*/

// @ ways to open files
// 1. Using constructor
// 2. Using member function open() of class

int main(){
    std::string st = "Soumava is an engineer";
    std::string st2;
    // Opening files using constructor
    std::ofstream out("sample.txt");

    // Write operation
    out<<st;


    // Opening file using constructor and reading operation
    std::ifstream in("sample2.txt");

    // Read operation
    // in>>st2; <- for only 1 word until blank space
    getline(in, st2); // take the full line instead of 1 word until blank
    std::cout<<st2;
    return 0;
}
