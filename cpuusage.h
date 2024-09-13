// cpuusage.h

#ifndef CPUUSAGE_H
#define CPUUSAGE_H

#include <string.h>

/**
 * @brief Reads CPU statistics from /proc/stat and calculates the CPU
 * load by comparing idle and total times between two intervals.
 *
 * @return CPU load as a float between 0.0 (0%) and 1.0 (100%).
 * Returns -1.0 on error (e.g., unable to read /proc/stat).
 *
 * @param ms_interval amount of time to wait between readings in ms
 */
float get_cpu_load(int ms_interval);

/**
 * @brief Struct to hold memory and swap usage information.
 */
typedef struct {
    long long total_mem;       /** Total physical memory (in kB) */
    long long used_mem;        /** Used physical memory (in kB) */
    long long free_mem;        /** Free physical memory (in kB) */
    long long cached_mem;      /** Cached memory (in kB) */
    long long buffers_mem;     /** Buffers memory (in kB) */
    long long shared_mem;      /** Shared memory (in kB) */
    long long available_mem;   /** Available memory (in kB) */
    long long total_swap;      /** Total swap space (in kB) */
    long long used_swap;       /** Used swap space (in kB) */
    long long free_swap;       /** Free swap space (in kB) */
    float mem_in_use_percent;  /** Memory usage percentage */
    float swap_in_use_percent; /** Swap usage percentage */
} MemInfo;

/**
 * @brief Retrieves memory and swap usage information.
 * 
 * @param mem_info Pointer to MemInfo struct to store the results.
 * @return 0 on success, -1 on failure.
 */
int get_mem_info(MemInfo *mem_info);

#endif
