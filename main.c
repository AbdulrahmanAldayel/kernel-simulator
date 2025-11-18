// Assignment 2 Part II

// Abdulrahman Aldayel, 101142760
// Nicholas Thibault, 101172413

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

int global_timer = 0;


typedef struct pcb {		
    int process_id;
    int arrival_time;
    int remaining_cpu_time;
    int cpu_arrival_time;
    int io_frequency;
    int io_duration;
    int time_until_io;
    int priority;
    int size_of_process;
    int base_memory_location;
    struct pcb *next;
} pcb_t;

typedef struct {      		
    pcb_t *front;
    pcb_t *rear;
    int size;
} queue_t;





queue_t *initialize_queue(void) {


    queue_t *queue = malloc(sizeof(queue_t));     
    assert(queue != NULL);

    queue->front = NULL;				
    queue->rear = NULL; 
    queue->size = 0;

    return queue;
}

pcb_t *create_process_control_block(int process_id, int arrival_time, int total_cpu_time, int io_freq, int io_dur, int priority, int process_size) {


    pcb_t *pcb = malloc(sizeof(pcb_t));
    assert(pcb != NULL);

    pcb->process_id = process_id;
    pcb->arrival_time = arrival_time;
    pcb->remaining_cpu_time = total_cpu_time;
    pcb->cpu_arrival_time = 0;
    pcb->io_frequency = io_freq;
    pcb->io_duration = io_dur;
    pcb->next = NULL;
    pcb->priority = priority;
    pcb->time_until_io = io_freq;
    pcb->base_memory_location = -1;         
    pcb->size_of_process = process_size;     

    return pcb;
}





void enqueue(queue_t *queue, pcb_t *pcb) {


    if (queue->front == NULL) {		
        queue->front = pcb;
    } else {
        queue->rear->next = pcb;	
    }

    queue->rear = pcb;     			
    queue->size += 1;				
}

void dequeue(queue_t *queue, _Bool deallocate) {


    if (queue->size == 0) {    					
        printf("Queue is empty, cannot dequeue.");
        assert(false);
    }
     pcb_t *pcb_to_delete = queue->front;       

    if (deallocate) { 							
        queue->front = queue->front->next;		
        free(pcb_to_delete);          			
        pcb_to_delete = NULL;
    } else {
        queue->front = queue->front->next;
        pcb_to_delete->next = NULL;
        
    }

    if (queue->front == NULL){					
        queue->rear = NULL;
    }

    queue->size -= 1;     						
}





void clear_and_prepare_file(char writeFile[]) {


    FILE *f1 = fopen(writeFile, "w");       
    if (f1 == NULL) {
        printf("File was not opened");
        assert(false);
    }
    fclose(f1);
    f1 = NULL;
}

void log_state_transition(int transition_identifier, int process_id, char write_file[]) {

    FILE *f2 = fopen(write_file, "a");
    if (f2 == NULL) {
        printf("File was not opened");
        assert(false);
    }
    fprintf(f2, "%-14d %-14d ", global_timer, process_id);

    if ((transition_identifier > 5) || (transition_identifier < 0)) {
        printf("Incorrect transition identifier");
        assert(false);
    } else if (transition_identifier == 0) {
        fprintf(f2, "NEW            READY\n\n");  
    } else if (transition_identifier == 1) {
        fprintf(f2, "READY          RUNNING\n\n");
    } else if (transition_identifier == 2) {
        fprintf(f2, "RUNNING        WAITING\n\n");
    } else if (transition_identifier == 3) {
        fprintf(f2, "WAITING        READY\n\n");
    } else if (transition_identifier == 4) {
        fprintf(f2, "RUNNING        READY\n\n");
    } else {
        fprintf(f2, "RUNNING        TERMINATED\n\n");
    }
    fclose(f2);    					
    f2 = NULL;

}

void transition(int transition_identifier, queue_t *old_state, queue_t *new_state, char write_file[]) {

    
    log_state_transition(transition_identifier, old_state->front->process_id, write_file);

    enqueue(new_state, old_state->front);
    dequeue(old_state, false);
}



void fcfs_scheduler(int transition_type, queue_t *old_state, queue_t *new_state, char write_file[]) {

    transition(transition_type, old_state, new_state, write_file);
}

void external_priorities_scheduler(int transition_type, queue_t *old_state, queue_t *new_state, char write_file[]) {

    if (new_state->size == 0) {
        transition(transition_type, old_state, new_state, write_file);

    } else {
        pcb_t *prev = NULL;
        pcb_t *curr = new_state->front;
        pcb_t *process_to_schedule = old_state->front;
        _Bool scheduled = false;
        for (int i = 0; i < new_state->size; i++) {                
            if (curr->priority > process_to_schedule->priority) {   
                dequeue(old_state, false);
                process_to_schedule->next = curr;
                scheduled = true;
                new_state->size++;
                log_state_transition(transition_type, process_to_schedule->process_id, write_file);
                if (i == 0) {
                    new_state->front = process_to_schedule;
                } else {
                    prev->next = process_to_schedule;
                }
                break;
            }
            prev = curr;
            curr = curr->next;
        }

        if (!scheduled) {
            transition(transition_type, old_state, new_state, write_file);
        }
    }
}

void round_robin_scheduler(int transition_type, queue_t *old_state, queue_t *new_state, char write_file[]) {

    transition(transition_type, old_state, new_state, write_file);
}

void scheduler(int transition_type, int scheduler_type, queue_t *old_state, queue_t *new_state, char write_file[]) {

    if (scheduler_type == 0) {
        fcfs_scheduler(transition_type, old_state, new_state, write_file);
    } else if (scheduler_type == 1) {
        external_priorities_scheduler(transition_type, old_state, new_state, write_file);
    } else if (scheduler_type == 2) {
        round_robin_scheduler(transition_type, old_state, new_state, write_file);
    }
}




_Bool assign_memory(pcb_t *process, int memory[][2], int num_portions, char write_file[]) {

    int current_address = 0;
    _Bool allocated = false;

    int total_used_memory = 0;
    int num_used_portions = 0;
    int total_free_memory;
    int free_usable_memory = 0;

    for (int i = 0; i < num_portions; i++) {
        if ((memory[i][1] == 0) && (memory[i][0] >= process->size_of_process) && (!allocated)) {
            memory[i][1] = process->size_of_process;
            process->base_memory_location = current_address;
            allocated = true;
        }
        if (memory[i][1] != 0) {
            total_used_memory += memory[i][1];
            num_used_portions++;
        } else {
            free_usable_memory += memory[i][0];
        }
        current_address += memory[i][0];
    }
    total_free_memory = current_address - total_used_memory;

    if (allocated) {
        FILE *f6 = fopen(write_file, "a");
        assert(f6 != NULL);
        fprintf(f6, "MEMORY HAS BEEN ALLOCATED TO PROCESS %d\n", process->process_id);
        fprintf(f6, "Total used memory: %d Mb\n", total_used_memory);
        fprintf(f6, "Used memory portions: %d\nFree memory portions: %d\n", num_used_portions, num_portions - num_used_portions);
        fprintf(f6, "Total amount of free memory: %d Mb\n", total_free_memory);
        fprintf(f6, "Total amount of free usable memory: %d Mb\n\n", free_usable_memory);
        fclose(f6);
    }
    return allocated;
}

void release_memory(pcb_t *process, int memory[][2], int num_portions) {
    
    int current_address = 0;

    for (int i = 0; i < num_portions; i++) {
        if ((current_address == process->base_memory_location) && (memory[i][1] != 0)) {
            process->base_memory_location = -1;
            memory[i][1] = 0;
            break;
        }
        current_address += memory[i][0];
    }
}



int main(int argc, char *argv[]) {

    if ((argc < 6) || (argc > 6)) {
        printf("Incorrect number of arguments");
        assert(false);
    }

    int scheduler_type = atoi(argv[3]);
    int quantum;                   
    if (scheduler_type == 2) {    
        quantum = atoi(argv[4]);    
    } else {
        quantum = 1000000;
    }

    _Bool use_memory_management;
    if (atoi(argv[5]) != 0) {           
        use_memory_management = true;   
    } else {
        use_memory_management = false;
    }


    queue_t *new = initialize_queue();
    queue_t *ready = initialize_queue();
    queue_t *running = initialize_queue();
    queue_t *waiting = initialize_queue();
    queue_t *terminated = initialize_queue();

    int memory_portion_sizes[] = {0, 0, 0, 0};
    if (atoi(argv[5]) == 1) {
        memory_portion_sizes[0] = 500;
        memory_portion_sizes[1] = 250;
        memory_portion_sizes[2] = 150;
        memory_portion_sizes[3] = 100;
    } else if (atoi(argv[5]) == 2) {
        memory_portion_sizes[0] = 300;
        memory_portion_sizes[1] = 300;
        memory_portion_sizes[2] = 350;
        memory_portion_sizes[3] = 50;
    }

    int num_portions = sizeof(memory_portion_sizes) / sizeof(int);
    int main_memory[num_portions][2];
    for (int i = 0; i < num_portions; i++) {
        main_memory[i][0] = memory_portion_sizes[i];
        main_memory[i][1] = 0;
    }

    clear_and_prepare_file(argv[2]);

    FILE *f5 = fopen(argv[2], "a");
    assert(f5 != NULL);
    fprintf(f5, "%-14s %-14s %-14s %-14s\n\n", "Time", "pid", "Old State", "New State");
    fclose(f5);

    int total_cpu_burst_time = 0;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_time_between_io = 0;
    int num_io = 0;

    int process_id;
    int arrival_time;
    int total_cpu_time;
    int io_freq;
    int io_dur;
    int priority;
    int process_size;
    int num_processes = 0;

    FILE *f3 = fopen(argv[1], "r");
    assert(f3 != NULL);
    while (!feof(f3)) {
        fscanf(f3, "%d %d %d %d %d %d %d\n", &process_id, &arrival_time, &total_cpu_time, &io_freq, &io_dur, &priority, &process_size);
        num_processes++;
    }

    pcb_t *inputted_processes[num_processes];      

    rewind(f3);

    for (int i = 0; i < num_processes; i++) {
        fscanf(f3, "%d %d %d %d %d %d %d\n", &process_id, &arrival_time, &total_cpu_time, &io_freq, &io_dur, &priority, &process_size);
        inputted_processes[i] = create_process_control_block(process_id, arrival_time, total_cpu_time, io_freq, io_dur, priority, process_size);
        total_cpu_burst_time += inputted_processes[i]->remaining_cpu_time;
    }
    fclose(f3);


    int num_terminated = 0;
    int wait_until;
    queue_t *temp = initialize_queue();
    pcb_t *curr;
    pcb_t *prev;
    int num_filled_memory_portions = 0;
    _Bool first_loop;

    while (num_processes > num_terminated) {

        total_waiting_time += ready->size;
        if (waiting->size == 0) { total_time_between_io++; }

        int count = 0;
        for (int i = 0; i < num_processes; i++) {
            if (inputted_processes[i]->arrival_time == global_timer) {
                enqueue(new, inputted_processes[i]);
            }
        }

        if (use_memory_management) {
            if (num_filled_memory_portions < num_portions) {
                curr = new->front;
                prev = NULL;
                first_loop = true;
                while (curr != NULL) {
                    if (assign_memory(curr, main_memory, num_portions, argv[2])) {
                        if (first_loop) {
                            scheduler(0, scheduler_type, new, ready, argv[2]);
                            curr = new->front;
                        } else {
                            if (curr == new->rear) {
                                new->rear = prev;
                            }
                            prev->next = curr->next;
                            curr->next = NULL;
                            new->size--;
                            enqueue(temp, curr);
                            scheduler(0, scheduler_type, temp, ready, argv[2]);
                            curr = prev->next;
                        }
                        num_filled_memory_portions++;
                    } else {
                        prev = curr;
                        curr = curr->next;
                        first_loop = false;
                    }
                }
            }
        } else {       
            while (new->size != 0) {
                scheduler(0, scheduler_type, new, ready, argv[2]);
            }
        }

        if ((ready->size > 0) && (running->size == 0)) {
            transition(1, ready, running, argv[2]);
            running->front->cpu_arrival_time = global_timer;
        }

        if (running->size > 0) {

            if (running->front->remaining_cpu_time == 0) {
                total_turnaround_time += global_timer - running->front->arrival_time;
                transition(5, running, terminated, argv[2]);
                if (use_memory_management) {
                    release_memory(terminated->front, main_memory, num_portions);
                    num_filled_memory_portions--;
                }
                dequeue(terminated, true);
                num_terminated++;

            } else if (running->front->time_until_io == 0) {
                num_io++;
                running->front->time_until_io = running->front->io_frequency;
                transition(2, running, waiting, argv[2]);
                if (waiting->size == 1) {
                    wait_until = global_timer + waiting->front->io_duration;
                }

            } else if ((global_timer - running->front->cpu_arrival_time) > quantum)  {
                scheduler(4, scheduler_type, running, ready, argv[2]);
            } else {
                running->front->remaining_cpu_time -= 1;
                running->front->time_until_io -= 1;
            }
        }

        if ((global_timer == wait_until) && (waiting->size > 0)) {
            scheduler(3, scheduler_type, waiting, ready, argv[2]);
            if (waiting->size > 0) {
                wait_until = global_timer + waiting->front->io_duration;
            }
        }
        global_timer++;
    }

    free(new);
    free(ready);
    free(running);
    free(waiting);
    free(terminated);
    free(temp);

    FILE *f4 = fopen(argv[2], "a");
    assert(f4 != NULL);

    fprintf(f4, "\n\nNo. of Processes: %d\n\n", num_processes);

    fprintf(f4, "Throughput: %.2lf ms/process\n\n", (double) global_timer / num_processes);
    printf("Throughput: %.2lf ms/process\n\n", (double) global_timer / num_processes);

    fprintf(f4, "Avg. TAT: %.2lf ms/process\n\n", (double) total_turnaround_time / num_processes);
    printf("Avg. TAT: %.2lf ms/process\n\n", (double) total_turnaround_time / num_processes);

    fprintf(f4, "Tot. Wait Time: %d ms\n\n", total_waiting_time);
    printf("Tot. Wait Time: %d ms\n\n", total_waiting_time);

    fprintf(f4, "Avg. Wait Time: %.2lf ms/process\n\n", (double) total_waiting_time / num_processes);
    printf("Avg. Wait Time: %.2lf ms/process\n\n", (double) total_waiting_time / num_processes);

    fprintf(f4, "Avg. CPU Burst Time: %.2lf ms/process\n\n", (double) total_cpu_burst_time / num_processes);
    printf("Avg. CPU Burst Time: %.2lf ms/process\n\n", (double) total_cpu_burst_time / num_processes);

    if (num_io > 0) {
        fprintf(f4, "Avg. Response Time: %.2lf ms", (double) total_time_between_io / num_io);
        printf("Avg. Response Time: %.2lf ms", (double) total_time_between_io / num_io);
    }

    fclose(f4);
    return 0;
}
