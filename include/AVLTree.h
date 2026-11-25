#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <iostream>
#include <string>

using namespace std;
class AVLTree{
    struct Node{string key;Node*left;Node*right;int height;Node(const string&k):key(k),left(0),right(0),height(1){}};
    Node*root;
    int h(Node*n)const{return n?n->height:0;}int balance(Node*n)const{return n?h(n->left)-h(n->right):0;}
    void update(Node*n){const int left=h(n->left), right=h(n->right);n->height=1+(left>right?left:right);}
    Node*rotateRight(Node*y){Node*x=y->left;Node*t=x->right;x->right=y;y->left=t;update(y);update(x);return x;}
    Node*rotateLeft(Node*x){Node*y=x->right;Node*t=y->left;y->left=x;x->right=t;update(x);update(y);return y;}
    Node*insertNode(Node*n,const string&k){if(!n)return new Node(k);if(k<n->key)n->left=insertNode(n->left,k);else if(k>n->key)n->right=insertNode(n->right,k);else return n;update(n);int b=balance(n);if(b>1&&k<n->left->key)return rotateRight(n);if(b< -1&&k>n->right->key)return rotateLeft(n);if(b>1&&k>n->left->key){n->left=rotateLeft(n->left);return rotateRight(n);}if(b< -1&&k<n->right->key){n->right=rotateRight(n->right);return rotateLeft(n);}return n;}
    bool containsNode(Node*n,const string&k)const{if(!n)return false;if(k==n->key)return true;return k<n->key?containsNode(n->left,k):containsNode(n->right,k);}
    void inorderNode(Node*n)const{if(!n)return;inorderNode(n->left);cout<<n->key<<"\n";inorderNode(n->right);}
    void destroy(Node*n){if(!n)return;destroy(n->left);destroy(n->right);delete n;}
public:
    AVLTree():root(0){}~AVLTree(){destroy(root);}AVLTree(const AVLTree&)=delete;AVLTree&operator=(const AVLTree&)=delete;
    void clear(){destroy(root);root=0;} void insert(const string&k){root=insertNode(root,k);}bool contains(const string&k)const{return containsNode(root,k);}void printSorted()const{inorderNode(root);}
};
#endif
