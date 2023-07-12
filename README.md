Monyreak Kit 
Assignment 3

use two condition variables: not_empty and not_full.
not_empty is signaled when an item is pushed to the queue,
and not_full is signaled when an item is popped from the queue. 
This ensures that queue_push will wait if the queue is full, and queue_pop will wait if the queue is empty. Mutex lock is used to synchronize access to the shared queue data.

The queue_t struct holds the queue's state:

data is a pointer to the array that holds the elements of the queue.
size is the size of the queue.
in and out are the indices where the next element will be inserted and removed, respectively.
count is the number of elements currently in the queue.
lock is a mutex used to ensure thread-safety.
not_empty and not_full are condition variables used to block threads when the queue is empty or full, respectively.
// functions 
queue_new(int size) is a function that creates a new queue of the specified size. It allocates memory for the queue and its data array, initializes the queue's state, and returns a pointer to the queue.

queue_delete(queue_t **q) is a function that deletes a queue. It destroys the mutex and condition variables, frees the memory allocated for the data array and the queue itself, and sets the pointer to the queue to NULL.

queue_push(queue_t *q, void *elem) is a function that inserts an element at the end of the queue. If the queue is full, it blocks until there is space available. It acquires the lock before modifying the queue's state and signals the not_empty condition variable after the element is inserted to wake up any threads waiting to remove an element.

queue_pop(queue_t *q, void **elem) is a function that removes an element from the front of the queue. If the queue is empty, it blocks until there is an element available. It acquires the lock before modifying the queue's state and signals the not_full condition variable after the element is removed to wake up any threads waiting to insert an element.