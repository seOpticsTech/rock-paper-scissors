//
// Created by user on 19/02/2026.
//

#ifndef GAME_CYCLICLIST_H
#define GAME_CYCLICLIST_H

#include <cstddef>
#include <initializer_list>
#include <list>
#include <vector>

template <typename T>
class CyclicList {
    private:
    struct Node {
        T value;
        Node* next;
        Node* prev;
    };

    Node* head;
    Node* tail;
    size_t count;

    void linkSingle(Node* node) {
        node->next = node;
        node->prev = node;
        head = node;
        tail = node;
        count = 1;
    }

    void insertBeforeNode(Node* pos, Node* node) {
        if (pos == nullptr) {
            if (head == nullptr) {
                linkSingle(node);
                return;
            }
            node->next = head;
            node->prev = tail;
            tail->next = node;
            head->prev = node;
            tail = node;
            ++count;
            return;
        }

        if (pos == head) {
            node->next = head;
            node->prev = tail;
            tail->next = node;
            head->prev = node;
            head = node;
            ++count;
            return;
        }

        Node* before = pos->prev;
        node->next = pos;
        node->prev = before;
        before->next = node;
        pos->prev = node;
        ++count;
    }

    void eraseNode(Node* node) {
        if (node == nullptr || head == nullptr) {
            return;
        }

        if (head == tail) {
            delete node;
            head = nullptr;
            tail = nullptr;
            count = 0;
            return;
        }

        Node* next = node->next;
        Node* prev = node->prev;
        prev->next = next;
        next->prev = prev;
        if (node == head) {
            head = next;
        }
        if (node == tail) {
            tail = prev;
        }
        delete node;
        --count;
    }

    public:
    class iterator {
        public:
        iterator() : node(nullptr), head(nullptr), tail(nullptr), list(nullptr) {}
        iterator(Node* node, Node* head, Node* tail, CyclicList* list)
            : node(node), head(head), tail(tail), list(list) {}

        T& operator*() const { return node->value; }
        T* operator->() const { return &node->value; }

        iterator& operator++() {
            if (node == nullptr) {
                return *this;
            }
            if (node->next == head) {
                node = nullptr;
                return *this;
            }
            node = node->next;
            return *this;
        }

        iterator& operator--() {
            if (node == nullptr) {
                node = tail;
                return *this;
            }
            node = node->prev;
            return *this;
        }

        bool operator==(const iterator& other) const { return node == other.node; }
        bool operator!=(const iterator& other) const { return node != other.node; }
        CyclicList* operator!() const { return list; }

        private:
        Node* node;
        Node* head;
        Node* tail;
        CyclicList* list;
        friend class CyclicList;
    };

    class const_iterator {
        public:
        const_iterator() : node(nullptr), head(nullptr), tail(nullptr), list(nullptr) {}
        const_iterator(Node* node, Node* head, Node* tail, const CyclicList* list)
            : node(node), head(head), tail(tail), list(list) {}
        const_iterator(const iterator& it)
            : node(it.node), head(it.head), tail(it.tail), list(it.list) {}

        const T& operator*() const { return node->value; }
        const T* operator->() const { return &node->value; }

        const_iterator& operator++() {
            if (node == nullptr) {
                return *this;
            }
            if (node->next == head) {
                node = nullptr;
                return *this;
            }
            node = node->next;
            return *this;
        }

        const_iterator& operator--() {
            if (node == nullptr) {
                node = tail;
                return *this;
            }
            node = node->prev;
            return *this;
        }

        bool operator==(const const_iterator& other) const { return node == other.node; }
        bool operator!=(const const_iterator& other) const { return node != other.node; }
        const CyclicList* operator!() const { return list; }

        private:
        Node* node;
        Node* head;
        Node* tail;
        const CyclicList* list;
        friend class CyclicList;
    };

    CyclicList() : head(nullptr), tail(nullptr), count(0) {}

    CyclicList(const std::vector<T>& values) : head(nullptr), tail(nullptr), count(0) {
        for (const T& value : values) {
            push_back(value);
        }
    }

    CyclicList(const std::list<T>& values) : head(nullptr), tail(nullptr), count(0) {
        for (const T& value : values) {
            push_back(value);
        }
    }

    template <size_t N>
    CyclicList(const T (&values)[N]) : head(nullptr), tail(nullptr), count(0) {
        for (size_t i = 0; i < N; ++i) {
            push_back(values[i]);
        }
    }

    CyclicList(std::initializer_list<T> values) : head(nullptr), tail(nullptr), count(0) {
        for (const T& value : values) {
            push_back(value);
        }
    }

    CyclicList(const CyclicList& other) : head(nullptr), tail(nullptr), count(0) {
        for (const T& value : other) {
            push_back(value);
        }
    }

    CyclicList& operator=(const CyclicList& other) {
        if (this != &other) {
            clear();
            for (const T& value : other) {
                push_back(value);
            }
        }
        return *this;
    }

    CyclicList(CyclicList&& other) noexcept
        : head(other.head), tail(other.tail), count(other.count) {
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }

    CyclicList& operator=(CyclicList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            count = other.count;
            other.head = nullptr;
            other.tail = nullptr;
            other.count = 0;
        }
        return *this;
    }

    ~CyclicList() {
        clear();
    }

    bool empty() const { return count == 0; }
    size_t size() const { return count; }

    iterator begin() { return iterator(head, head, tail, this); }
    iterator end() { return iterator(nullptr, head, tail, this); }
    const_iterator begin() const { return const_iterator(head, head, tail, this); }
    const_iterator end() const { return const_iterator(nullptr, head, tail, this); }

    T& front() { return head->value; }
    const T& front() const { return head->value; }
    T& back() { return tail->value; }
    const T& back() const { return tail->value; }

    void push_back(const T& value) {
        Node* node = new Node{value, nullptr, nullptr};
        insertBeforeNode(nullptr, node);
    }

    void push_front(const T& value) {
        Node* node = new Node{value, nullptr, nullptr};
        insertBeforeNode(head, node);
    }

    iterator insert(iterator pos, const T& value) {
        Node* node = new Node{value, nullptr, nullptr};
        insertBeforeNode(pos.node, node);
        return iterator(node, head, tail, this);
    }

    iterator erase(iterator pos) {
        Node* node = pos.node;
        if (node == nullptr) {
            return end();
        }
        iterator next = pos;
        ++next;
        eraseNode(node);
        return next;
    }

    void clear() {
        if (head == nullptr) {
            return;
        }
        Node* current = head;
        for (size_t i = 0; i < count; ++i) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        tail = nullptr;
        count = 0;
    }
};

#endif //GAME_CYCLICLIST_H
