#include <cassert>
#include "../include/CircularLinkedList.h"

using namespace std;
int main(){CircularLinkedList<int>a;a.push_back(10);a.push_back(20);assert(a.pop_front()==10);assert(a.pop_front()==20);assert(a.isEmpty());}
