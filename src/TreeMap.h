#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <forward_list>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi {

template <typename KeyType, typename ValueType>
class TreeMap {
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
  // Non-value node
  class Node {
   public:
    enum Color { BLACK, RED };

    Node* m_p;
    Node* m_left;
    Node* m_right;
    Color m_color;

    Node() : m_p(this), m_left(this), m_right(this), m_color(Color::BLACK) {}
    virtual ~Node() = default;
  };

  // Value node
  class ValueNode : public Node {
   public:
    value_type m_value;

    ValueNode(const key_type& key, const mapped_type& value)
        : Node(), m_value(key, value) {}
  };

 private:
  // Members
  Node* m_root;
  Node* m_nil;
  size_type m_size;

 private:
  // Methods

  void leftRotate(Node* x) {
    Node* y = x->m_right;
    x->m_right = y->m_left;

    if (y->m_left != m_nil)
      y->m_left->m_p = x;

    y->m_p = x->m_p;

    if (x->m_p == m_nil) {
      m_root = y;
      m_nil->m_left = m_root;
      m_nil->m_right = m_root;
    } else if (x == x->m_p->m_left)
      x->m_p->m_left = y;
    else
      x->m_p->m_right = y;
    y->m_left = x;
    x->m_p = y;
  }

  void rightRotate(Node* x) {
    Node* y = x->m_left;
    x->m_left = y->m_right;

    if (y->m_right != m_nil)
      y->m_right->m_p = x;

    y->m_p = x->m_p;

    if (x->m_p == m_nil) {
      m_root = y;
      m_nil->m_left = m_root;
      m_nil->m_right = m_root;
    } else if (x == x->m_p->m_right)
      x->m_p->m_right = y;
    else
      x->m_p->m_left = y;
    y->m_right = x;
    x->m_p = y;
  }

  void insertFixup(Node* z) {
    Node* y;

    while (z->m_p->m_color) {
      if (z->m_p == z->m_p->m_p->m_left) {
        y = z->m_p->m_p->m_right;
        if (y->m_color) {
          z->m_p->m_color = Node::Color::BLACK;
          y->m_color = Node::Color::BLACK;
          z->m_p->m_p->m_color = Node::Color::RED;
          z = z->m_p->m_p;
        } else {
          if (z == z->m_p->m_right) {
            z = z->m_p;
            leftRotate(z);
          }
          z->m_p->m_color = Node::Color::BLACK;
          z->m_p->m_p->m_color = Node::Color::RED;
          rightRotate(z->m_p->m_p);
        }
      } else {
        y = z->m_p->m_p->m_left;
        if (y->m_color) {
          z->m_p->m_color = Node::Color::BLACK;
          y->m_color = Node::Color::BLACK;
          z->m_p->m_p->m_color = Node::Color::RED;
          z = z->m_p->m_p;
        } else {
          if (z == z->m_p->m_left) {
            z = z->m_p;
            rightRotate(z);
          }
          z->m_p->m_color = Node::Color::BLACK;
          z->m_p->m_p->m_color = Node::Color::RED;
          leftRotate(z->m_p->m_p);
        }
      }
    }

    m_root->m_color = Node::Color::BLACK;
  }

  Node* search(key_type key) const {
    Node* ptr = m_root;
    while (ptr != m_nil && static_cast<ValueNode*>(ptr)->m_value.first != key)
      if (key < static_cast<ValueNode*>(ptr)->m_value.first)
        ptr = ptr->m_left;
      else
        ptr = ptr->m_right;
    return ptr;
  }

  void insert(ValueNode* z) {
    Node* y = m_nil;
    Node* x = m_root;

    while (x != m_nil) {
      y = x;
      if (z->m_value.first < static_cast<ValueNode*>(x)->m_value.first)
        x = x->m_left;
      else
        x = x->m_right;
    }
    z->m_p = y;

    if (y == m_nil) {
      m_root = z;
      m_nil->m_left = m_root;
      m_nil->m_right = m_root;
    } else if (z->m_value.first < static_cast<ValueNode*>(y)->m_value.first)
      y->m_left = z;
    else
      y->m_right = z;

    z->m_left = m_nil;
    z->m_right = m_nil;
    z->m_color = Node::Color::RED;

    m_size++;
    insertFixup(z);
  }

  void transplant(Node* u, Node* v) {
    if (u->m_p == m_nil) {
      m_root = v;
      m_nil->m_left = m_root;
      m_nil->m_right = m_root;
    } else if (u == u->m_p->m_left)
      u->m_p->m_left = v;
    else
      u->m_p->m_right = v;
    v->m_p = u->m_p;
  }

  void deleteFixup(Node* x) {
    Node* w;
    while (x != m_root && x->m_color == Node::Color::BLACK) {
      if (x == x->m_p->m_left) {
        w = x->m_p->m_right;
        if (w->m_color) {
          w->m_color = Node::Color::BLACK;
          x->m_p->m_color = Node::Color::RED;
          leftRotate(x->m_p);
          w = x->m_p->m_right;
        }
        if (w->m_left->m_color == Node::Color::BLACK &&
            w->m_right->m_color == Node::Color::BLACK) {
          w->m_color = Node::Color::RED;
          x = x->m_p;
        } else {
          if (w->m_right->m_color == Node::Color::BLACK) {
            w->m_left->m_color = Node::Color::BLACK;
            w->m_color = Node::Color::RED;
            rightRotate(w);
            w = x->m_p->m_right;
          }
          w->m_color = x->m_p->m_color;
          x->m_p->m_color = Node::Color::BLACK;
          w->m_right->m_color = Node::Color::BLACK;
          leftRotate(x->m_p);
          x = m_root;
        }
      } else {
        w = x->m_p->m_left;
        if (w->m_color) {
          w->m_color = Node::Color::BLACK;
          x->m_p->m_color = Node::Color::RED;
          rightRotate(x->m_p);
          w = x->m_p->m_left;
        }
        if (w->m_right->m_color == Node::Color::BLACK &&
            w->m_left->m_color == Node::Color::BLACK) {
          w->m_color = Node::Color::RED;
          x = x->m_p;
        } else {
          if (w->m_left->m_color == Node::Color::BLACK) {
            w->m_right->m_color = Node::Color::BLACK;
            w->m_color = Node::Color::RED;
            leftRotate(w);
            w = x->m_p->m_left;
          }
          w->m_color = x->m_p->m_color;
          x->m_p->m_color = Node::Color::BLACK;
          w->m_left->m_color = Node::Color::BLACK;
          rightRotate(x->m_p);
          x = m_root;
        }
      }
    }

    x->m_color = Node::Color::BLACK;
  }

  void remove(Node* z) {
    Node* org = z;
    Node* x;
    Node* y = z;
    auto yOriginalColor = y->m_color;

    if (z->m_left == m_nil) {
      x = z->m_right;
      transplant(z, z->m_right);
    } else if (z->m_right == m_nil) {
      x = z->m_left;
      transplant(z, z->m_left);
    } else {
      y = z->m_right;

      while (y->m_left != m_nil)
        y = y->m_left;

      yOriginalColor = y->m_color;
      x = y->m_right;

      if (y->m_p == z)
        x->m_p = y;
      else {
        transplant(y, y->m_right);
        y->m_right = z->m_right;
        y->m_right->m_p = y;
      }

      transplant(z, y);
      y->m_left = z->m_left;
      y->m_left->m_p = y;
      y->m_color = z->m_color;
    }

    m_size--;
    if (yOriginalColor == Node::Color::BLACK)
      deleteFixup(x);
    delete org;
  }

  void clear() {
    while (m_root != m_nil)
      remove(m_root);
  }

 public:
  TreeMap() {
    m_nil = new Node();
    m_root = m_nil;
    m_size = 0;
  }

  ~TreeMap() {
    clear();
    delete m_nil;
  }

  TreeMap(std::initializer_list<value_type> list) : TreeMap() {
    for (const value_type& val : list) {
      ValueNode* x = new ValueNode(val.first, val.second);
      insert(x);
    }
  }

  TreeMap(const TreeMap& other) : TreeMap() {
    for (auto i = other.begin(); i != other.end(); ++i) {
      ValueNode* x = new ValueNode(i->first, i->second);
      insert(x);
    }
  }

  TreeMap(TreeMap&& other) : m_root(nullptr), m_nil(nullptr), m_size(0) {
    *this = std::move(other);
  }

  TreeMap& operator=(const TreeMap& other) {
    if (this != &other) {
      clear();
      for (auto i = other.begin(); i != other.end(); ++i) {
        ValueNode* x = new ValueNode(i->first, i->second);
        insert(x);
      }
    }

    return *this;
  }

  TreeMap& operator=(TreeMap&& other) {
    if (this != &other) {
      if (m_nil) {
        clear();
        delete m_nil;
      }

      m_root = other.m_root;
      m_nil = other.m_nil;
      m_size = other.m_size;

      other.m_root = nullptr;
      other.m_nil = nullptr;
      other.m_size = 0;
    }

    return *this;
  }

  bool isEmpty() const { return !m_size; }

  mapped_type& operator[](const key_type& key) {
    Node* ptr = search(key);
    if (ptr == m_nil) {
      ptr = new ValueNode(key, mapped_type{});
      insert(static_cast<ValueNode*>(ptr));
    }

    return static_cast<ValueNode*>(ptr)->m_value.second;
  }

  const mapped_type& valueOf(const key_type& key) const {
    Node* x = search(key);
    if (x == m_nil)
      throw std::out_of_range("Node with given key does not exist.");

    return static_cast<ValueNode*>(x)->m_value.second;
  }

  mapped_type& valueOf(const key_type& key) {
    Node* x = search(key);
    if (x == m_nil)
      throw std::out_of_range("Node with given key does not exist.");

    return static_cast<ValueNode*>(x)->m_value.second;
  }

  const_iterator find(const key_type& key) const {
    Node* x = search(key);
    return const_iterator(*this, x);
  }

  iterator find(const key_type& key) {
    Node* x = search(key);
    return iterator(*this, x);
  }

  void remove(const key_type& key) {
    Node* x = search(key);
    if (x == m_nil)
      throw std::out_of_range("Node with given key does not exist.");
    remove(x);
  }

  void remove(const const_iterator& it) {
    Node* x = search(it->first);
    if (x == m_nil)
      throw std::out_of_range("Node with given key does not exist.");
    remove(x);
  }

  size_type getSize() const { return m_size; }

  bool operator==(const TreeMap& other) const {
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

  bool operator!=(const TreeMap& other) const { return !(*this == other); }

  iterator begin() {
    Node* x = m_root;
    while (x->m_left != m_nil)
      x = x->m_left;
    return iterator(*this, x);
  }

  iterator end() { return iterator(*this, m_root->m_p); }

  const_iterator cbegin() const {
    Node* x = m_root;
    while (x->m_left != m_nil)
      x = x->m_left;
    return const_iterator(*this, x);
  }

  const_iterator cend() const { return const_iterator(*this, m_root->m_p); }

  const_iterator begin() const { return cbegin(); }

  const_iterator end() const { return cend(); }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator {
 public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;
  using node_pointer = typename TreeMap::Node*;
  using valuenode_pointer = typename TreeMap::ValueNode*;

 protected:
  const TreeMap& m_source;
  node_pointer m_ptr;

 public:
  explicit ConstIterator(const TreeMap& source, node_pointer ptr)
      : m_source(source), m_ptr(ptr) {}

  ConstIterator(const ConstIterator& other)
      : m_source(other.m_source), m_ptr(other.m_ptr) {}

  ConstIterator& operator++() {
    if (m_ptr == m_source.m_nil)
      throw std::out_of_range("Next node does not exist.");

    if (m_ptr->m_right != m_source.m_nil) {
      node_pointer tmp_x = m_ptr->m_right;
      while (tmp_x->m_left != m_source.m_nil)
        tmp_x = tmp_x->m_left;
      m_ptr = tmp_x;
      return *this;
    }

    node_pointer tmp_y = m_ptr->m_p;
    node_pointer tmp_x = m_ptr;

    while (tmp_y != m_source.m_nil && tmp_x == tmp_y->m_right) {
      tmp_x = tmp_y;
      tmp_y = tmp_y->m_p;
    }

    m_ptr = tmp_y;
    return *this;
  }

  ConstIterator operator++(int) {
    ConstIterator tmp(*this);
    ++(*this);
    return tmp;
  }

  ConstIterator& operator--() {
    node_pointer min = m_source.m_root;
    while (min->m_left != m_source.m_nil)
      min = min->m_left;

    if (m_ptr == min)
      throw std::out_of_range("Previous node does not exist.");

    if (m_ptr->m_left != m_source.m_nil) {
      node_pointer tmp_x = m_ptr->m_left;
      while (tmp_x->m_right != m_source.m_nil)
        tmp_x = tmp_x->m_right;
      m_ptr = tmp_x;
      return *this;
    }

    node_pointer tmp_y = m_ptr->m_p;
    node_pointer tmp_x = m_ptr;

    while (tmp_y != m_source.m_nil && tmp_x == tmp_y->m_left) {
      tmp_x = tmp_y;
      tmp_y = tmp_y->m_p;
    }

    m_ptr = tmp_y;
    return *this;
  }

  ConstIterator operator--(int) {
    ConstIterator tmp(*this);
    --(*this);
    return tmp;
  }

  reference operator*() const {
    if (m_ptr == m_source.m_nil)
      throw std::out_of_range("Iterator out of range.");

    return static_cast<valuenode_pointer>(m_ptr)->m_value;
  }

  pointer operator->() const { return &this->operator*(); }

  bool operator==(const ConstIterator& other) const {
    return m_ptr == other.m_ptr;
  }

  bool operator!=(const ConstIterator& other) const {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator
    : public TreeMap<KeyType, ValueType>::ConstIterator {
 public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;
  using node_pointer = typename TreeMap::Node*;

  explicit Iterator(const TreeMap& source, node_pointer ptr)
      : ConstIterator(source, ptr) {}

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

#endif /* AISDI_MAPS_MAP_H */
