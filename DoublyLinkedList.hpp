// minimal linked list class for convienient access by LRUCache
namespace detail {
template<class Derived, class T>
class Cache;

template<class T>
class LRUCache;

template<class T>
class node {
  friend Cache<LRUCache<T>, T>;
  friend LRUCache<T>;
private:
  T data;
  shared_ptr<node> next, prev;
public:
  node(T const& data, shared_ptr<node> next = nullptr, shared_ptr<node> prev = nullptr) : data(data), next(next), prev(prev) { }

  template<class U>
  static shared_ptr<node> create(U&& u) {
      return make_shared<node>(std::forward<U>(u));
  }
};

template<class T>
class list {
  friend Cache<LRUCache<T>, T>;
  friend LRUCache<T>;
private:
  using node = shared_ptr<node<T>>;
  using value_type = T;
private:
  node head, tail;
private:
  template<class U> void push_back(U&&);
  template<class U> void push_front(U&&);
public:
  void push_front(node);
  void push_back(node);
}; 

template<class T>
template<class U>
void list<T>::push_back(U&& u) {
  push_back(node::create(std::forward<U>(u)));
}

template<class T>
template<class U>
void list<T>::push_front(U&& u) {
  push_front(node::create(std::forward<U>(u)));
}

template<class T>
void list<T>::push_back(node obj) {
  // if the list is empty
  if (head == nullptr)
    head = obj;
  else
    tail->next = obj;
  
  // set previous node and tail
  obj->prev = tail;
  tail = obj;
}

template<class T>
void list<T>::push_front(node obj) {
  if (obj->next) obj->next->prev = obj->prev; // update prev node of obj->next
  obj->prev->next = obj->next; // remove obj from node list
  obj->next = head; // append obj to head
  if (head) head->prev = obj; // update prev node of head
  head = obj; // update head (now most recently used)
}
} // namespace detail
