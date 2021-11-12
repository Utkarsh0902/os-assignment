# Round robin scheduling simulation

## Specifications
* 1 Main process, 3 Child processes.
* The children communicate with the parent through a shared memory segment for **schedular related communication.**
* For other communication, they use pipes.
* Each child process needs to have 2 threads: one to actually perform the task, and one to perform the communication.

## Overview
1. The parent will **first initiate a semaphore**, and then create the child processes. This semaphore will be used to restrict running of the code before the shared memory is created by the parent.
2. Child processes will use the semaphore to get access to the shared block in a synchronised manner. They will then **start polling**. A child process will leave this polling state when the parent (schedular) communicates that it is the current process' turn to run.
3. The schedular will take input for the time quantum from the user (or cli arguments). It will then indicate which process needs to run or stop in the shared memory segment through : `run process_id` or `stop process_id`.
4. The concerned process will signal its task thread to run when it recieves the `run` command. It will then resume polling for the `stop` command. Once it receives the signal to stop, it will `"kill"` or stop the task thread mid-way. At the same time, it will **also look for communication from the task thread** to indicate if the task has been finished.
5. Once the `finish` signal from the task thread is recieved, the communication thread will indicate that to the schedular through the `done` flag and exit.

## Data structures required
1. Semaphore for initial access to the shared memory.
2. Running queue: To specify which process needs to run next.
3. `done` array: array of boolean values to specify if a process has finished its task.


