#ifndef MIN_HEAP_H
#define MIN_HEAP_H
#include "DynamicArray.h"
#include <stdexcept>

using namespace std;
struct HeapItem{int node;long long priority;HeapItem(int n=0,long long p=0):node(n),priority(p){}};
class MinHeap{
    DynamicArray<HeapItem> a;
    void swapItems(int i,int j){HeapItem t=a[i];a[i]=a[j];a[j]=t;}
    void siftUp(int i){while(i>0){int p=(i-1)/2;if(a[p].priority<=a[i].priority)break;swapItems(p,i);i=p;}}
    void siftDown(int i){for(;;){int l=2*i+1,r=2*i+2,s=i;if(l<a.size()&&a[l].priority<a[s].priority)s=l;if(r<a.size()&&a[r].priority<a[s].priority)s=r;if(s==i)break;swapItems(i,s);i=s;}}
public:
    void insert(int node,long long priority){a.push_back(HeapItem(node,priority));siftUp(a.size()-1);}
    HeapItem extractMin(){if(a.isEmpty())throw out_of_range("Heap is empty");HeapItem result=a[0];a[0]=a[a.size()-1];a.pop_back();if(!a.isEmpty())siftDown(0);return result;}
    bool isEmpty()const{return a.isEmpty();}int size()const{return a.size();}
};
#endif
