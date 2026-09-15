#include<iostream>
#include<string>

template<typename T> // The variable can be used throughout the function
void Print(T val){
    std::cout<<val<<std::endl;
}

template<typename T, int N>
class Array{
private:
    T m_Array[N];
public:
    int GetSize() const { return N; }
};

int main(){
    Print<int>(5);
    Print<std::string>("Soumava");
    Print<float>(7.9);

    Array<int,5> array;
    std::cout << array.GetSize() <<std::endl;

    return 0;
}
