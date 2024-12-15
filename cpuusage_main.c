#include <stdio.h>
#include "cpuusage.h"

int main() {
    float cpu_load = get_cpu_load(100);

    if (cpu_load >= 0.0f) {
        printf("CPU Load: %.2f%%\n", cpu_load * 100.0f);
    } else {
        printf("Failed to calculate CPU load.\n");
    }

    MemInfo mem_info;

    get_mem_info(&mem_info);
    printf("Total Memory: %lld MB\n", convert_mem(mem_info.total_mem, 'M', TOTALMEM));
    printf("Total Memory: %.1f GB\n", convert_mem(mem_info.total_mem, 'G', TOTALMEM));
    printf("Used Memory: %lld MB\n", convert_mem(mem_info.used_mem, 'M', USEDMEM));
    printf("Free Memory: %lld MB\n", convert_mem(mem_info.free_mem, 'M', FREEMEM));
    printf("Cached Memory: %lld MB\n", convert_mem(mem_info.cached_mem, 'M', CACHEDMEM));
    printf("Buffers: %lld MB\n", convert_mem(mem_info.buffers_mem, 'M', BUFFERSMEM));
    printf("Total Swap: %lld MB\n", convert_mem(mem_info.total_swap, 'M', TOTALSWAP));
    printf("Used Swap: %lld MB\n", convert_mem(mem_info.used_swap, 'M', USEDSWAP));
    printf("Free Swap: %lld MB\n", convert_mem(mem_info.free_swap, 'M', FREESWAP));
    printf("Memory In Use: %.2f%%\n", mem_info.mem_in_use_percent);
    printf("Swap In Use: %.2f%%\n", mem_info.swap_in_use_percent);
    printf("htop %lld MB\n", (mem_info.used_mem - mem_info.buffers_mem - mem_info.cached_mem) / 1024);
    printf("available mem %lld MB\n", convert_mem(mem_info.available_mem, 'M', AVAILMEM));
    printf("total - available mem %lld MB\n", (mem_info.total_mem - mem_info.available_mem) / 1024);

    printf("getting load for all cpu cores\n");
    cpu_load_info_t *info = get_cpu_load_info();

    if (info) {
        printf("Number of CPU cores: %d\n", info->num_cores);
        for (int i = 0; i < info->num_cores; i++) {
            printf("CPU %d Load: %.2f%%\n", i, info->loads[i] * 100.0f);
        }

        free_cpu_load_info(info);
    } else {
        printf("Failed to retrieve CPU load information.\n");
    }

    return 0;
}
