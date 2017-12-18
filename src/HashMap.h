#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <list>
#include <stdexcept>
#include <string>
#include <utility>

#define HASHMAP_SIZE 8192

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

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

 private:
  std::array<std::list<value_type>, HASHMAP_SIZE> m_data;
  size_type m_size = 0;
  std::hash<key_type> hash_fn;

 public:
  HashMap() {}

  HashMap(std::initializer_list<value_type> list) : m_size(list.size()) {
    for (const value_type& val : list) {
      const size_type index = hash_fn(val.first) % HASHMAP_SIZE;
      m_data[index].emplace_back(val.first, val.second);
    }
  }

  HashMap(const HashMap& other) {
    for (auto i = 0; i < HASHMAP_SIZE; i++) {
      if (!other.m_data[i].empty())
        for (auto elem : other.m_data[i])
          m_data[i].emplace_back(elem.first, elem.second);
    }
    m_size = other.m_size;
  }

  HashMap(HashMap&& other) { *this = std::move(other); }

  HashMap& operator=(const HashMap& other) {
    if (this != &other) {
      for (auto i = 0; i < HASHMAP_SIZE; i++) {
        m_data[i].clear();
        if (!other.m_data[i].empty())
          for (auto elem : other.m_data[i])
            m_data[i].emplace_back(elem.first, elem.second);
      }
      m_size = other.m_size;
    }
    return *this;
  }

  HashMap& operator=(HashMap&& other) {
    if (this != &other) {
      m_data = std::move(other.m_data);
      m_size = std::move(other.m_size);

      other.m_size = 0;
    }
    return *this;
  }

  bool isEmpty() const { return !m_size; }

  mapped_type& operator[](const key_type& key) {
    const size_type index = hash_fn(key) % HASHMAP_SIZE;

    for (auto it = m_data[index].begin(); it != m_data[index].end(); it++) {
      if ((*it).first == key)
        return (*it).second;
    }

    m_data[index].emplace_back(key, mapped_type{});
    ++m_size;
    return m_data[index].back().second;
  }

  const mapped_type& valueOf(const key_type& key) const {
    const size_type index = hash_fn(key) % HASHMAP_SIZE;

    for (auto it = m_data[index].begin(); it != m_data[index].end(); it++) {
      if ((*it).first == key)
        return (*it).second;
    }

    throw std::out_of_range("Element with given key does not exist");
  }

  mapped_type& valueOf(const key_type& key) {
    // return const_cast<HashMap&>(*this).valueOf(key);
    const size_type index = hash_fn(key) % HASHMAP_SIZE;

    for (auto it = m_data[index].begin(); it != m_data[index].end(); it++) {
      if ((*it).first == key)
        return (*it).second;
    }

    throw std::out_of_range("Element with given key does not exist");
  }

  const_iterator find(const key_type& key) const {
    const size_type index = hash_fn(key) % HASHMAP_SIZE;

    for (auto it = m_data[index].begin(); it != m_data[index].end(); it++) {
      if ((*it).first == key)
        return const_iterator(*this, index, it);
    }
    return cend();
  }

  iterator find(const key_type& key) {
    const size_type index = hash_fn(key) % HASHMAP_SIZE;

    for (auto it = m_data[index].begin(); it != m_data[index].end(); it++) {
      if ((*it).first == key)
        return iterator(*this, index, it);
    }

    return end();
  }

  void remove(const key_type& key) {
    const size_type index = hash_fn(key) % HASHMAP_SIZE;

    for (auto it = m_data[index].begin(); it != m_data[index].end(); it++) {
      if ((*it).first == key) {
        m_data[index].erase(it);
        --m_size;
        return;
      }
    }

    throw std::out_of_range("Element with given key does not exist");
  }

  void remove(const const_iterator& it) {
    const size_type index = hash_fn(it->first) % HASHMAP_SIZE;

    for (auto it_ = m_data[index].begin(); it_ != m_data[index].end(); it_++) {
      if ((*it_).first == (*it).first) {
        m_data[index].erase(it_);
        --m_size;
        return;
      }
    }

    throw std::out_of_range("Element with given key does not exist");
  }

  size_type getSize() const { return m_size; }

  bool operator==(const HashMap& other) const {
    if (this->getSize() != other.getSize())
      return false;

    const_iterator org = this->cbegin();
    const_iterator oth = other.cbegin();

    for (size_type i = 0; i < getSize(); i++) {
      if (org->first != oth->first || org->second != oth->second)
        return false;

      ++org;
      ++oth;
    }

    return true;
  }

  bool operator!=(const HashMap& other) const { return !(*this == other); }

  iterator begin() {
    if (!isEmpty())
      for (auto i = 0; i < HASHMAP_SIZE; ++i)
        if (!m_data[i].empty())
          return iterator(*this, i, m_data[i].begin());

    return iterator(*this, 0, m_data[0].begin(), true);
  }

  iterator end() {
    return iterator(*this, HASHMAP_SIZE - 1, m_data[HASHMAP_SIZE - 1].end(),
                    true);
  }

  const_iterator cbegin() const {
    if (!isEmpty())
      for (auto i = 0; i < HASHMAP_SIZE; ++i)
        if (!m_data[i].empty())
          return const_iterator(*this, i, m_data[i].begin());

    return const_iterator(*this, 0, m_data[0].begin(), true);
  }

  const_iterator cend() const {
    return const_iterator(*this, HASHMAP_SIZE - 1,
                          m_data[HASHMAP_SIZE - 1].end(), true);
  }

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
  using list_iterator =
      typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

 protected:
  const HashMap& m_source;
  size_type m_index;
  list_iterator m_element;
  bool m_isSentinel;

 public:
  explicit ConstIterator(const HashMap& source,
                         size_type index,
                         list_iterator element,
                         bool isSentinel = false)
      : m_source(source),
        m_index(index),
        m_element(element),
        m_isSentinel(isSentinel) {}

  ConstIterator(const ConstIterator& other)
      : m_source(other.m_source),
        m_index(other.m_index),
        m_element(other.m_element),
        m_isSentinel(other.m_isSentinel) {}

  ConstIterator& operator++() {
    if (m_isSentinel)
      throw std::out_of_range("Next iterator does not exist");

    list_iterator tmp = ++m_element;
    if (tmp != m_source.m_data[m_index].end()) {
      ++m_element;
      return *this;
    }

    for (size_type i = m_index + 1; i < HASHMAP_SIZE; ++i) {
      if (!m_source.m_data[i].empty()) {
        m_element = m_source.m_data[i].begin();
        m_index = i;
        return *this;
      }
    }

    m_index = HASHMAP_SIZE - 1;
    m_element = m_source.m_data[HASHMAP_SIZE - 1].end();
    m_isSentinel = true;
    return *this;
  }

  ConstIterator operator++(int) {
    ConstIterator tmp(*this);
    ++(*this);
    return tmp;
  }

  ConstIterator& operator--() {
    if (m_element != m_source.m_data[m_index].begin()) {
      --m_element;
      m_isSentinel = false;
      return *this;
    }

    if (m_index == 0)
      throw std::out_of_range("Previous iterator does not exist");

    for (size_type i = m_index - 1; i >= 0; --i) {
      if (!m_source.m_data[i].empty()) {
        m_element = --(m_source.m_data[i].end());
        m_index = i;
        m_isSentinel = false;
        return *this;
      }
    }

    throw std::out_of_range("Previous iterator does not exist");
  }

  ConstIterator operator--(int) {
    ConstIterator tmp(*this);
    --(*this);
    return tmp;
  }

  reference operator*() const {
    if (m_isSentinel)
      throw std::out_of_range("Iterator does not have a value");
    return *(m_element);
  }

  pointer operator->() const { return &this->operator*(); }

  bool operator==(const ConstIterator& other) const {
    if (&m_source == &(other.m_source)) {
      if (m_isSentinel == other.m_isSentinel)
        return true;
      return m_index == other.m_index && m_element == other.m_element;
    }
    return false;
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
  using list_iterator =
      typename std::list<std::pair<const KeyType, ValueType>>::iterator;
  explicit Iterator(const HashMap& source,
                    size_type index,
                    list_iterator element,
                    bool isSentinel = false)
      : ConstIterator(source, index, element, isSentinel) {}

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
