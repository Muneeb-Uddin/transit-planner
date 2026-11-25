#include <cassert>
#include "../include/Stack.h"

using namespace std;
int main(){Stack<int>s;s.push(1);s.push(2);assert(s.peek()==2);assert(s.pop()==2);assert(s.pop()==1);assert(s.isEmpty());}
