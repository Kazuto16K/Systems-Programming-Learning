#include<iostream>
#include<string>

// Danging Pointer - A pointer that is pointing to a memeory that is no longer 
//                  and has been deallocated

// Memeory Leak - memory is allocated from heap but the coder forgets to release it using delete;

struct myStruct{
    std::string name;
    int total_marks;
};

void dangling_ptr_fix(){
    myStruct *ptr{ new myStruct };
    ptr->name = "Soumava";
    ptr->total_marks = 95;

    
    std::cout << ptr->name << ptr->total_marks <<std::endl; // NO DANGLING READ
    delete ptr; // Now ptr is freed after use.
    // DOUBLE DELETE REMOVED
}

void mem_leak_fix(){
    myStruct *ptr2{ new myStruct };
    ptr2->name = "Rohit";
    ptr2->total_marks = 90;

    std::cout << ptr2->name << ptr2->total_marks <<std::endl;
    delete ptr2; // FIX ADDED TO FREE MEMORY
}

int main(){
    dangling_ptr_fix();
    mem_leak_fix();
    return 0;
}