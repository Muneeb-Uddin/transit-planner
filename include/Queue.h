#ifndef QUEUE_H
#define QUEUE_H
#include "LinkedList.h"

using namespace std;
template<typename T> class Queue{LinkedList<T> list;public:void enqueue(const T&v){list.push_back(v);}T dequeue(){return list.pop_front();}T front()const{return list.front();}bool isEmpty()const{return list.isEmpty();}int size()const{return list.size();}};
#endif
