#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

/**
 * @brief Reads CPU statistics from /proc/stat and calculates the CPU
 * load by comparing idle and total times between two intervals.
 *
 * @return CPU load as a float between 0.0 (0%) and 1.0 (100%).
 * Returns -1.0 on error (e.g., unable to read /proc/stat).
 *
 * @param ms_interval amount of time to wait between readings in ms
 */
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

int main() {
    float cpu_load = get_cpu_load(100);

    if (cpu_load >= 0.0f) {
        printf("CPU Load: %.2f%%\n", cpu_load * 100.0f);
    } else {
        printf("Failed to calculate CPU load.\n");
    }

    return 0;
}
