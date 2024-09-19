#ifndef PROC_H
#define PROC_H
#include "hashtable.h"
typedef struct {
    int pid;
    char name[256];
    char user[64];
    float cpu_usage;
    long memory;
} ProcessInfo;

void get_process_user(int pid, char *user);
float get_cpu_usage(int pid, HashTable *table, uint64_t total_cpu_time_diff);
long get_memory_usage(int pid);
int read_process_info(ProcessInfo *processes, HashTable *table, uint64_t total_cpu_time_diff, int num_cores);
void get_load_average(float *load1, float *load5, float *load15);
void get_uptime(double *uptime);
int get_task_count();


#endif
