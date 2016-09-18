namespace detail {
template<class Derived, class T>
class Cache;

template<class T>
class LRUCache;

template<class T>
class DoublyLinkedList {
    struct Node;
public:
    void Push(Node*);
    void Prepend(Node*);
    template<class... U>
    void Push(U&&...);
    template<class... U>
    void Prepend(U&&...);
    friend detail::Cache<LRUCache<T>, T>;
    friend detail::LRUCache<T>;
private:
    Node* head = nullptr,
        * tail = nullptr;
        
    template<class... U>
    Node* NewNode(U&&...);
};

template<class T>
struct DoublyLinkedList<T>::Node {
   template<class... U>
   Node(U&&... us) : data(std::forward<U>(us)...) {};
   friend DoublyLinkedList<T>;
   friend detail::Cache<LRUCache<T>, T>;
   friend detail::LRUCache<T>;
private:
    Node* next;
    Node* prev;
    T data;
};

// creates a new node
template<class T>
template<class... U>
auto DoublyLinkedList<T>::NewNode(U&&... us) -> Node* {
  return new Node(std::forward<U>(us)...);
}

// pushes a new node onto the cache
template<class T>
template<class... U>
void DoublyLinkedList<T>::Push(U&&... us) {
  Push(NewNode(std::forward<U>(us)...));
}

// prepends a node
template<class T>
template<class... U>
void DoublyLinkedList<T>::Prepend(U&&... us) {
  Prepend(NewNode(std::forward<U>(us)...));
}
// prepends a node
template<class T>
void DoublyLinkedList<T>::Prepend(Node* obj) {
  if (obj->next) obj->next->prev = obj->prev; // update prev node of obj->next
  obj->prev->next = obj->next; // remove obj from node list
  obj->next = head; // append obj to head
  if (head) head->prev = obj; // update prev node of head
  head = obj; // update head (now most recently used)
}

// pushes a new node onto the cache
template<class T>
void DoublyLinkedList<T>::Push(Node* obj) {
  // if the list is empty
  if (head == NULL)
    head = obj;
  else
    tail->next = obj;
  
  // set previous node and tail
  obj->prev = tail;
  tail = obj;
}
}
