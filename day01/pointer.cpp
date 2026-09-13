#include<iostream>
#include<string>

// Danging Pointer - A pointer that is pointing to a memeory that is no longer 
//                  and has been deallocated

// Memeory Leak - memory is allocated from heap but the coder forgets to release it using delete;

struct myStruct{
    std::string name;
    int total_marks;
};

void dangling_ptr(){
    myStruct *ptr{ new myStruct };
    ptr->name = "Soumava";
    ptr->total_marks = 95;

    delete ptr; // Now ptr is dangling
    std::cout << ptr->name << ptr->total_marks <<std::endl; // Dangling Read
    delete ptr; // double delete error
}

void mem_leak(){
    myStruct *ptr2{ new myStruct };
    ptr2->name = "Rohit";
    ptr2->total_marks = 90;

    std::cout << ptr2->name << ptr2->total_marks <<std::endl;
}

int main(){
    dangling_ptr();
    mem_leak();
    return 0;
}