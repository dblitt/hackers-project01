#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>

#define MAX_PROCESSES 1024
#define DISPLAY_ROWS 20  // Number of rows for displaying processes

typedef struct {
    int pid;
    char name[256];
    float cpu_usage;
    long memory;
} ProcessInfo;

// Function to calculate CPU usage of the process
float get_cpu_usage(int pid) {
    char path[64], buffer[256];
    snprintf(path, 64, "/proc/%d/stat", pid);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) return 0.0;

    long utime, stime;
    fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld", &utime, &stime);
    fclose(fp);

    return (float)(utime + stime) / sysconf(_SC_CLK_TCK);
}

// Function to calculate memory usage of the process
long get_memory_usage(int pid) {
    char path[64], buffer[256];
    snprintf(path, 64, "/proc/%d/status", pid);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) return 0;

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strncmp(buffer, "VmRSS:", 6) == 0) {
            long memory;
            sscanf(buffer + 7, "%ld", &memory);
            fclose(fp);
            return memory;  // Return memory in kilobytes
        }
    }

    fclose(fp);
    return 0;
}

// Function to read process information
int read_process_info(ProcessInfo *processes) {
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir() error");
        return 0;
    }

    struct dirent *entry;
    int num_processes = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            int pid = atoi(entry->d_name);
            snprintf(processes[num_processes].name, sizeof(processes[num_processes].name), "/proc/%d/stat", pid);

            // Open the stat file to get the process name
            FILE *fp = fopen(processes[num_processes].name, "r");
            if (fp == NULL) continue;

            fscanf(fp, "%d %s", &processes[num_processes].pid, processes[num_processes].name);
            fclose(fp);

            processes[num_processes].cpu_usage = get_cpu_usage(pid);
            processes[num_processes].memory = get_memory_usage(pid);

            num_processes++;
            if (num_processes >= MAX_PROCESSES) break;  // Limit max processes to prevent overflow
        }
    }

    closedir(dir);
    return num_processes;
}
