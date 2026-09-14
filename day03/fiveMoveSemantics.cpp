#include<iostream>
#include<string.h>

// lvalues + rvalues
// “l-value” refers to a memory location that identifies an object. 
// "r-value” refers to the data value that is stored at some address in memory.

void lvalue(std::string &name){
    std::cout << "[lvalue]" << name << std::endl;
    return;
}

void rvalue(std::string &&name){
    std::cout << "[rvalue]" << name << std::endl;
    return;
}

void bothvalue(const std::string &name){
    std::cout << "[bothvalue]" << name << std::endl;
    return;
}

void driver(){
    std::string fName = "Soumava";
    std::string lName = "Das";

    std::string fullName = fName + lName;
    lvalue(fullName);
    // lvalue(fName + lName); <- This gives error

    rvalue(fName + lName);
    // rvalue(fullName); <- This gives error

    bothvalue(fullName);
    bothvalue(fName+lName);
    return;
}

// Move Semantics

/*

Move semantics is a feature that allows our program to transfer ownership of resources 
(like memory, files, etc.) from one object to another instead of copying them. 
This results in:
    Faster performance
    Less memory usage
    Better efficiency, especially with big objects (like std::vector, std::string, or file streams)

Normally when we copy an object that owns a resource, both objects must mange separate copies of that resource.
But copying is expensive - especially for big data.

With move semantics we can transfer the resource from one object to another, 
leaving the first object in the "moved-from" state which means it's still a valid object 
but it no longer owns the original resource (like text in a string or data in a vector).

*/

class String{
    private:
        char *m_Data;
        uint32_t m_Size;
    public:
        String() = default;
        String(const char* string){
            printf("Created!\n");
            m_Size = strlen(string);
            m_Data = new char[m_Size];
            memcpy(m_Data, string, m_Size);
        }
        // This Copy is a problem, since to create a string we need to allocated memory on the heap.
        String(const String& other){    
            printf("Copy Constructor!\n");
            m_Size = other.m_Size;
            m_Data = new char[m_Size];
            memcpy(m_Data, other.m_Data, m_Size);
        }

        // Move Constructor
        // like copy constructor but takes and rvalue reference
        String(String && other ) noexcept{
            printf("Move Constructor!\n");
            m_Size = other.m_Size;
            m_Data = other.m_Data; // instead of copying every data, we are simply assigning the pointer
            
            // we are taking control of the "other" string, so we have to take care of it, 
            // since it also might get deleted on destructor call 
            // Now other is an hollow object
            other.m_Size = 0;
            other.m_Data = nullptr;
        }
        
        // Move assignment operator
        String& operator=(String&& other) noexcept{
            printf("Move Assignment!\n");

            // delete is required because, currently, if I already have some data
            // I want to overwrite it, so I call delete since current object might have some memory allocated
            if(this != &other){ // This is to be called when they are different objects
                delete[] m_Data;

                m_Size = other.m_Size;
                m_Data = other.m_Data;

                other.m_Size = 0;
                other.m_Data = nullptr;
            }
            return *this;
        }

        ~String(){
            printf("Destroyed!\n");
            delete[] m_Data;
        }

        void PrintName(){
            for(uint32_t i=0;i<m_Size;i++)
                printf("%c", m_Data[i]);
            printf("\n");
        }
};

class Entity{
    private:
    String m_Name;
    public:
        Entity(const String &name) : m_Name(name){} // this is for copy constructor
        Entity(String &&name): m_Name((String &&)name){} // this is for move constructor - to use move constructor, we have to explicitly cast it
        void Print(){
            m_Name.PrintName();
        }
};

// Move assignment operator - We use it when we want to move an object into another object



int main(){
    Entity et2("Soumava");
    et2.Print();

    String s1 = "Hello";
    // We casted s1, such that it invokes move constructor
    String dest = (String &&)s1; // but this is not an elegant way

    // better way is the following
    String dest2(std::move(s1));

    return 0;
}