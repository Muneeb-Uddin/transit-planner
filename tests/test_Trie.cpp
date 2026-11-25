#include <cassert>
#include <sstream>
#include "../include/Trie.h"

using namespace std;
int main(){Trie t;t.insert("Numaish");t.insert("North Nazimabad");assert(t.contains("Numaish"));assert(!t.contains("Numa"));}
