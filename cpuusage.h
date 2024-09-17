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

typedef struct {
    int num_cores;   /**< Number of CPU cores */
    float *loads;    /**< Array of CPU load percentages (0.0 to 1.0) for each core */
} cpu_load_info_t;

/**
 * @brief Frees the memory associated with a cpu_load_info_t structure.
 * 
 * @param info Pointer to the cpu_load_info_t structure to free.
 */
void free_cpu_load_info(cpu_load_info_t *info);

/**
 * @brief Retrieves CPU load information for all cores.
 * 
 * Allocates and returns a structure containing the number of cores and an array
 * of CPU load percentages. The user must call free_cpu_load_info() to release memory.
 * 
 * @return cpu_load_info_t* A pointer to the cpu_load_info_t structure, or NULL on error.
 */
cpu_load_info_t* get_cpu_load_info();

#endif
