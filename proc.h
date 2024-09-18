#ifndef PROC_H
#define PROC_H
typedef struct {
    int pid;
    char name[256];
    float cpu_usage;
    long memory;
} ProcessInfo;

float get_cpu_usage(int pid);
long get_memory_usage(int pid);
int read_process_info(ProcessInfo *processes);
void get_load_average(float *load1, float *load5, float *load15);
void get_uptime(double *uptime);
int get_task_count();


#endif
