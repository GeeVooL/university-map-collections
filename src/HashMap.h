#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <list>
#include <stdexcept>
#include <utility>

#define HASHMAP_SIZE 2048

namespace aisdi {

template <typename KeyType, typename ValueType>
class HashMap {
 public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using list_iterator = typename std::list<key_type>::iterator;
  using hash_type = typename std::hash<key_type>;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

 private:
  std::list<value_type> m_data[HASHMAP_SIZE];
  size_type m_size = 0;
  hash_type hash_fn;
 public:
  HashMap() {}

  HashMap(std::initializer_list<value_type> list) {
    for (const value_type& val : list)
    {
      const size_type index = hash_fn(val->first) % HASHMAP_SIZE;
      m_data[index].emplace_back(val->first, val->second);
    }
  }

  HashMap(const HashMap& other) {
    m_data = other.m_data;
    m_size = other.m_size;
  }

  HashMap(HashMap&& other) {
    *this = std::move(other);
  }

  HashMap& operator=(const HashMap& other) {
    if(this != &other) {
      // Potential leak
      m_data = other.m_data;
      m_size = other.m_size;
    }
    return *this;
  }

  HashMap& operator=(HashMap&& other) {
    if(this != &other) {
      // Potential leak
      m_data = std::move(other.m_data);
      m_size = std::move(other.m_size);
    }
    return *this;
  }

  bool isEmpty() const { return !m_size; }

  mapped_type& operator[](const key_type& key) {
    const size_type index = hash_fn(key)  % HASHMAP_SIZE;

    for (list_iterator it : m_data[index]) {
      if (it->first == key)
        return it->second;
    }

    m_data[index].emplace_back(key, mapped_type{});
    ++m_size;
    return m_data[index].back()->second;
  }

  const mapped_type& valueOf(const key_type& key) const {
    const size_type index = hash_fn(key)  % HASHMAP_SIZE;
    
    for (list_iterator it : m_data[index]) {
      if (it->first == key)
        return it->second;
    }

    throw std::out_of_range("Element with given key does not exist");
  }

  mapped_type& valueOf(const key_type& key) {
    return const_cast<HashMap&>(*this).valueOf(key);
  }

  const_iterator find(const key_type& key) const {
    const size_type index = hash_fn(key)  % HASHMAP_SIZE;
    
    for (list_iterator it : m_data[index]) {
      if (it->first == key)
        return const_iterator(); // TODO: here
    }

    return cend();
  }

  iterator find(const key_type& key) {
    const size_type index = hash_fn(key)  % HASHMAP_SIZE;

    for (list_iterator it : m_data[index]) {
      if (it->first == key)
        return iterator(); // TODO: here
    }

    return end();
  }

  void remove(const key_type& key) {
    const size_type index = hash_fn(key)  % HASHMAP_SIZE;

    for (list_iterator it : m_data[index]) {
      if (it->first == key) {
        m_data[index].erase(it);
        --m_size;
        return;
      }
    }

    throw std::out_of_range("Element with given key does not exist");
  }

  void remove(const const_iterator& it) {
    const size_type index = hash_fn(it->first)  % HASHMAP_SIZE;

    for (list_iterator elem : m_data[index]) {
      if (elem->first == it->first) {
        m_data[index].erase(elem);
        --m_size;
        return;
      }
    }

    throw std::out_of_range("Element with given key does not exist");
  }

  size_type getSize() const { return m_size; }

  bool operator==(const HashMap& other) const {
    (void)other;
    throw std::runtime_error("TODO");
  }

  bool operator!=(const HashMap& other) const { return !(*this == other); }

  iterator begin() { throw std::runtime_error("TODO"); }

  iterator end() { throw std::runtime_error("TODO"); }

  const_iterator cbegin() const { throw std::runtime_error("TODO"); }

  const_iterator cend() const { throw std::runtime_error("TODO"); }

  const_iterator begin() const { return cbegin(); }

  const_iterator end() const { return cend(); }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator {
 public:
  using reference = typename HashMap::const_reference;
  // using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;
  using size_type = typename HashMap::size_type;
 private:
  size_type index;
  size_type offset;
 public:
  explicit ConstIterator() {}

  ConstIterator(const ConstIterator& other) {
    (void)other;
    throw std::runtime_error("TODO");
  }

  ConstIterator& operator++() { throw std::runtime_error("TODO"); }

  ConstIterator operator++(int) { throw std::runtime_error("TODO"); }

  ConstIterator& operator--() { throw std::runtime_error("TODO"); }

  ConstIterator operator--(int) { throw std::runtime_error("TODO"); }

  reference operator*() const { throw std::runtime_error("TODO"); }

  pointer operator->() const { return &this->operator*(); }

  bool operator==(const ConstIterator& other) const {
    (void)other;
    throw std::runtime_error("TODO");
  }

  bool operator!=(const ConstIterator& other) const {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator
    : public HashMap<KeyType, ValueType>::ConstIterator {
 public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

  explicit Iterator() {}

  Iterator(const ConstIterator& other) : ConstIterator(other) {}

  Iterator& operator++() {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int) {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--() {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int) {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const { return &this->operator*(); }

  reference operator*() const {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};
}  // namespace aisdi

#endif /* AISDI_MAPS_HASHMAP_H */
