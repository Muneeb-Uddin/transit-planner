#ifndef TRIE_H
#define TRIE_H

#include "DynamicArray.h"
#include <iostream>
#include <string>

class Trie {
    struct Node {
        Node* children[256];
        bool terminal;
        Node() : children(), terminal(false) {}
    };

    Node* root;

    static void destroy(Node* node) {
        if (!node) return;
        for (int i = 0; i < 256; ++i) destroy(node->children[i]);
        delete node;
    }

    const Node* findPrefix(const string& prefix) const {
        const Node* node = root;
        for (size_t i = 0; node && i < prefix.size(); ++i)
            node = node->children[static_cast<unsigned char>(prefix[i])];
        return node;
    }

    static void collect(const Node* node, string& prefix, DynamicArray<string>& matches, int limit) {
        if (!node || matches.size() >= limit) return;
        if (node->terminal) matches.push_back(prefix);
        for (int i = 0; i < 256 && matches.size() < limit; ++i) {
            if (!node->children[i]) continue;
            prefix += static_cast<char>(i);
            collect(node->children[i], prefix, matches, limit);
            prefix.pop_back();
        }
    }

public:
    Trie() : root(new Node()) {}
    ~Trie() { destroy(root); }
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    void clear() {
        Node* fresh = new Node();
        destroy(root);
        root = fresh;
    }

    void insert(const string& word) {
        Node* node = root;
        for (size_t i = 0; i < word.size(); ++i) {
            Node*& child = node->children[static_cast<unsigned char>(word[i])];
            if (!child) child = new Node();
            node = child;
        }
        node->terminal = true;
    }

    bool contains(const string& word) const {
        const Node* node = findPrefix(word);
        return node && node->terminal;
    }

    DynamicArray<string> suggestions(string prefix, int limit = 10) const {
        DynamicArray<string> matches;
        collect(findPrefix(prefix), prefix, matches, limit);
        return matches;
    }

    void suggest(const string& prefix, int limit = 10) const {
        const DynamicArray<string> matches = suggestions(prefix, limit);
        if (matches.isEmpty()) cout << "No suggestions.\n";
        for (int i = 0; i < matches.size(); ++i) cout << matches[i] << '\n';
    }
};

#endif
