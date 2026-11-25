#include <cassert>
#include "../include/MinHeap.h"

using namespace std;
int main(){MinHeap h;h.insert(1,30);h.insert(2,10);h.insert(3,20);assert(h.extractMin().node==2);assert(h.extractMin().priority==20);assert(h.extractMin().priority==30);assert(h.isEmpty());}
