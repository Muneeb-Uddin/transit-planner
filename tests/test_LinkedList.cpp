#include <cassert>
#include "../include/LinkedList.h"

using namespace std;
int main(){LinkedList<int>a;a.push_back(2);a.push_front(1);a.push_back(3);assert(a.size()==3&&a.contains(2));assert(a.pop_front()==1);assert(a.remove(3));assert(!a.remove(9));LinkedList<int>b=a;b.push_back(4);assert(a.size()==1&&b.size()==2);}
