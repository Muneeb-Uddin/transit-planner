#include <cassert>
#include "../include/DynamicArray.h"

using namespace std;
int main(){DynamicArray<int>a;for(int i=0;i<20;++i)a.push_back(i);assert(a.size()==20&&a[19]==19);DynamicArray<int>b=a;b[0]=99;assert(a[0]==0&&b[0]==99);DynamicArray<int>c;c=b;c[1]=88;assert(b[1]==1&&c[1]==88);}
