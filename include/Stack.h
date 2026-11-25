#ifndef STACK_H
#define STACK_H
#include "LinkedList.h"

using namespace std;
template<typename T> class Stack{LinkedList<T> list;public:void push(const T&v){list.push_front(v);}T pop(){return list.pop_front();}T peek()const{return list.front();}bool isEmpty()const{return list.isEmpty();}int size()const{return list.size();}};
#endif
