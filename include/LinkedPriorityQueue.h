#ifndef LINKED_PRIORITY_QUEUE_H
#define LINKED_PRIORITY_QUEUE_H

#include "MinHeap.h"

class LinkedPriorityQueue {
    struct Node {
        HeapItem item;
        Node* next;
        Node(int value, long long priority, Node* after)
            : item(value, priority), next(after) {}
    };

    Node* head;

public:
    LinkedPriorityQueue() : head(nullptr) {}
    LinkedPriorityQueue(const LinkedPriorityQueue&) = delete;
    LinkedPriorityQueue& operator=(const LinkedPriorityQueue&) = delete;

    ~LinkedPriorityQueue() {
        while (head) extractMin();
    }

    void insert(int value, long long priority) {
        Node** position = &head;
        while (*position && (*position)->item.priority <= priority)
            position = &(*position)->next;
        *position = new Node(value, priority, *position);
    }

    HeapItem extractMin() {
        if (!head) throw out_of_range("Priority queue is empty");
        Node* first = head;
        const HeapItem item = first->item;
        head = first->next;
        delete first;
        return item;
    }

    bool isEmpty() const { return head == nullptr; }
};

#endif
