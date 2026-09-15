#include<iostream>
#include<array> // Needed for static array
#include<vector>
#include<string>
#include<algorithm>

#define WAIT std::cin.get() // MACROS

// Why should we use static array - we know the size
void PrintArray(std::array<int, 5>& data){
    for(int i=0;i<data.size();i++);
    std::cout << "PrintArray Func" << std::endl;
}

// Function Pointers
// They are a way to assign a function to a variable


void HelloWorld(){
    std::cout<<"Hello World!"<<std::endl;
}

void PrintValue(int val){
    std::cout << "Value: "<<val<<std::endl;
}

void ForEach(const std::vector<int> &values, void(*func)(int)){
    for(int value:values)
        func(value);
}


void funcPtrs(std::array<int, 5>& data){
    void(*function)() = HelloWorld; // <function return tyoe>(* <function ptr name>)(parameters of iriginal function)
    void(*prtArray)(std::array<int, 5>&) = PrintArray;
    function();
    function();
    prtArray(data);
}

// NAMESPACES in C++

/*
We use namespaces primarily to prevent naming conflicts (collisions) and to organize code into logical groups.

In large software projects or when integrating multiple third-party libraries, it is common for different developers 
to choose the same names for variables, classes, or functions. Without namespaces, this causes a compilation or linker error because the compiler cannot determine which entity you are referring to

An anonymous namespace (also called an unnamed namespace) is a namespace defined without a name

*/

namespace apple{
    void print(const std::string& text){
        std::cout<<text<<std::endl;
    }
}

namespace orange{
    void print(const std::string& text){
        std::string temp = text;
        std::reverse(temp.begin(), temp.end());
        std::cout<<temp<<std::endl;
    }
}

// Note: For the above code, if we weren't using namespaces, it would cause errors
// Since both functions have same definition


// END OF NAMESPACES

int main(){
    std::array<int, 5> data; // static array - an array that does not grow
    data[0] = 5;
    data[4] = 8;
    funcPtrs(data);
    std::vector<int> values = {1,5,4,2,3};
    ForEach(values, PrintValue);

    // lambda function - used to define a function without actually defining a function
    // Used with function pointer
    ForEach(values, [](int value){
        std::cout<<"value: "<<value<<std::endl;
    });

    /*
    int a = 5;
    int b = 10
    */

    auto lambda = [/*a, &b*,=(to pass everything by val)*, &(to pass everything by reference*/](int value){ // The square bracket is used to pass values by reference or by value
        std::cout<<"value: "<<value<<std::endl;
    };

    // Namespaces code
    std::string txt = "Soumava";
    apple::print(txt);
    orange::print(txt);
    
    return 0;
}

