#include <cassert>
#include "../include/HashMap.h"

using namespace std;
int main(){HashMap h(2);h.put("Numaish",0);h.put("Saddar",1);h.put("Tower",2);int x=-1;assert(h.get("Saddar",x)&&x==1);h.put("Saddar",7);assert(h.get("Saddar",x)&&x==7);assert(!h.contains("Unknown"));HashMap c=h;assert(c.get("Tower",x)&&x==2);}
