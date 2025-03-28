#include <iostream>
#include <thread>
#include <mutex>


// thread unsafe singleton
class Singleton
{
private:
  static Singleton *instance;
  Singleton() {} // Private constructor

public:
  static Singleton* getInstance()
  {
    if (instance == nullptr)
    {
      instance = new Singleton();
    }
    return instance;
  }
};

Singleton* Singleton::instance = nullptr;

// thread safe 
class SingletonSafe {
  private:
    static SingletonSafe* ins;
    static std::mutex mutex;
    SingletonSafe() {}

  public:
    static SingletonSafe* getSafeInstance() {
      if(ins == nullptr ) {
        std::lock_guard<std::mutex> lock(mutex); 
        if(ins == nullptr) {
          ins = new SingletonSafe();
        }
      }
      return ins;
    }


};

SingletonSafe* SingletonSafe::ins = nullptr;
std::mutex SingletonSafe::mutex;


//Meyers Singleton 

class MeyerSingleton {
  private:
    //prevent sirect instanciation
    MeyerSingleton() {}

  public: 
  // Provides global access points
    static MeyerSingleton& getInstance() {
        static MeyerSingleton instance;
        return instance;
    }

    //Delete copy/move constructors and assignent operator
    MeyerSingleton(const MeyerSingleton&) = delete;
    MeyerSingleton& operator=(const MeyerSingleton&) = delete;
    MeyerSingleton(const MeyerSingleton&&) = delete;
    MeyerSingleton& operator=(const MeyerSingleton&&) = delete;

    
};


// thread safe with RAII

class RaSafeSingleton {
  private:
    static std::unique_ptr<RaSafeSingleton> rinsatnce;
    static std::once_flag initflag;
    RaSafeSingleton(){}

  public:
    static RaSafeSingleton* getInstance() {
      std::call_once(initflag, []() {
        rinsatnce.reset(new RaSafeSingleton());
      });
      return rinsatnce.get();
    }
};

std::unique_ptr<RaSafeSingleton> RaSafeSingleton::rinsatnce;
std::once_flag RaSafeSingleton::initflag;

