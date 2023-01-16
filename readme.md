# FOS - Custom OS with Dynamic Memory Allocation, CPU Utilization, Parallel Processing, and Shared Resource Management

## Introduction

This project is a university project for the Operating Systems (OS) subject. The objective of the project is to implement various functions in a custom-made OS that addresses issues related to dynamic memory allocation, CPU utilization, parallel processing, and shared resource management.

## Team Members

- Rania Ahmed
- Youssif gamal
- Ahmed Mohammed
- Mostafa Alaa

## Features Implemented

- Dynamic memory allocation: A custom memory management system that allocates and deallocates memory dynamically, based on the needs of the running programs.
- CPU utilization: A scheduler that dynamically adjusts the allocation of CPU resources to ensure that all programs are executed efficiently.
- Parallel processing: The ability to run multiple programs simultaneously, utilizing multiple cores or processors.
- Shared resource management: A mechanism for managing access to shared resources, such as files and printers, by multiple programs.

## How to Run

Clone the repository to your local machine.

git clone https://github.com/Rania154/FOS.git

## Memory Management Techniques Used

1. **Static Memory Allocation**: In this technique, the memory is divided into fixed-sized blocks at the time of system design, and the blocks are then allocated to processes as needed. The main advantage of this technique is that it is simple and easy to implement. However, it is not very efficient since it does not allow for the reallocation of memory once it has been allocated.

2. **Dynamic Memory Allocation**: In this technique, the memory is allocated to a process as and when required. The main advantage of this technique is that it allows for more efficient use of memory. There are several different algorithms that can be used for dynamic memory allocation, such as First-Fit, Best-Fit, and Worst-Fit.

3. **Paged Memory Management**: In this technique, the memory is divided into fixed-sized pages, and a page table is used to keep track of which pages are currently in use by which process. This technique allows for efficient memory management and also provides protection for processes, as each process only has access to its own pages.

4. **Segmented Memory Management**: In this technique, the memory is divided into segments, with each segment being a logical grouping of memory that represents a specific type of data or code. Each process is then allocated a set of segments, and the operating system uses a segment table to manage the segments and ensure that each process only has access to its own segments.

5. **Virtual Memory**: In this technique, the operating system uses a technique called "memory paging" to swap pages of memory to and from disk, this allows a process to have a large memory space without consuming all the physical memory. This technique allows for more efficient use of memory and also provides protection for processes, as each process only has access to its own virtual memory.

Each technique has its own advantages and disadvantages, and the choice of which technique to use depends on the specific requirements of the operating system and the system it is running on.

## Parallel Processing Techniques Used

Parallel processing techniques are used in operating systems to allow multiple programs to be executed simultaneously, utilizing multiple cores or processors. Some of the used parallel processing techniques include:

1. **First Come First Serve (FCFS)**: In this technique, the operating system executes the first program that arrives first. This technique is simple and easy to implement, but it does not take into account the priority of the programs or the length of their execution time.

2. **Round Robin**: In this technique, the operating system allocates a fixed time slice (time quantum) for each program, and then switches between them in a cyclic manner. This technique is more efficient than FCFS as it allows for the fair allocation of CPU time to all programs. However, it can lead to poor performance for long-running programs.

3. **Priority Scheduling**: In this technique, the operating system assigns a priority to each program, and then executes the program with the highest priority first. This technique allows for the efficient execution of important programs and ensures that they are executed before less important programs.

4. **Multi-level Feedback Queue (MFQ)**: In this technique, the operating system uses multiple queues to hold processes at different priority levels. Processes that have used a lot of CPU time will be moved to lower-priority queues and processes that have used less CPU time will be moved to higher-priority queues. This technique allows for the efficient execution of important programs and ensures that they are executed before less important programs.

Each technique has its own advantages and disadvantages, and the choice of which technique to use depends on the specific requirements of the operating system and the system it is running on.

## Synchronization Techniques Used

Synchronization techniques are used in operating systems to ensure that multiple processes or threads do not access shared resources at the same time, which can lead to data inconsistencies and other errors. Some of the used synchronization techniques include:

1. **Critical Section**: A critical section is a code segment that accesses shared resources. Only one process can execute a critical section at a time. The operating system uses locks or semaphores to enforce mutual exclusion and prevent multiple processes from executing the critical section at the same time.

2. **Semaphores**: A semaphore is a variable that is used to control access to shared resources. The value of a semaphore can be either 0 or 1, with 0 indicating that the resource is being used by another process and 1 indicating that the resource is available. Processes wait (block) on a semaphore when the value is 0, and are released (unblocked) when the value becomes 1.

Each technique has its own advantages and disadvantages, and the choice of which technique

## Deadlock Prevention Techniques in Operating Systems

Deadlocks occur when two or more processes are unable to proceed because they are waiting for each other to release a resource. Deadlock prevention techniques are used in operating systems to avoid or break deadlocks. Some of the most used deadlock prevention techniques include:

1. **Resource ordering**: In this technique, resources are ordered in some predefined way, and a process can only request resources in increasing order. This technique ensures that a process that requests resources in a different order than the predefined order will be blocked and will not be able to cause a deadlock.

2. **Resource allocation graph**: In this technique, a graph is used to represent the state of the system. The nodes in the graph represent processes, and the edges represent resources. This technique allows the operating system to detect a potential deadlock by checking for cycles in the graph.

3. **Deadlock detection**: In this technique, the operating system periodically checks for deadlocks by looking for a state where none of the processes can proceed. If a deadlock is detected, the operating system takes action to break the deadlock, such as by aborting one of the processes.

4. **Preemptive resource allocation**: In this technique, a process can be forcibly removed from a resource it is holding, so that another process can use it. This technique allows the operating system to break a deadlock by preempting resources from a process that is not making progress.

5. **Time-out**: In this technique, each process is given a time-out value, and if a process cannot acquire a resource within the time-out value, it is terminated. This technique allows the operating system to break a deadlock by aborting processes that are not able to acquire resources within a certain time frame.

Each technique has its own advantages and disadvantages, and the choice of which technique to use depends on the specific requirements of the operating system and the system it is running on.
