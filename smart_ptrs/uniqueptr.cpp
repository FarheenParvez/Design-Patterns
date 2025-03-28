// #include<iostream>
// #include<memory>

// // not thread safe
// template <class T>
// class uniqueptr {
//     private:
//         // will always have one member, and almost have same memory as a normal ptr
//         T* ptr;
//     public: 
//         explicit uniqueptr(T*  newptr = nullptr) : ptr(newptr) {
//             std::cout<< "Inside constructor " << std::endl;

//         }
//         //delete copy constructor 
//         uniqueptr(const uniqueptr<T>& p) = delete;

//         // delete =
//         uniqueptr& operator=(const uniqueptr<T>& p) = delete;

//         // move copy constructor, double && because it will use the R-value
//         uniqueptr(uniqueptr<T>&& p) {
//             //we want to move the value from the p to newer ptr
//             ptr = p.ptr;
//             p.ptr = nullptr;

//         }

//         // move assignment operator, double && because it will use the R-value
//         uniqueptr& operator= (uniqueptr<T>&& p) {
//             // check if usr is not performing ptr1 = ptr1, that way if we want to delete it we do nt delete the original ptr
//             if(this!= &p) {
//                 // if ptr4 = ptr3, we will deallocate for ptr4
//                 if(ptr) {
//                     delete ptr;
//                 }
//                 ptr = p.ptr;
//                 p.ptr = nullptr;
//             }

//             return *this;
//         }

//         T* operator-> () {
//             return ptr;
//         }

//         T& operator* (){
//             return *ptr;
//         }

//         // get() returns basic raw  ptrs

//         T* get() {
//             return ptr;
//         }

//         //reset(T* newres), deallocate the prev and allocate new one.
//         void reset(T* newres = nullptr) {
//             if(ptr) {
//                 delete ptr;
//             }

//             ptr = newres;

//         } 

//         //destructor
//         ~uniqueptr(){
//             if(ptr) {
//                 delete ptr;
//                 ptr = nullptr;

//             }

//         }
// };



// int main () {

//     std::unique_ptr<int> p;
//  // how a user is gonna use
 
//  uniqueptr<int> ptr1(new int(10));
//  //uniqueptr<int> ptr2(ptr1); //it shouldgive compilation errorr bec ownership should be exclusive

//  //uniqueptr<int> ptr3 = ptr1; // error bec same (copy constructor)
//  uniqueptr<int> ptr4(new int(500));
//  //ptr4 = ptr3; //eerror (copy assignment operator)

//  //Now how to transfer the ownership
//  // use std::move(ptr)

//  uniqueptr<int> ptr5 = std::move(ptr4); 
//  ptr5 = std::move(ptr1);

//  // Dereferencing
//  std::cout << "Use: "<< *ptr5<< std::endl;

//  // to get the resource it is handling
//  ptr5.get();

//  //reset(), handle a different resource
//  ptr5.reset(new int());

//     return 0;
// }


#include<iostream> 



template <class T> 
class uniqueptr {
    private: 
        T* resource;
    public:
        // all the constructors
        
        explicit uniqueptr(T* ptr=nullptr) : resource(ptr) {
            std::cout<< "inside default  constructors" << std::endl;
        }

        // copy constructor should be deleted

        uniqueptr(const uniqueptr<T>& p) = delete;

        // ssignment operator should be deleted as well
        uniqueptr& operator= (const uniqueptr<T>& p) = delete;

        // move constructor
        uniqueptr(uniqueptr<T>&& ptr) {

            resource = ptr.resource;
            ptr.resource = nullptr;
        }

        // move assignment operator
        uniqueptr& operator= (uniqueptr<T>&& ptr) {

            if(this!= &ptr) {
                if(resource) {
                    delete resource;
                }

                resource = ptr.resource;
                ptr.resource = nullptr;
            }

            return *this;

        }

        // get
        T* get() {
            return resource;
        }

        // restore
        void restore(T* ptr = nullptr) {

            if(resource) {
                delete resource;
              
            }

            resource = ptr;
        }

        // * operator
        T& operator* () {
            return *resource;
        }

        // -> operator
        T* operator-> () {
            return resource;
        }


        // destructor
        ~uniqueptr() {
            if(resource) {
                delete resource;
                resource = nullptr;
            }

            
        }

};


int main() {
    // first decide how the user wants to use our ptr

    uniqueptr<int> p;
    uniqueptr<int> p1(new int(10));
    uniqueptr<int> p3(new int(40));

    // Now for error paths
    // uniqueptr<int> p4 = p3;
    // p1 = p;


    uniqueptr<int> p5 = std::move(p1);
    p3 = std::move(p);


    p5.get();
    
   // std::cout<<"get the value " << *p3 << std::endl;

    p3.restore(new int(113));
   // std::cout<<"get the ptr" << *p3 << std::endl;


    return 0;

    

}



























// #include<iostream>


// // uniqueptr

// template <class T> 
// class uniqueptr {
//     private:
//         T* resource;

//     public:
//         explicit uniqueptr(T* newptr = nullptr) : resource(newptr) {

//             std::cout<< "cons" << std::endl;

//         }

//         //copy constructpr delete
//         uniqueptr(const uniqueptr<T>& p) = delete;

//         // delete = 
//         uniqueptr& operator= (const uniqueptr<T>& p) = delete;

//         //move constructor, Rval
//         uniqueptr(uniqueptr<T>&& p) {

//             resource = p.resource;
//             p.resource = nullptr;

//         }

//         uniqueptr& operator= (uniqueptr<T>&& p) {
//             // if ptr1 = ptr1

//             if(this!= &p) {

//                 // ptrnew = p;
//                 if(resource) {
//                     delete resource;
//                 }
//                 resource = p.resource;
//                 p.resource = nullptr;

//             }

//             return *this;

//         }

//         //get
//         T* get() {
//             return resource;
//         }

//         //reset, 

//         void reset(T* newres) {
//             if(resource)
//                 resource = nullptr;

//             resource = newres;
//         }


//         T& operator* () {
//             return *resource;
//         }

//         T* operator-> () {
//             return resource;
//         }


//         ~uniqueptr() {
//             if(resource) {
//                 delete resource;
//                 resource = nullptr;
//             }
           
//         }

// };



// int main() {

//     uniqueptr<int> p(new int(20));
//     //uniqueptr<int> p1(p); //error
//    // p1 = p; // error

//     //uniqueptr<int> p2 = p; // error

//     std::cout<< *p;

//     uniqueptr<int> pm = std::move(p);

//     uniqueptr<int> pn(new int(10));

//     pm = std::move(pn);
    

//     pm.reset(new int(33));


// }