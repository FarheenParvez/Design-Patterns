#include<iostream>

template <class T> 
class sharedptr {
    private:
        T* _resource;
    public:
        //constructor
        explicit sharedptr(T* resource = nullptr) : _resource(resource) {
            std::cout << "insisde cons" << std::endl;
        }    


};


int main() {

    std::cout << "insisde cons" << std::endl;

}