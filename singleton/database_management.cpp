#include<iostream>
#include<mutex>
#include<cassert>
#include<string>


class DatabaseManage {
    private:
       //static DatabaseManage& instance;
        std::string connectionString;
        bool isConnected;

        DatabaseManage() : isConnected(false) {}

    public: 
        static DatabaseManage& getInstance() {
            static DatabaseManage instance;
            return  instance;
        }

        bool connect(const std::string& Constr) {
            if(!isConnected) {
                connectionString = Constr;
                isConnected = true;
                return true;
            }

            return false;
        }    

        void query(const std::string& q) {
            if(isConnected) {
                std::cout << "Querying" << std::endl;
            }
        }

        ~DatabaseManage() {
            if(isConnected) {
                isConnected = false;
            }
        }
};

int main() {

    DatabaseManage& db = DatabaseManage::getInstance();

    db.connect("mysql://localhost:3306/mydb");
    db.query("SELECT * from users");

    DatabaseManage& db2 = DatabaseManage::getInstance();
    db2.query("SELECT * FROM products");

    assert(&db == &db2);
    
    return 0;

}