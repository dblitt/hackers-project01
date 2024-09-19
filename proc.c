#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <pwd.h>
#include "hashtable.h"
#include "cpuusage.h"

#define MAX_PROCESSES 1024
#define DISPLAY_ROWS 20  // Number of rows for displaying processes

typedef struct {
    int pid;
    char name[256];
    char user[64];
    float cpu_usage;
    long memory;
} ProcessInfo;

void get_process_user(int pid, char *user) {
    char path[40], line[100];
    FILE *status_file;
    uid_t uid;
    struct passwd *pw;

    // Construct the path to the /proc/[pid]/status file
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    // Open the status file
    status_file = fopen(path, "r");
    if (status_file == NULL) {
        strcpy(user, "unknown");
        return;
    }

    // Read the status file line by line to find the UID
    while (fgets(line, sizeof(line), status_file)) {
        if (strncmp(line, "Uid:", 4) == 0) {
            // Extract the real UID (the first number in the Uid line)
            sscanf(line, "Uid: %d", &uid);
            break;
        }
    }

    fclose(status_file);

    pw = getpwuid(uid);
    if (pw == NULL) {
        strcpy(user, "unknown");  // If username lookup fails, set to "unknown"
    } else {
        strncpy(user, pw->pw_name, 63);  // Copy the username into the user field
        user[63] = '\0';  // Ensure null-termination
    }
}
// Function to calculate CPU usage of the process
float get_cpu_usage(int pid, HashTable *table, uint64_t total_cpu_time_diff) {
    uint64_t new_proc_cpu_time;
    if (read_process_cpu_time(pid, &new_proc_cpu_time) == -1) {
        return 0.0;
    }
    uint64_t old_proc_cpu_time = hashtable_search(table, pid);
    hashtable_insert(table, pid, new_proc_cpu_time);
    if (old_proc_cpu_time == 0) {
        return 0.0;
    }
    if (total_cpu_time_diff == 0) {
        return 0.0;
    }
    float usage = 100 * (float)(new_proc_cpu_time - old_proc_cpu_time) / total_cpu_time_diff;
    return usage;
}

// Function to calculate memory usage of the process
long get_memory_usage(int pid) {
    char path[64], buffer[256];
    snprintf(path, 64, "/proc/%d/status", pid);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return 0;
    }

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
int read_process_info(ProcessInfo *processes, HashTable *table, uint64_t total_cpu_time_diff, int num_cores) {
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
            if (pid != 1 && pid < 1000) continue;
            snprintf(processes[num_processes].name, sizeof(processes[num_processes].name), "/proc/%d/stat", pid);

            // Open the stat file to get the process name
            FILE *fp = fopen(processes[num_processes].name, "r");
            if (fp == NULL) continue;

            fscanf(fp, "%d %s", &processes[num_processes].pid, processes[num_processes].name);
            fclose(fp);

            processes[num_processes].cpu_usage = get_cpu_usage(pid, table, total_cpu_time_diff) * num_cores;
            processes[num_processes].memory = get_memory_usage(pid);

            get_process_user(pid, processes[num_processes].user);
            

            num_processes++;
            if (num_processes >= MAX_PROCESSES) break;  // Limit max processes to prevent overflow
        }
    }

    closedir(dir);
    return num_processes;
}

// Function to calculate the number of tasks (processes)
int get_task_count() {
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir() error");
        return -1;
    }

    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            count++;
        }
    }
    closedir(dir);
    return count;
}

// Function to get load averages
void get_load_average(float *load1, float *load5, float *load15) {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp == NULL) {
        perror("fopen() error");
        return;
    }

    fscanf(fp, "%f %f %f", load1, load5, load15);
    fclose(fp);
}

// Function to get system uptime
void get_uptime(double *uptime) {
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp == NULL) {
        perror("fopen() error");
        return;
    }

    fscanf(fp, "%lf", uptime);
    fclose(fp);
}
