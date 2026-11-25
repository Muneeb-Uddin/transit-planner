#ifndef CIRCULAR_LINKED_LIST_H
#define CIRCULAR_LINKED_LIST_H
#include <stdexcept>

using namespace std;
template<typename T> class CircularLinkedList {
    struct Node{T data;Node*next;Node(const T&v):data(v),next(0){}};
    Node*tail;int count;
public:
    CircularLinkedList():tail(0),count(0){}
    ~CircularLinkedList(){clear();}
    void push_back(const T&v){Node*n=new Node(v);if(!tail){tail=n;n->next=n;}else{n->next=tail->next;tail->next=n;tail=n;}++count;}
    void rotate(){if(tail)tail=tail->next;}
    const T& front()const{if(!tail)throw out_of_range("Circular list empty");return tail->next->data;}
    CircularLinkedList(const CircularLinkedList&)=delete;
    CircularLinkedList& operator=(const CircularLinkedList&)=delete;
    T pop_front(){if(!tail)throw out_of_range("Circular list empty");Node*h=tail->next;T v=h->data;if(h==tail)tail=0;else tail->next=h->next;delete h;--count;return v;}
    int size()const{return count;}bool isEmpty()const{return count==0;}
    void clear(){while(tail)pop_front();}
};
#endif
