// cpuusage.h

#ifndef CPUUSAGE_H
#define CPUUSAGE_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define MEMCONVTYPE 'K', 'M', 'G' 

enum MEMTYPES {
    TOTALMEM, /** Total physical memory (in kB) */
    USEDMEM,      /** Used physical memory (in kB) */
    FREEMEM,       /** Free physical memory (in kB) */
    CACHEDMEM,     /** Cached memory (in kB) */
    BUFFERSMEM,    /** Buffers memory (in kB) */
    SHAREDMEM,    /** Shared memory (in kB) */
    AVAILMEM,  /** Available memory (in kB) */
    TOTALSWAP,      /** Total swap space (in kB) */
    USEDSWAP,       /** Used swap space (in kB) */
    FREESWAP,       /** Free swap space (in kB) */
};

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
    long long MEMTYPE; /** Needed for convert_mem function */
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

typedef struct {
    uint64_t utime; /**< User mode CPU time */
    uint64_t stime; /**< Kernel mode CPU time */
} process_cpu_time_t;

/**
 * @brief Reads CPU times for a given process from /proc/[pid]/stat.
 * 
 * @param pid The process ID.
 * @param time Pointer to store the time.
 * @return int 0 on success, -1 on failure.
 */
int read_process_cpu_time(pid_t pid, uint64_t *time);

/**
 * @brief Reads total CPU times from /proc/stat for all CPUs.
 * 
 * @param total_cpu_time Pointer to store the total CPU time.
 * @return int 0 on success, -1 on failure.
 */
int read_total_cpu_time(uint64_t *total_cpu_time);

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
long long convert_mem(MemInfo *mem_info, char conv_type, enum MEMTYPES type);

#endif
