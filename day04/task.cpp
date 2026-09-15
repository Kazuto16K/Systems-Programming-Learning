#include<iostream>
#include<string>

template<typename T>
class DynamicArray{
private:
    T *m_Data;
    size_t m_Size;
    size_t m_Capacity;

public:
    DynamicArray(){
        m_Data = new T[1];
        m_Size = 0;
        m_Capacity = 1;
    }

    ~DynamicArray(){
        delete[] m_Data;
        m_Size = 0;
        m_Capacity = 0;
    }

    void push_back(T val){
        // when size == capacity
        if(m_Size == m_Capacity){
            // create new capacity
            size_t new_Capacity = m_Capacity*2;
            T* new_Data = new T[new_Capacity];

            // copy into new data
            for(size_t i=0; i<m_Size; i++){
                new_Data[i] = m_Data[i];
            }

            // Delete old data
            delete[] m_Data;

            m_Data = new_Data;
            m_Capacity = new_Capacity;
        }

        // Insert Element
        m_Data[m_Size] = val;
        m_Size++;
    }

    T& operator[](size_t idx){ // operator overloading of []
        return m_Data[idx];
    } 
    
};

int main(){
    DynamicArray<int> arr;
    arr.push_back(100);
    arr.push_back(20);
    arr.push_back(47);
    arr.push_back(69);

    DynamicArray<std::string> str_arr;
    str_arr.push_back("Soumava");
    str_arr.push_back("Das");
    str_arr.push_back("Riju");
    str_arr.push_back("Das");
    str_arr.push_back("Github");
    str_arr.push_back("Hello");
    

    std::cout << arr[1] << std::endl;
    std::cout << arr[0] << std::endl;

    std::cout << str_arr[2] << std::endl;
    std::cout << str_arr[0] << std::endl;
    std::cout << str_arr[4] << std::endl;

    return 0;
}