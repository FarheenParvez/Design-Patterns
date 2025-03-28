#include <iostream>
#include <unordered_map>

class LRUCache {
public:

    class Node {
        public:
            int key;
            int val;
            Node* prev;
            Node* next;
            
            Node(int key, int val) {
                this->key = key;
                this->val = val;
                prev = nullptr;
                next = nullptr;
            }
    }; 

    Node* head;
    Node* tail;

    int cap;
    std::unordered_map<int, Node*> mp;

    LRUCache(int capacity) {
        cap = capacity;
        head = new Node(-1, -1);
        tail = new Node(-1, -1);
        head->next = tail;
        tail->prev = head;      
    }

    ~LRUCache() {
        // Clean up all nodes to avoid memory leaks
        Node* current = head->next;
        while (current != tail) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        delete head;
        delete tail;
    }

    void addNode(Node* newnode) {
        Node* temp = head->next;

        newnode->next = temp;
        newnode->prev = head;

        head->next = newnode;
        temp->prev = newnode;
    }

    void deleteNode(Node* node) {
        Node* pre = node->prev;
        Node* nex = node->next;

        pre->next = nex;
        nex->prev = pre;

        // Delete the node to free memory
        delete node;
    }
    
    int get(int key) {
        if (mp.find(key) != mp.end()) {
            Node* resNode = mp[key];
            int result = resNode->val;

            mp.erase(key);
            deleteNode(resNode);
            addNode(new Node(key, result));

            mp[key] = head->next;
            return result;
        }
        return -1;
    }
    
    void put(int key, int value) {
        if (mp.find(key) != mp.end()) {
            Node* cur = mp[key];
            mp.erase(key);
            deleteNode(cur);
        }

        if (mp.size() == cap) {
            mp.erase(tail->prev->key);
            deleteNode(tail->prev);
        }
        
        addNode(new Node(key, value));
        mp[key] = head->next;
    }
};

int main() {
    LRUCache* obj = new LRUCache(2);
    obj->put(1,1);
    obj->put(2,2);
    std::cout << obj->get(1) << std::endl;
    obj->put(3,3);
    std::cout << obj->get(2) << std::endl;
    obj->put(4,4);
    std::cout << obj->get(1) << std::endl;
    std::cout << obj->get(3) << std::endl;
    std::cout << obj->get(4) << std::endl;

    delete obj;
    return 0;
}