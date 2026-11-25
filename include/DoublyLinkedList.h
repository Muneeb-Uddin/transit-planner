#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H
#include <stdexcept>

using namespace std;
template<typename T> class DoublyLinkedList {
    struct Node{T data;Node*prev;Node*next;Node(const T&v):data(v),prev(0),next(0){}};
    Node*head;Node*tail;int count;
    void copyFrom(const DoublyLinkedList&o){for(Node*p=o.head;p;p=p->next)push_back(p->data);}
public:
    DoublyLinkedList():head(0),tail(0),count(0){}
    ~DoublyLinkedList(){clear();}
    DoublyLinkedList(const DoublyLinkedList&o):head(0),tail(0),count(0){copyFrom(o);}
    DoublyLinkedList&operator=(const DoublyLinkedList&o){if(this!=&o){clear();copyFrom(o);}return *this;}
    void push_front(const T&v){Node*n=new Node(v);n->next=head;if(head)head->prev=n;else tail=n;head=n;++count;}
    void push_back(const T&v){Node*n=new Node(v);n->prev=tail;if(tail)tail->next=n;else head=n;tail=n;++count;}
    T pop_front(){if(!head)throw out_of_range("DoublyLinkedList empty");Node*n=head;T v=n->data;head=head->next;if(head)head->prev=0;else tail=0;delete n;--count;return v;}
    T pop_back(){if(!tail)throw out_of_range("DoublyLinkedList empty");Node*n=tail;T v=n->data;tail=tail->prev;if(tail)tail->next=0;else head=0;delete n;--count;return v;}
    const T& back()const{if(!tail)throw out_of_range("DoublyLinkedList empty");return tail->data;}
    int size()const{return count;}bool isEmpty()const{return count==0;}
    void clear(){while(head){Node*n=head;head=head->next;delete n;}tail=0;count=0;}
};
#endif
