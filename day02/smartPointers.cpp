#include<iostream>
#include<memory> // for unqiue ptr and shared ptr

// smartpointers - sometimes we forget to delete a pointer using delete. But smart pointers
// automatically deallocate memory when going out of scope

// RAII = Resource Acquisition is Initialization (lifetime of resource is tied to its scope)

struct Resource{
    std::string name;
    int phnNo;
};

class Resource1{
    public :
    Resource1(){std::cout << "Resource1 Initialized!!\n";}
    ~Resource1(){std::cout << "Resource1 Destroyed!!\n";}
    void func(){
        std::cout <<"hello from class Resource1\n"<<std::endl;
    }
};

class Resource2{
    public:
    Resource2(){std::cout << "Resource2 Initialized!!\n";}
    ~Resource2(){std::cout << "Resource2 Destroyed!!\n";}
};

void noDelete(){
    Resource *ptr = new Resource;
    ptr->name = "Soumava";
    ptr->phnNo = 8420;
    if(ptr->phnNo == 8420)
        return;

    delete ptr; // In this case even if we call delete on ptr , memory is never dellocated as function is returned before it is called.

}

/*
std::unique_ptr is a smart pointer introduced in C++11 that manages the lifetime of a 
dynamically allocated object through exclusive ownership. It guarantees that the resource 
it points to is automatically deleted when the unique_ptr goes out of scope, effectively 
eliminating manual delete calls and preventing memory leaks.

1. UNIQUE PTR cannot be copied. It throws error.
2. Ownership can be transferred using std::move().
*/

void uniquePtr(){
    Resource1* ptr = new Resource1();

    //make_unique<A>() creates an object of type A, and ptr becomes its exclusive owner.
    std::unique_ptr<Resource2> res = std::make_unique<Resource2>();
    

    return;
    // I have not called delete but Unique ptr will call it's delete;
    // But Resource 1 with normal ptr won't get destroyed.
}

/* std::shared_ptr
    enables multiple pointers to share ownership of the same dynamically allocated objects.
    uses reference counting to automatically manage the object's lifetime.

    1. uses reference counting for automatic memory management
    2. eliminates manual mem deallocation.

*/

void sharedPtr(){
    std::shared_ptr<Resource1> p1(new Resource1);
    std::cout<<p1.get()<<std::endl;
    p1->func();

    std::shared_ptr<Resource1> p2(p1); // Sharing ownership
    p2->func();

    // Not calling any delete. But both would be deallocated.
    return;
}

int main(){
    std::cout <<  "No Delete Example" <<std::endl;
    noDelete();
    std::cout <<  "Unique Ptr Example" <<std::endl;
    uniquePtr();
    std::cout <<  "Shared Ptr Example" <<std::endl;
    sharedPtr();
    return 0;
}