#include <map>
#include <utility>
#include "DoublyLinkedList.hpp"

namespace detail {
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
   using Node = typename DoublyLinkedList<T>::Node;
   // set function
   template<class Vi>
   void set(Key const& key, Vi&& value) {
       static_cast<Derived&>(*this).set(key, std::forward<Vi>(value));
   }
   // get function
   Value get(Key const& key) {
       return static_cast<Derived&>(*this).get(key);
   }
   std::map<Key, Node*> mp; // map the key to the node in the linked list
   size_t cp;  // capacity
   DoublyLinkedList<T> list;
};

// Concrete derived class
// Note: Use of this-> required throughout data member access as Cache<T> is a dependent base class
template<class T>
class LRUCache : public Cache<LRUCache<T>, T> {
public:
    using Node = typename Cache<LRUCache<T>, T>::Node;
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
    Node* head() { return this->list.head; }
    Node* tail() { return this->list.tail; }
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
  Node* target;
  // if there is a node with that key set target as that node
  if (get(key) != -1) {
    target = this->mp[key];
  }
  else {
    // if adding a new node does not exceed capacity, set as NULL to indicate that,
    // otherwise set as tail in order to replace the head
    target = ((m_size+1) <= this->cp) ? NULL : tail();
  }
   
  // if list is empty (or adding a new node does NOT exceed capacity), append a new node
  if (target == NULL) {
    this->list.Push(T(key, value));
    this->mp[key] = head();
    m_size++;
  }
  // if the target is the head, simply update its value
  else if (target == head()) {
      head()->data.value() = std::forward<Vi>(value);
  }
  // otherwise update the value of target and set it as the most recently used
  else {
    this->mp[target->data.key()] = NULL; // "remove" node
    target->data.value() = std::forward<Vi>(value); // update value
    target->data.key() = key; // update key
    
    // move node to the beginning of the list
    this->list.Prepend(target);
  }
}

// returns the value mapped to the key or -1 if not found
template<class T>
auto LRUCache<T>::get(Key const& key) -> Value {
  // mp[key] is NULL only if it has been removed from the cache
  return (this->mp.find(key) != this->mp.end()) && this->mp[key] != NULL
    ? this->mp[key]->data.value()
    : -1;
}
}

template<class T>
using LRUCache = detail::LRUCache<detail::MappedItem<int, T>>;
