#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "cpuusage.h"

float get_cpu_load(int ms_interval) {
    FILE *file;
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
    uint64_t user2, nice2, system2, idle2, iowait2, irq2, softirq2, steal2;
    uint64_t total1, total2, idle_time1, idle_time2;

    // First reading from /proc/stat
    file = fopen("/proc/stat", "r");
    if (file == NULL)
    {
        perror("Failed to open /proc/stat");
        return -1.0f; // Return an error indicator if /proc/stat cannot be opened
    }
    
    // Read the first CPU line (aggregated values across all cores)
    if (fscanf(file, "cpu  %lu %lu %lu %lu %lu %lu %lu %lu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) != 8)
    {
        perror("Failed to read CPU data");
        fclose(file);
        return -1.0f; // Return an error if we cannot correctly read the values
    }
    fclose(file);

    // Calculate the total time and idle time from the first read
    total1 = user + nice + system + idle + iowait + irq + softirq + steal;
    idle_time1 = idle + iowait;

    // Sleep for a short time to allow for the next CPU measurement (250 ms)
    usleep(ms_interval * 1000); // 250 milliseconds

    // Second reading from /proc/stat
    file = fopen("/proc/stat", "r");
    if (file == NULL)
    {
        perror("Failed to open /proc/stat");
        return -1.0f;
    }

    // Read the first CPU line again (aggregated values across all cores)
    if (fscanf(file, "cpu  %lu %lu %lu %lu %lu %lu %lu %lu",
               &user2, &nice2, &system2, &idle2, &iowait2, &irq2, &softirq2, &steal2) != 8)
    {
        perror("Failed to read CPU data");
        fclose(file);
        return -1.0f;
    }
    fclose(file);

    // Calculate the total time and idle time from the second read
    total2 = user2 + nice2 + system2 + idle2 + iowait2 + irq2 + softirq2 + steal2;
    idle_time2 = idle2 + iowait2;

    // Calculate the differences in total time and idle time
    uint64_t total_diff = total2 - total1;
    uint64_t idle_diff = idle_time2 - idle_time1;

    // Avoid division by zero (this can happen if total_diff is zero)
    if (total_diff == 0) {
        return 0.0f; // CPU load is effectively 0% if no time passed between measurements
    }

    // Calculate CPU load (1 - the fraction of time spent idle)
    float cpu_load = (float)(total_diff - idle_diff) / (float)total_diff;

    return cpu_load; // This will return a value between 0.0 (0%) and 1.0 (100%)
}

int get_mem_info(MemInfo *mem_info) {
    FILE *file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        perror("Failed to open /proc/meminfo");
        return -1;
    }

    char line[256];
    int fields_found = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 9, "%lld", &mem_info->total_mem);
            fields_found++;
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 8, "%lld", &mem_info->free_mem);
            fields_found++;
        } else if (strncmp(line, "Shmem:", 6) == 0) {
            sscanf(line + 6, "%lld", &mem_info->shared_mem);
            fields_found++;
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line + 13, "%lld", &mem_info->available_mem);
            fields_found++;
        } else if (strncmp(line, "Buffers:", 8) == 0) {
            sscanf(line + 8, "%lld", &mem_info->buffers_mem);
            fields_found++;
        } else if (strncmp(line, "Cached:", 7) == 0) {
            sscanf(line + 7, "%lld", &mem_info->cached_mem);
            fields_found++;
        } else if (strncmp(line, "SwapTotal:", 10) == 0) {
            sscanf(line + 10, "%lld", &mem_info->total_swap);
            fields_found++;
        } else if (strncmp(line, "SwapFree:", 9) == 0) {
            sscanf(line + 9, "%lld", &mem_info->free_swap);
            fields_found++;
        }
    }

    fclose(file);

    if (fields_found < 7) {
        fprintf(stderr, "Error: Could not parse all required fields from /proc/meminfo\n");
        return -1;
    }

    // Calculate used memory
    mem_info->used_mem = mem_info->total_mem - mem_info->free_mem;// - mem_info->buffers_mem - mem_info->cached_mem;
    
    // Calculate used swap
    mem_info->used_swap = mem_info->total_swap - mem_info->free_swap;

    // Calculate percentages
    mem_info->mem_in_use_percent = 100.0 * (float)mem_info->used_mem / (float)mem_info->total_mem;
    if (mem_info->total_swap > 0) {
        mem_info->swap_in_use_percent = 100.0 * (float)mem_info->used_swap / (float)mem_info->total_swap;
    } else {
        mem_info->swap_in_use_percent = 0.0;
    }

    return 0;
}
