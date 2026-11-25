#ifndef HASH_MAP_H
#define HASH_MAP_H
#include <string>

using namespace std;
class HashMap{
    struct Entry{string key;int value;Entry*next;Entry(const string&k,int v):key(k),value(v),next(0){}};
    Entry**buckets;int capacity;int count;
    unsigned long hash(const string&s)const{unsigned long h=5381;for(size_t i=0;i<s.size();++i)h=((h<<5)+h)+(unsigned char)s[i];return h;}
    void destroy(){for(int i=0;i<capacity;++i){Entry*p=buckets[i];while(p){Entry*n=p->next;delete p;p=n;}}delete[]buckets;}
    void rehash(){int oldCap=capacity;Entry**old=buckets;capacity*=2;buckets=new Entry*[capacity];for(int i=0;i<capacity;++i)buckets[i]=0;for(int i=0;i<oldCap;++i){Entry*p=old[i];while(p){Entry*n=p->next;int b=(int)(hash(p->key)%capacity);p->next=buckets[b];buckets[b]=p;p=n;}}delete[]old;}
    void copyFrom(const HashMap&o){for(int i=0;i<o.capacity;++i)for(Entry*p=o.buckets[i];p;p=p->next)put(p->key,p->value);}
public:
    explicit HashMap(int c=16):capacity(c>0?c:16),count(0){buckets=new Entry*[capacity];for(int i=0;i<capacity;++i)buckets[i]=0;}
    ~HashMap(){destroy();}
    HashMap(const HashMap&o):capacity(o.capacity),count(0){buckets=new Entry*[capacity];for(int i=0;i<capacity;++i)buckets[i]=0;copyFrom(o);}
    HashMap&operator=(const HashMap&o){if(this!=&o){destroy();capacity=o.capacity;count=0;buckets=new Entry*[capacity];for(int i=0;i<capacity;++i)buckets[i]=0;copyFrom(o);}return *this;}
    void put(const string&key,int value){int b=(int)(hash(key)%capacity);for(Entry*p=buckets[b];p;p=p->next)if(p->key==key){p->value=value;return;}Entry*n=new Entry(key,value);n->next=buckets[b];buckets[b]=n;++count;if(count*10>capacity*7)rehash();}
    bool get(const string&key,int&out)const{int b=(int)(hash(key)%capacity);for(Entry*p=buckets[b];p;p=p->next)if(p->key==key){out=p->value;return true;}return false;}
    bool contains(const string&key)const{int v;return get(key,v);}int size()const{return count;}
};
#endif
