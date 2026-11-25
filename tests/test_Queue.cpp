#include <cassert>
#include "../include/Queue.h"

using namespace std;
int main(){Queue<int>q;q.enqueue(1);q.enqueue(2);assert(q.front()==1);assert(q.dequeue()==1);assert(q.dequeue()==2);assert(q.isEmpty());}
