#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <iostream>
#include <stdexcept>

using namespace std;
template <typename T> class LinkedList {
private:
    struct Node { T data; Node* next; explicit Node(const T& v):data(v),next(0){} };
    Node* head; Node* tail; int count;
    void copyFrom(const LinkedList& other) { for(Node* p=other.head;p;p=p->next) push_back(p->data); }
public:
    LinkedList():head(0),tail(0),count(0){}
    ~LinkedList(){ clear(); }
    LinkedList(const LinkedList& other):head(0),tail(0),count(0){copyFrom(other);}
    LinkedList& operator=(const LinkedList& other){if(this!=&other){clear();copyFrom(other);}return *this;}
    void push_front(const T& v){Node* n=new Node(v);n->next=head;head=n;if(!tail)tail=n;++count;}
    void push_back(const T& v){Node* n=new Node(v);if(tail)tail->next=n;else head=n;tail=n;++count;}
    T pop_front(){if(!head)throw out_of_range("LinkedList is empty");Node* n=head;T v=n->data;head=head->next;delete n;--count;if(!head)tail=0;return v;}
    bool remove(const T& v){Node* p=0;Node* c=head;while(c&&!(c->data==v)){p=c;c=c->next;}if(!c)return false;if(p)p->next=c->next;else head=c->next;if(c==tail)tail=p;delete c;--count;return true;}
    bool contains(const T& v)const{for(Node*p=head;p;p=p->next)if(p->data==v)return true;return false;}
    T& front(){if(!head)throw out_of_range("LinkedList is empty");return head->data;}
    const T& front()const{if(!head)throw out_of_range("LinkedList is empty");return head->data;}
    int size()const{return count;} bool isEmpty()const{return count==0;}
    void clear(){while(head){Node*n=head;head=head->next;delete n;}tail=0;count=0;}
    void print()const{for(Node*p=head;p;p=p->next)cout<<p->data<<(p->next?" -> ":"\n");if(!head)cout<<"(empty)\n";}
};
#endif
