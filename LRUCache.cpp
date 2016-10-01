#include <memory>
#include <map>

// minimal linked list class for convienient access by LRUCache
namespace detail {
template<class Derived, class T>
class Cache;

template<class T>
class LRUCache;
  
template<class T>
class list;

template<class T>
class node {
  friend list<T>;
  friend LRUCache<T>;
private:
  T data;
  std::shared_ptr<node> next, prev;
public:
  node(T const& data, std::shared_ptr<node> next = nullptr, std::shared_ptr<node> prev = nullptr) : data(data), next(next), prev(prev) { }

  template<class U>
  static std::shared_ptr<node> create(U&& u) {
      return make_shared<node>(std::forward<U>(u));
  }
};

template<class T>
class list {
  friend LRUCache<T>;
private:
  using value_type = T;
private:
  std::shared_ptr<node<T>> head = nullptr, tail = nullptr;
private:
  template<class U> void push_back(U&&);
  template<class U> void push_front(U&&);
public:
  void push_front(std::shared_ptr<node<T>>);
  void push_back(std::shared_ptr<node<T>>);
}; 

template<class T>
template<class U>
void list<T>::push_back(U&& u) {
  push_back(node<T>::create(std::forward<U>(u)));
}

template<class T>
template<class U>
void list<T>::push_front(U&& u) {
  push_front(node<T>::create(std::forward<U>(u)));
}

template<class T>
void list<T>::push_back(std::shared_ptr<node<T>> obj) {
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
void list<T>::push_front(std::shared_ptr<node<T>> obj) {
  if (obj->next) obj->next->prev = obj->prev; // update prev node of obj->next
  obj->prev->next = obj->next; // remove obj from node list
  obj->next = head; // append obj to head
  if (head) head->prev = obj; // update prev node of head
  head = obj; // update head (now most recently used)
}

template<class T, class U>
struct MappedItem : std::pair<T, U> {
    using Base = std::pair<T, U>;
    using Base::Base;
    T& key()   { return this->first; }
    U& value() { return this->second; }
};

class ICache {
public:
    virtual ~ICache() = default;
};

template<class Derived, class T>
class Cache;

template<class Derived, class K, class V>
class Cache<Derived, MappedItem<K, V>> : public ICache {
    using T = MappedItem<K, V>;
protected:
   using Key = K;
   using Value = V;
   // set function
   template<class Vi>
   void set(Key const& key, Vi&& value) {
       static_cast<Derived&>(*this).set(key, std::forward<Vi>(value));
   }
   // get function
   Value get(Key const& key) {
       return static_cast<Derived&>(*this).get(key);
   }
   std::map<Key, std::shared_ptr<node<T>>> mp; // map the key to the node in the linked list
   size_t cp;  // capacity
   list<T> cache;
};

// Concrete derived class
// Note: Use of this-> required throughout data member access as Cache<T> is a dependent base class
template<class T>
class LRUCache : public Cache<LRUCache<T>, T> {
public:
    using Key = typename Cache<LRUCache<T>, T>::Key;
    using Value = typename Cache<LRUCache<T>, T>::Value;
  // initializes the cache with given capacity
  LRUCache(size_t capacity);
  // updates/sets element with key value pair
  template<class Vi>
  void set(Key const&, Vi&& u);
  // retrieves element with given key (returns -1 if not found)
  Value get(Key const&);
  // size of the cache
  size_t size() const { return m_size; }
private:
    std::shared_ptr<node<T>> head() { return this->cache.head; }
    std::shared_ptr<node<T>> tail() { return this->cache.tail; }
  // capacity and current size of the cache
  size_t m_size;
};

template<class T>
LRUCache<T>::LRUCache(size_t capacity)
  : m_size(0)
{
    // set capacity
    this->cp = capacity;
}

// if key is present and cache length exceeds capacity, move node to beginning of the list (now most recently use)
// otherwise, remove the least recently used node and prepend new node to beginning
template<class T>
template<class Vi>
void LRUCache<T>::set(Key const& key, Vi&& value) {
  std::shared_ptr<node<T>> target;
  // if there is a node with that key set target as that node
  if (get(key) != -1) {
    target = this->mp[key];
  }
  else {
    // if adding a new node does not exceed capacity, set as NULL to indicate that,
    // otherwise set as tail in order to replace the head
    target = ((m_size+1) <= this->cp) ? nullptr : tail();
  }
   
  // if list is empty (or adding a new node does NOT exceed capacity), append a new node
  if (target == nullptr) {
    auto obj = make_shared<node<T>>(T(key, value));
    this->cache.push_back(obj);
    this->mp[key] = obj;
    m_size++;
  }
  // if the target is the head, simply update its value
  else if (target == head()) {
      head()->data.value() = std::forward<Vi>(value);
  }
  // otherwise update the value of target and set it as the most recently used
  else {
    this->mp[target->data.key()] = nullptr; // "remove" node
    target->data.value() = std::forward<Vi>(value); // update value
    target->data.key() = key; // update key 
    
    // move node to the beginning of the list
    this->cache.push_front(target);
    // update map
    this->mp[key] = target;
  }
}

// returns the value mapped to the key or -1 if not found
template<class T>
auto LRUCache<T>::get(Key const& key) -> Value {
  // mp[key] is NULL only if it has been removed from the cache
  return (this->mp.find(key) != this->mp.end()) && this->mp[key] != nullptr
    ? this->mp[key]->data.value()
    : -1;
}
} // namespace detail

template<class T>
using LRUCache = detail::LRUCache<detail::MappedItem<int, T>>;
