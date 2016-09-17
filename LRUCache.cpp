#include <map>

struct Node {
   Node* next;
   Node* prev;
   int value;
   int key;
   Node(Node* p, Node* n, int k, int val) : prev(p), next(n), key(k), value(val) {};
   Node(int k, int val) : prev(NULL), next(NULL), key(k), value(val) {};
};

class Cache {
protected: 
   std::map<int, Node*> mp; // map the key to the node in the linked list
   int cp;  // capacity
   Node* tail; // double linked list tail pointer
   Node* head; // double linked list head pointer
   virtual void set(int, int) = 0; // set function
   virtual int get(int) = 0; // get function
};

class LRUCache : public Cache {
public:
  // initializes the cache with given capacity
  LRUCache(size_t capacity);
  // updates/sets element with key value pair
  void set(int, int);
  // retrieves element with given key (returns -1 if not found)
  int get(int);
  // size of the cache
  size_t size() const { return m_size; }
private:
  // appends a node to the cache
  void Push(Node*);
  // appends a node to the cache
  void Push(int, int);
  // moves a node to the begining of the list
  void Prepend(Node*);
  // creates a node and moves it to the beginning of the list
  void Prepend(int, int);
  // creates a new node
  Node* NewNode(int, int);
  // capacity and current size of the cache
  size_t m_size;
};

LRUCache::LRUCache(size_t capacity)
  : m_size(0)
{
    // set capacity
    cp = capacity;
    // create empty cache
    head = tail = NULL;
}

// creates a new node
Node* LRUCache::NewNode(int key, int value) {
  return new Node(key, value);
}

// pushes a new node onto the cache
void LRUCache::Push(int key, int value) {
  Push(NewNode(key, value));
}

// prepends a node
void LRUCache::Prepend(int key, int value) {
  Prepend(NewNode(key, value));
}
// prepends a node
void LRUCache::Prepend(Node* obj) {
  if (obj->next) obj->next->prev = obj->prev; // update prev node of obj->next
  obj->prev->next = obj->next; // remove obj from node list
  obj->next = head; // append obj to head
  if (head) head->prev = obj; // update prev node of head
  head = obj; // update head (now most recently used)
  // map key to obj
  mp[obj->key] = obj;
}

// pushes a new node onto the cache
void LRUCache::Push(Node* obj) {
  // if the list is empty
  if (head == NULL)
    head = obj;
  else
    tail->next = obj;
  
  // set previous node and tail
  obj->prev = tail;
  tail = obj;
  
  // map key to new node
  mp[obj->key] = obj;
}

// if key is present and cache length exceeds capacity, move node to beginning of the list (now most recently use)
// otherwise, remove the least recently used node and prepend new node to beginning
void LRUCache::set(int key, int value) {
  Node* target;
  // if there is a node with that key set target as that node
  if (get(key) != -1) {
    target = mp[key];
  }
  else {
    // if adding a new node does not exceed capacity, set as NULL to indicate that,
    // otherwise set as tail in order to replace the head
    target = ((m_size+1) <= cp) ? NULL : tail;
  }
   
  // if list is empty (or adding a new node does NOT exceed capacity), append a new node
  if (target == NULL) {
    Push(key, value);
    m_size++;
  }
  // if the target is the head, simply update its value
  else if (target == head) {
      head->value = value;
  }
  // otherwise update the value of target and set it as the most recently used
  else {
    mp[target->key] = NULL; // "remove" node
    target->value = value; // update value
    target->key = key; // update key
    
    // move node to the beginning of the list
    Prepend(target);
  }
}

// returns the value mapped to the key or -1 if not found
int LRUCache::get(int key) {
  // mp[key] is NULL only if it has been removed from the cache
  return (mp.find(key) != mp.end()) && mp[key] != NULL
    ? mp[key]->value
    : -1;
}
