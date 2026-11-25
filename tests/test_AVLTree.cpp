#include <cassert>
#include "../include/AVLTree.h"

using namespace std;
int main(){AVLTree t;t.insert("C");t.insert("B");t.insert("A");t.insert("D");t.insert("E");assert(t.contains("A")&&t.contains("E")&&!t.contains("Z"));}
