#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <forward_list>

namespace aisdi
{

    template<typename KeyType, typename ValueType>
    class TreeMap
    {
    public:
        using key_type = KeyType;
        using mapped_type = ValueType;
        using value_type = std::pair<const key_type, mapped_type>;
        using size_type = std::size_t;
        using reference = value_type &;
        using const_reference = const value_type&;

        class ConstIterator;

        class Iterator;

        using iterator = Iterator;
        using const_iterator = ConstIterator;
    private:
        // Non-value node
        class Node
        {
        public:
            enum Color
            {
                BLACK,
                RED
            };

            Node* m_p;
            Node* m_left;
            Node* m_right;
            Color m_color;

            Node() : m_p(this), m_left(this), m_right(this), m_color(Color::BLACK) {}
            virtual ~Node() = default;
        };

        // Value node
        class ValueNode : public Node
        {
        public:
            value_type m_value;

            ValueNode(const key_type& key, const mapped_type& value) : Node(), m_value(key, value) {}
        };

    private:
        // Members
        Node* m_root;
        Node* m_nil;
        size_type m_size;

    private:
        // Methods
        Node* search(key_type key) const
        {
            Node* ptr = m_root;
            while (ptr != m_nil && static_cast<ValueNode*>(ptr)->m_value.first != key)
                if (key < static_cast<ValueNode*>(ptr)->m_value.first)
                    ptr = ptr->m_left;
                else
                    ptr = ptr->m_right;
            return ptr;
        }

        void insert(ValueNode* z)
        {
            Node* y = m_nil;
            Node* x = m_root;

            while (x != m_nil)
            {
                y = x;
                if (z->m_value.first < static_cast<ValueNode*>(x)->m_value.first)
                    x = x->m_left;
                else
                    x = x->m_right;
            }
            z->m_p = y;

            if (y == m_nil)
            {
                m_root = z;
                m_nil->m_left = m_root;
                m_nil->m_right = m_root;
            }
            else if (z->m_value.first < static_cast<ValueNode*>(y)->m_value.first)
                y->m_left = z;
            else
                y->m_right = z;

            z->m_left = m_nil;
            z->m_right = m_nil;
            z->m_color = Node::Color::RED;

            m_size++;
            // insertFixup(z);
        }



    public:
        TreeMap() {
            m_nil = new Node();
            m_root = m_nil;
            m_size = 0;
        }

        TreeMap(std::initializer_list<value_type> list)
        {
            (void) list; // disables "unused argument" warning, can be removed when method is implemented.
            throw std::runtime_error("TODO");
        }

        TreeMap(const TreeMap &other)
        {
            (void) other;
            throw std::runtime_error("TODO");
        }

        TreeMap(TreeMap &&other)
        {
            (void) other;
            throw std::runtime_error("TODO");
        }

        TreeMap &operator=(const TreeMap &other)
        {
            (void) other;
            throw std::runtime_error("TODO");
        }

        TreeMap &operator=(TreeMap &&other)
        {
            (void) other;
            throw std::runtime_error("TODO");
        }

        bool isEmpty() const
        {
            return !m_size;
        }

        mapped_type& operator[](const key_type &key)
        {
            Node* ptr = search(key);
            if (ptr == m_nil)
            {
                ptr = new ValueNode(key, mapped_type{});
                insert(static_cast<ValueNode*>(ptr));
            }

            return static_cast<ValueNode*>(ptr)->m_value.second;
        }

        const mapped_type &valueOf(const key_type &key) const
        {
            (void) key;
            throw std::runtime_error("TODO");
        }

        mapped_type &valueOf(const key_type &key)
        {
            (void) key;
            throw std::runtime_error("TODO");
        }

        const_iterator find(const key_type &key) const
        {
            Node* x = search(key);
            return const_iterator(*this, x);
        }

        iterator find(const key_type &key)
        {
            Node* x = search(key);
            return iterator(*this, x);
        }

        void remove(const key_type &key)
        {
            (void) key;
            throw std::runtime_error("TODO");
        }

        void remove(const const_iterator &it)
        {
            (void) it;
            throw std::runtime_error("TODO");
        }

        size_type getSize() const
        {
            return m_size;
        }

        bool operator==(const TreeMap &other) const
        {
            (void) other;
            throw std::runtime_error("TODO");
        }

        bool operator!=(const TreeMap &other) const
        {
            return !(*this == other);
        }

        iterator begin()
        {
            Node* x = m_root; 
            while (x->m_left != m_nil)
                x = x->m_left;
            return iterator(*this, x);
        }

        iterator end()
        {
            return iterator(*this, m_root->m_p);
        }

        const_iterator cbegin() const
        {
            Node* x = m_root;         
            while (x->m_left != m_nil)
                x = x->m_left;
            return const_iterator(*this, x);
        }

        const_iterator cend() const
        {
            return const_iterator(*this, m_root->m_p);
        }

        const_iterator begin() const
        {
            return cbegin();
        }

        const_iterator end() const
        {
            return cend();
        }
    };

    template<typename KeyType, typename ValueType>
    class TreeMap<KeyType, ValueType>::ConstIterator
    {
    public:
        using reference = typename TreeMap::const_reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename TreeMap::value_type;
        using pointer = const typename TreeMap::value_type*;
        using node_pointer = typename TreeMap::Node*;
        using valuenode_pointer = typename TreeMap::ValueNode*;
    private:
        const TreeMap& m_source;
        node_pointer m_ptr;

    public:
        explicit ConstIterator(const TreeMap& source, node_pointer ptr) : m_source(source), m_ptr(ptr) {}

        ConstIterator(const ConstIterator &other) : m_source(other.m_source), m_ptr(other.m_ptr) {}

        ConstIterator& operator++()
        {
            if (m_ptr == m_source.m_nil)
                throw std::out_of_range("Next node does not exist.");

            if (m_ptr->m_right != m_source.m_nil)
            {
                node_pointer tmp_x = m_ptr->m_right;
                while (tmp_x->m_left != m_source.m_nil)
                    tmp_x = tmp_x->m_left;
                m_ptr = tmp_x;
                return *this;
            }

            node_pointer tmp_y = m_ptr->m_p;
            node_pointer tmp_x = m_ptr;

            while (tmp_y != m_source.m_nil && tmp_x == tmp_y->m_right)
            {
                tmp_x = tmp_y;
                tmp_y = tmp_y->m_p;
            }

            m_ptr = tmp_y;
            return *this;
        }

        ConstIterator operator++(int)
        {
            ConstIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        ConstIterator& operator--()
        {
            node_pointer min = m_source.m_root;
            while (min->m_left != m_source.m_nil)
                min = min->m_left;

            if (m_ptr == min)
                throw std::out_of_range("Previous node does not exist.");

            if (m_ptr->m_left != m_source.m_nil)
            {
                node_pointer tmp_x = m_ptr->m_left;
                while (tmp_x->m_right != m_source.m_nil)
                    tmp_x = tmp_x->m_right;
                m_ptr = tmp_x;
                return *this;
            }

            node_pointer tmp_y = m_ptr->m_p;
            node_pointer tmp_x = m_ptr;

            while (tmp_y != m_source.m_nil && tmp_x == tmp_y->m_left)
            {
                tmp_x = tmp_y;
                tmp_y = tmp_y->m_p;
            }

            m_ptr = tmp_y;
            return *this;
        }

        ConstIterator operator--(int)
        {
            ConstIterator tmp(*this);
            --(*this);
            return tmp;
        }

        reference operator*() const
        {
            if (m_ptr == m_source.m_nil)
                throw std::out_of_range("Iterator out of range.");

            return static_cast<valuenode_pointer>(m_ptr)->m_value;
        }

        pointer operator->() const
        {
            return &this->operator*();
        }

        bool operator==(const ConstIterator &other) const
        {
            return m_ptr == other.m_ptr;
        }

        bool operator!=(const ConstIterator &other) const
        {
            return !(*this == other);
        }
    };

    template<typename KeyType, typename ValueType>
    class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
    {
    public:
        using reference = typename TreeMap::reference;
        using pointer = typename TreeMap::value_type *;
        using node_pointer = typename TreeMap::Node*;

        explicit Iterator(const TreeMap& source, node_pointer ptr) : ConstIterator(source, ptr) {}

        Iterator(const ConstIterator &other)
                : ConstIterator(other) {}

        Iterator &operator++()
        {
            ConstIterator::operator++();
            return *this;
        }

        Iterator operator++(int)
        {
            auto result = *this;
            ConstIterator::operator++();
            return result;
        }

        Iterator &operator--()
        {
            ConstIterator::operator--();
            return *this;
        }

        Iterator operator--(int)
        {
            auto result = *this;
            ConstIterator::operator--();
            return result;
        }

        pointer operator->() const
        {
            return &this->operator*();
        }

        reference operator*() const
        {
            // ugly cast, yet reduces code duplication.
            return const_cast<reference>(ConstIterator::operator*());
        }
    };

}

#endif /* AISDI_MAPS_MAP_H */
