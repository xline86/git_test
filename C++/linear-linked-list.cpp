#include <iostream>
#include <string>
using namespace std;

template <typename T>
class List;
void test();

int main()
{
    test();
}

// ---------------------------
// Node Class Template
// ---------------------------
template <typename T>
class Node
{
public:
    T data;
    Node<T>* prev;
    Node<T>* next;

    Node(T data);
    // NodeはListから操作, new, deleteはListから行う
    Node() = delete;
    ~Node() {}
};

template <typename T>
Node<T>::Node(T data)
{
    this->data = data;
    this->next = nullptr;
    this->prev = nullptr;
}

// ---------------------------
// List Class Template
// ---------------------------
template <typename T>
class List
{
protected:
    int size;
    Node<T>* head;
    Node<T>* tail;

public:
    List();
    virtual ~List();
    void push(T data);
    void pop();
    void insert(T data, int index);
    void del(int index);
    int get_size() { return size; }
    T get_head() { return head->data; }
    T get_tail() { return tail->data; }
    bool is_empty() { return size == 0; }

    template <typename... Args>
    void emplace(Args&&... args);

    void operate(std::function<void(T&, int)> func);
    void operate(std::function<void(const T&, int)> func) const;

    // イテレータクラス
    class Iterator {
        Node<T>* current;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;

        Iterator(Node<T>* node) : current(node) {}

        reference operator*() const { return current->data; }
        pointer operator->() const { return &(current->data); }

        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return !(*this == other); }
    };

    // const イテレータクラス
    class ConstIterator {
        const Node<T>* current;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T*;
        using reference         = const T&;

        ConstIterator(const Node<T>* node) : current(node) {}

        reference operator*() const { return current->data; }
        pointer operator->() const { return &(current->data); }

        ConstIterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const ConstIterator& other) const { return current == other.current; }
        bool operator!=(const ConstIterator& other) const { return !(*this == other); }
    };
};

template <typename T>
List<T>::List()
{
    size = 0;
    head = nullptr;
    tail = nullptr;
}

template <typename T>
List<T>::~List()
{
    Node<T>* cur = head;
    while (cur != nullptr)
    {
        Node<T>* next = cur->next;
        delete cur;
        cur = next;
    }
}

template <typename T>
void List<T>::push(T data)
{
    Node<T>* new_node = new Node<T>(data);
    if (head == nullptr)
    {
        head = tail = new_node;
    }
    else
    {
        tail->next = new_node;
        new_node->prev = tail;
        tail = new_node;
    }

    size++;
}

template <typename T>
void List<T>::pop()
{
    if (tail == nullptr)
    {
        return;
    }
    if (head == tail)
    {
        delete head;
        head = tail = nullptr;
    }
    else
    {
        Node<T>* to_delete = tail;
        tail = tail->prev;
        if (tail != nullptr)
        {
            tail->next = nullptr;
        }
        delete to_delete;
    }

    size--;
}

template <typename T>
void List<T>::insert(T data, int index)
{
    if (index < 0 || index > size)
    {
        return;
    }

    Node<T>* new_node = new Node<T>(data);
    if (index == 0)
    {
        new_node->next = head;
        if (head != nullptr)
        {
            head->prev = new_node;
        }
        head = new_node;
        if (tail == nullptr) // If the list was empty
        {
            tail = new_node;
        }
    }
    else if (index == size) // Insert at the end
    {
        tail->next = new_node;
        new_node->prev = tail;
        tail = new_node;
    }
    else
    {
        Node<T>* cur = head;
        for (int i = 0; i < index - 1; i++)
        {
            cur = cur->next;
        }
        new_node->next = cur->next;
        new_node->prev = cur;
        if (cur->next != nullptr)
        {
            cur->next->prev = new_node;
        }
        cur->next = new_node;
    }

    size++;
}

template <typename T>
void List<T>::del(int index)
{
    if (index < 0 || index >= size || head == nullptr)
    {
        return;
    }

    Node<T>* to_delete = head;
    for (int i = 0; i < index; i++)
    {
        to_delete = to_delete->next;
    }

    if (to_delete->prev != nullptr)
    {
        to_delete->prev->next = to_delete->next;
    }
    else
    {
        head = to_delete->next; // Deleting the head
    }

    if (to_delete->next != nullptr)
    {
        to_delete->next->prev = to_delete->prev;
    }
    else
    {
        tail = to_delete->prev; // Deleting the tail
    }

    delete to_delete;
    size--;
}

template <typename T>
template <typename... Args>
void List<T>::emplace(Args&&... args)
{
    push(T(std::forward<Args>(args)...));
}

template <typename T>
void List<T>::operate(std::function<void(T&, int)> func)
{
    Node<T>* cur = head;
    for (int i = 0; i < size; i++)
    {
        func(cur->data, i);
        cur = cur->next;
    }
}

template <typename T>
void List<T>::operate(std::function<void(const T&, int)> func) const
{
    Node<T>* cur = head;
    for (int i = 0; i < size; i++)
    {
        func(cur->data, i);
        cur = cur->next;
    }
}

template <typename T>
typename List<T>::Iterator begin() { return Iterator(head); }

template <typename T>
typename List<T>::Iterator end() { return Iterator(nullptr); }

template <typename T>
typename List<T>::ConstIterator cbegin() const { return ConstIterator(head); }

template <typename T>
typename List<T>::ConstIterator cend() const { return ConstIterator(nullptr); }

// ---------------------------
// Student Class
// ---------------------------
class Student
{
    int no;
    string name;

public:
    Student() : no(0), name("") {}
    Student(int no, string name) : no(no), name(name) {}
    ~Student() {}

    void disp_student_list() const { cout << "No. " << no << ", name: " << name << endl; }
};

void disp_student_list(Student& s, int index)
{
    s.disp_student_list();
}

void test()
{
    List<Student> list;

    Student s1 = Student(1, "mike");
    Student s2 = Student(2, "bob");

    list.push(s1);
    list.push(s2);
    list.push(Student(3, "alice"));

    list.operate(disp_student_list);
    cout<<endl;

    list.pop();
    list.operate(disp_student_list);
    cout<<endl;

    list.insert(Student(4, "john"), 1);
    list.operate(disp_student_list);
    cout<<endl;

    list.emplace(5, "jane");

    list.del(2);
    list.operate(disp_student_list);
    cout<<endl;
}
