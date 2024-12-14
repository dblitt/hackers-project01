#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
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

void free_cpu_load_info(cpu_load_info_t *info) {
    if (info) {
        if (info->loads) {
            free(info->loads);
        }
        free(info);
    }
}

// cpu_load_info_t* get_cpu_load_info() {
//     FILE *file;
//     int num_cores = 0;
//     char buffer[256];

//     // Open /proc/stat to read CPU information
//     file = fopen("/proc/stat", "r");
//     if (file == NULL) {
//         perror("Failed to open /proc/stat");
//         return NULL;
//     }

//     fscanf(file, "%*[^\n]\n");
//     // First, count how many CPU cores we have by counting the "cpu" lines
//     while (fgets(buffer, sizeof(buffer), file)) {
//         if (strncmp(buffer, "cpu", 3) == 0 && buffer[3] != ' ') {
//             num_cores++;
//         } else {
//             continue;
//         }
//     }
    
//     // Reset the file pointer to the beginning of the file
//     rewind(file);

//     // Allocate the cpu_load_info_t structure
//     cpu_load_info_t *info = (cpu_load_info_t *)malloc(sizeof(cpu_load_info_t));
//     if (!info) {
//         perror("Failed to allocate cpu_load_info_t");
//         fclose(file);
//         return NULL;
//     }

//     // Allocate memory for the loads array
//     info->loads = (float *)malloc(num_cores * sizeof(float));
//     if (!info->loads) {
//         perror("Failed to allocate loads array");
//         free(info);
//         fclose(file);
//         return NULL;
//     }

//     // Set the number of cores in the structure
//     info->num_cores = num_cores;

//     // Variables to store CPU times
//     uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
//     uint64_t user2, nice2, system2, idle2, iowait2, irq2, softirq2, steal2;
//     uint64_t total1, total2, idle_time1, idle_time2;

//     fscanf(file, "%*[^\n]\n");

//     // Read and calculate CPU loads for each core
//     for (int i = 0; i < num_cores; i++) {
//         // First reading from /proc/stat
//         int temp = fscanf(file, "cpu%*d %lu %lu %lu %lu %lu %lu %lu %lu %*d %*d\n", 
//                    &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
//         if (temp < 8) {
//             perror("Failed to read CPU data");
//             free_cpu_load_info(info);
//             fclose(file);
//             return NULL;
//         }

//         // Calculate the total time and idle time from the first read
//         total1 = user + nice + system + idle + iowait + irq + softirq + steal;
//         idle_time1 = idle + iowait;

//         // Sleep for a short time to allow for the next CPU measurement (100 ms)
//         usleep(100000); // 100 milliseconds

//         // Reset the file pointer to the beginning of the file for the second read
//         rewind(file);

//         // Second reading from /proc/stat
//         for (int j = 0; j <= i; j++) {
//             // Skip the first (j-1) CPUs
//             if (fscanf(file, "cpu%*d %lu %lu %lu %lu %lu %lu %lu %lu", 
//                        &user2, &nice2, &system2, &idle2, &iowait2, &irq2, &softirq2, &steal2) != 8) {
//                 perror("Failed to read CPU data on second pass");
//                 free_cpu_load_info(info);
//                 fclose(file);
//                 return NULL;
//             }
//         }

//         // Calculate the total time and idle time from the second read
//         total2 = user2 + nice2 + system2 + idle2 + iowait2 + irq2 + softirq2 + steal2;
//         idle_time2 = idle2 + iowait2;

//         // Calculate the differences in total time and idle time
//         uint64_t total_diff = total2 - total1;
//         uint64_t idle_diff = idle_time2 - idle_time1;

//         // Avoid division by zero
//         if (total_diff == 0) {
//             info->loads[i] = 0.0f;
//         } else {
//             // Calculate CPU load for this core (1 - fraction of time spent idle)
//             info->loads[i] = (float)(total_diff - idle_diff) / (float)total_diff;
//         }
//     }

//     fclose(file);
//     return info;
// }

/**
 * @brief Reads CPU stats from /proc/stat for all cores.
 * 
 * @param stats Array to store CPU times for each core.
 * @param num_cores Number of CPU cores to read.
 * @return int 0 on success, -1 on failure.
 */
int read_cpu_stats(uint64_t (*stats)[8], int num_cores) {
    FILE *file = fopen("/proc/stat", "r");
    if (!file) {
        perror("Failed to open /proc/stat");
        return -1;
    }

    fscanf(file, "%*[^\n]\n");

    char buffer[256];
    for (int i = 0; i < num_cores; i++) {
        if (fgets(buffer, sizeof(buffer), file) == NULL || 
            sscanf(buffer, "cpu%*d %lu %lu %lu %lu %lu %lu %lu %lu",
                   &stats[i][0], &stats[i][1], &stats[i][2], &stats[i][3],
                   &stats[i][4], &stats[i][5], &stats[i][6], &stats[i][7]) != 8) {
            perror("Failed to read CPU data");
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}

/**
 * @brief Retrieves CPU load information for all cores.
 * 
 * Allocates and returns a structure containing the number of cores and an array
 * of CPU load percentages. The user must call free_cpu_load_info() to release memory.
 * 
 * @return cpu_load_info_t* A pointer to the cpu_load_info_t structure, or NULL on error.
 */
cpu_load_info_t* get_cpu_load_info() {
    int num_cores = 0;
    char buffer[256];
    
    // Open /proc/stat to count the number of CPU cores
    FILE *file = fopen("/proc/stat", "r");
    if (!file) {
        perror("Failed to open /proc/stat");
        return NULL;
    }

    fscanf(file, "%*[^\n]\n");

    // Count CPU cores by checking lines starting with "cpu" followed by a number
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "cpu", 3) == 0 && buffer[3] != ' ') {
            num_cores++;
        } else {
            break;
        }
    }

    fclose(file);

    if (num_cores == 0) {
        fprintf(stderr, "No CPU cores found\n");
        return NULL;
    }

    // Allocate the cpu_load_info_t structure
    cpu_load_info_t *info = (cpu_load_info_t *)malloc(sizeof(cpu_load_info_t));
    if (!info) {
        perror("Failed to allocate cpu_load_info_t");
        return NULL;
    }

    // Allocate memory for the loads array
    info->loads = (float *)malloc(num_cores * sizeof(float));
    if (!info->loads) {
        perror("Failed to allocate loads array");
        free(info);
        return NULL;
    }

    // Set the number of cores in the structure
    info->num_cores = num_cores;

    // Allocate arrays to store CPU times (user, nice, system, idle, etc.)
    uint64_t stats1[num_cores][8], stats2[num_cores][8];

    // Read first snapshot of CPU stats
    if (read_cpu_stats(stats1, num_cores) == -1) {
        free_cpu_load_info(info);
        return NULL;
    }

    // Sleep for a short time (100ms) to take a second measurement
    usleep(4*250000);

    // Read second snapshot of CPU stats
    if (read_cpu_stats(stats2, num_cores) == -1) {
        free_cpu_load_info(info);
        return NULL;
    }

    // Calculate CPU loads for each core
    for (int i = 0; i < num_cores; i++) {
        uint64_t total1 = 0, total2 = 0, idle_time1 = 0, idle_time2 = 0;

        // Calculate total times and idle times from the snapshots
        for (int j = 0; j < 8; j++) {
            total1 += stats1[i][j];
            total2 += stats2[i][j];
        }

        idle_time1 = stats1[i][3] + stats1[i][4]; // idle + iowait
        idle_time2 = stats2[i][3] + stats2[i][4];

        uint64_t total_diff = total2 - total1;
        uint64_t idle_diff = idle_time2 - idle_time1;

        if (total_diff == 0) {
            info->loads[i] = 0.0f;
        } else {
            info->loads[i] = (float)(total_diff - idle_diff) / (float)total_diff;
        }
    }

    return info;
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

// /**
//  * @brief Reads CPU times for a given process from /proc/[pid]/stat.
//  * 
//  * @param pid The process ID.
//  * @param time Pointer to store the time.
//  * @return int 0 on success, -1 on failure.
//  */
// int read_process_cpu_time(pid_t pid, uint64_t *time) {
//     char filepath[256];
//     snprintf(filepath, sizeof(filepath), "/proc/%d/stat", pid);

//     FILE *file = fopen(filepath, "r");
//     if (!file) {
//         perror("Failed to open /proc/[pid]/stat");
//         return -1;
//     }

//     // Reading fields from /proc/[pid]/stat; we are interested in fields 14 (utime) and 15 (stime)
//     unsigned long utime, stime;
//     int val = fscanf(file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &utime, &stime);
//     if (val != 2) {
//         perror("Failed to read process CPU times");
//         fclose(file);
//         return -1;
//     }

//     fclose(file);

//     // Store the CPU times in the structure
//     *time = (uint64_t)utime;
//     *time += (uint64_t)stime;

//     return 0;
// }

/**
 * @brief Reads CPU time for a given process from /proc/[pid]/stat.
 * 
 * @param pid The process ID.
 * @param time Pointer to store the time.
 * @return int 0 on success, -1 on failure.
 */
int read_process_cpu_time(pid_t pid, uint64_t *time) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "/proc/%d/stat", pid);

    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Failed to open /proc/[pid]/stat");
        return -1;
    }

    // Read the whole line from the file
    char line[1024];
    if (!fgets(line, sizeof(line), file)) {
        perror("Failed to read process stats");
        fclose(file);
        return -1;
    }

    fclose(file);

    // Parse the line to extract utime and stime
    char *pointer = line;
    char *lastparen = line;
    while (*pointer) {
        if (*pointer == ')') {
            lastparen = pointer;
        }
        pointer++;
    }
    lastparen += 2;
    unsigned long utime, stime;
    int fields_read = sscanf(lastparen,
                             "%*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
                             &utime, &stime);

    if (fields_read != 2) {
        perror("Failed to parse process CPU times");
        return -1;
    }

    // Store the CPU times in the structure
    *time = (uint64_t)utime + (uint64_t)stime;

    return 0;
}

/**
 * @brief Reads total CPU times from /proc/stat for all CPUs.
 * 
 * @param total_cpu_time Pointer to store the total CPU time.
 * @return int 0 on success, -1 on failure.
 */
int read_total_cpu_time(uint64_t *total_cpu_time) {
    FILE *file = fopen("/proc/stat", "r");
    if (!file) {
        perror("Failed to open /proc/stat");
        return -1;
    }

    char buffer[256];
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal;

    // Read the first line, which gives the total CPU time across all cores
    if (fgets(buffer, sizeof(buffer), file) == NULL ||
        sscanf(buffer, "cpu %lu %lu %lu %lu %lu %lu %lu %lu", 
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) != 8) {
        perror("Failed to read system CPU times");
        fclose(file);
        return -1;
    }

    fclose(file);

    // Total CPU time is the sum of all fields
    *total_cpu_time = user + nice + system + idle + iowait + irq + softirq + steal;

    return 0;
}

/**
 * @brief Converts memory of one type to desired type.
 * Use the function like this: convert_mem([MemInfo pointer], convtype, and type being converted).
 * 
 * @param mem_info Pointer to MemInfo to source data from.
 * @param conv_type MEMCONVTYPE used for determining what to convert to. 'K' = kilobytes, 'M' = megabytes and 'G' = gigabytes.
 * @param type The type of data being converted (e.g usable memory). A full list of types can be seen in cpuusage.h.
 * 
 * @return Returns converted value if it properly converts, -1 if it fails. 
 */
long long convert_mem(MemInfo *mem_info, char conv_type, enum MEMTYPES type) {
    long long MEMTYPE = get_mem_type(&mem_info, type);
    if (!mem_info) { return -1; }
    switch (conv_type) {
        case 'K': {
            return mem_info; /* By default the data is already in kilobytes 
            but implementing in case of possible use in the future */
            break;
        }
        case 'M': {
            return mem_info->MEMTYPE / 1024;
            break;
        }
        case 'G': {
            return mem_info->MEMTYPE / 1024 / 1024;
        }
        default: {
            return -1;
        }
    }
}

/**
 * @brief Takes enum type and converts it to a memory pointer. 
 * Required for convert_mum function to work properly, as without it
 * there would be no way of automatically identifying what type of data to convert.
 *  
 *@param mem_info Pointer to MemInfo
 *@param MEMTYPES Type of memory being used when converting (e.g. used memory, total memory)
 *
 *@return Returns memory type if properly recieved, -1 if it fails.
 */
long long get_mem_type(MemInfo *mem_info, enum MEMTYPES type) {
    if (!mem_info) { return -1; }
    switch (type) {
        case TOTALMEM: return mem_info->total_mem; break; 
        case USEDMEM: return mem_info->used_mem; break;
        case FREEMEM: return mem_info->free_mem; break;
        case CACHEDMEM: return mem_info->cached_mem; break;
        case BUFFERSMEM: return mem_info->buffers_mem; break;
        case SHAREDMEM: return mem_info->shared_mem; break;
        case AVAILMEM: return mem_info->available_mem; break;
        case TOTALSWAP: return mem_info->total_swap; break;
        case USEDSWAP: return mem_info->used_swap; break;
        case FREESWAP: return mem_info->free_swap; break;
        default: return -1;
    }
}
