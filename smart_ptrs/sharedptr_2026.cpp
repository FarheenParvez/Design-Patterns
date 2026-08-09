#include<iostream>
using  namespace std; 

template<typename T> 
class sharedptr {
    private: 
        T* res;
        int* counter; //its not a counter but a pointer to a pointer
        // bec in case of copying two objects will have their own counnter
        // counter shud be per resource and res, this will make sure we point to the same one
        
        //implement an increment counter func
        void incrementCounter () {
            if(counter) {
                (*counter)++;
            }
        }

        //implememnt decreemnt
        void decrementCounter() {
            if(counter) {
                (*counter)--;
                if((*counter) == 0) {
                    if(res) {
                        delete res;
                        delete counter;
                    }
                }
            }
        }

    public:

    sharedptr(T* resource = nullptr) : res(resource), counter(new int(1)) {
        cout<< " ctr"<< endl;

    }
    //copy constructor

    sharedptr(const sharedptr<T>& ptr) {
        res = ptr.res;
        counter = ptr.counter;
        //now we have to increment
        incrementCounter();

    }

    //copy assignment operator

    sharedptr<T>& operator=(const sharedptr<T>& ptr) {
        if(this != &ptr) // self asingment
        {
            decrementCounter();
            res = ptr.res;
            counter = ptr.counter;
            incrementCounter();
        }
        return *this;
    }

    //move constr
    
    sharedptr(sharedptr<T>&& ptr) {
        res = ptr.res;
        counter = ptr.counter;

        // no need to increment count bec we are moving not copying
        ptr.res = nullptr;
        ptr.counter = nullptr;


    }

    //move assingment
        //copy assignment operator

    sharedptr<T>& operator=(sharedptr<T>&& ptr) {
        if(this != &ptr) // self asingment
        {
            decrementCounter();
            res = ptr.res;
            counter = ptr.counter;

            ptr.res = nullptr;
            ptr.counter = nullptr;
        }
        return *this;
    }

    //reset
    void reset(T* ptr) {
        decrementCounter();
        res = ptr;
        counter = new int(1);
    }

    // get countfunc
    int getCount() {
        if(counter) {
            return (*counter);
        }
        return -1;
    }

    T* operator->() {
        return res;
    }

    T& operator*() {
        return (*res);
    }

    T* get() {
        return res;
    }

    ~sharedptr() {
        // it shud decrement the count n not delte it
        decrementCounter();
    }
    

}; 

int main() {
    //main usage 
    sharedptr<int> ptr0; // default ctr
    sharedptr<int> ptr1(new int(10));
    sharedptr<int> ptr2(ptr1);

    sharedptr<int> ptr3; 
    ptr3 = ptr1;

    //move
    sharedptr<int> ptr4(std::move(ptr2));
    ptr2 = std::move(ptr4);

    int* val = ptr4.get();
    ptr1.reset(new int(5));

    cout<< *ptr1<<endl;
    
}