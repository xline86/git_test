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
    Node<T>* next;

    Node(T data);
    // NodeはListから操作, new, deleteはListから行う
    Node() = delete;
    // Node() {}
    ~Node() {}
};

template <typename T>
Node<T>::Node(T data)
{
    this->data = data;
    this->next = nullptr;
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
    Node<T>* node = new Node<T>(data);
    if (head == nullptr)
    {
        head = tail = node;
    }
    else
    {
        tail->next = node;
        tail = node;
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
        Node<T>* cur = head;
        while (cur->next != tail)
        {
            cur = cur->next;
        }
        delete tail;
        this->tail = cur;
        cur->next = nullptr;
    }
    size--;
}

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

    void disp_student() const { cout << "No. " << no << ", name: " << name << endl; }
};

// ---------------------------
// myList Class
// ---------------------------
class myList : public List<Student>
{
public:
    void disp_List()
    {
        Node<Student>* cur = List<Student>::head;
        while (cur != nullptr)
        {
            cur->data.disp_student();
            cur = cur->next;
        }
    }
};

void test()
{
    myList list;

    Student s1 = Student(1, "mike");
    Student s2 = Student(2, "bob");

    list.push(s1);
    list.push(s2);

    list.disp_List();

    list.pop();
    list.disp_List();
}
