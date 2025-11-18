# OS Process Scheduler & Memory Management Simulator

This project is a C-based **operating system scheduler and memory manager simulator**.  
It models CPU scheduling, process lifecycle transitions, I/O blocking, and fixed-partition
memory allocation, providing detailed logs and performance metrics.

Designed to strengthen low-level **systems**, **embedded**, and **kernel-style**
development skills—relevant for radio platform software, RTOS, and telecom environments.

---

## Features

### CPU Scheduling Algorithms
- **FCFS – First Come First Served**
- **External Priority Scheduling**
- **Round Robin** (configurable quantum)

### Complete Process Lifecycle
NEW → READY → RUNNING → WAITING → READY → TERMINATED

- Preemption  
- CPU burst tracking  
- Timestamped event logging  

### I/O Blocking Simulation
- Custom I/O frequency & duration  
- WAITING queue  
- Automatic return to READY  

### Fixed-Partition Memory Management
- Static partition table  
- Allocation & deallocation on arrival/termination  
- Logs free/used memory and fragmentation  

### Performance Metrics
- Throughput  
- Average turnaround time  
- Total & average waiting time  
- CPU burst statistics  
- I/O response time  

---

## Why This Matters

This project demonstrates key skills used in **radio software, embedded systems, and Linux-based development**, including:

- C systems programming  
- Working with queues, event loops, and state machines  
- OS scheduling logic  
- Memory management  
- Debugging under deterministic timing constraints  
- Understanding kernel-level process behavior  
