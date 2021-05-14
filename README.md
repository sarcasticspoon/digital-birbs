# digital-birbs

Part 1:

1. In order to ensure that process_create works reliably and can be called at any time, we need to ensure mutual exclusion on the process queue. The two processes that manipualte the queue are process_create and process_select. Since we are assuming that we are running this code on a uniprocessor, preventing interrupts at any point in the two functions can ensure mutual exclusion. process_select is already protected by the functions that call it, so all we have to do is add disable and enable interrupt calls to process_create.

2. In order to ensure that we can free the space that is allocated to each process, we need to store the base pointer and call free on it. We achieve this by including a pointer for the base pointer in the current proc and setting it by passing a pointer to the process struct into the process_init function, and letting process_init save the base pointer. Subsequently, we call free on the base pointer in process_terminate.

We have two test cases to demonstrate that:
A) process_create can be called at any time.
B) processes are successfully freed.