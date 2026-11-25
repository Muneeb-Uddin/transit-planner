#include <cassert>
#include "../include/DoublyLinkedList.h"

using namespace std;
int main(){DoublyLinkedList<int>a;a.push_front(2);a.push_front(1);a.push_back(3);assert(a.pop_back()==3);assert(a.pop_front()==1);assert(a.size()==1);}
