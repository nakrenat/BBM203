#include "Queue.h"
#include <iostream>
#include <queue>

// Constructor to initialize an empty queue
Queue::Queue() {
    front = -1;
    rear = -1;
}

// Adds a province to the end of the queue
void Queue::enqueue(int province) {
    if((rear+1) % MAX_QUEUE_SIZE ==  front) {
        return;
    }
    rear = (rear + 1) % MAX_QUEUE_SIZE;
    if(front == -1) {
        front++;
    }
    data[rear] = province;
    // Implement circular structure
    // Add the province
}

// Removes and returns the front province from the queue
int Queue::dequeue() {
    if(front == -1) {
        return -1;
    }
    if(front == rear) {
        int temp = data[front];
        front = rear = -1;
        return temp;
    }
    int value = data[front];
    front = (front + 1) % MAX_QUEUE_SIZE;
    return value;

}

// Returns the front province without removing it
int Queue::peek() const {
    // Check if the queue is empty
    if (front == -1) {
        std::cerr << "Queue is empty: Cannot peek.\n";
        return -1; // Return an error value for empty queue
    }
    return data[front];
}

// Checks if the queue is empty
bool Queue::isEmpty() const {
    if(front == -1 && rear == -1) {
        return true;
    }else {
        return false;
    }

}

// Adds a priority neighboring province in a way that it will be dequeued before other non-priority neighbors
void Queue::enqueuePriority(int province) {
    if (rear >= MAX_QUEUE_SIZE - 1) {
        std::cerr << "Queue overflow error: Cannot enqueue more elements.\n";
        return;
    }

    // Shift all elements in the queue one position to the right to make space at the front
    for (int i = rear; i >= front; --i) {
        data[i + 1] = data[i];
    }

    // Insert the priority province at the front of the queue
    data[front] = province;

    // Update rear to reflect the addition of a new element
    ++rear;

    // If the queue was previously empty, update front as well
    if (front == -1) {
        front = 0;
    }
}

