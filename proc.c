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

// Function to display process info with scrolling
void display_process_info(WINDOW *win, ProcessInfo *processes, int num_processes, int start) {
    int row = 1;

    wattron(win, A_BOLD | COLOR_PAIR(1));  // Bold and colored header
    mvwprintw(win, 0, 1, "PID      NAME             CPU(%%)   MEMORY(KB)");
    wattroff(win, A_BOLD | COLOR_PAIR(1));

    for (int i = start; i < num_processes && row < DISPLAY_ROWS; i++) {
        if (processes[i].cpu_usage > 50.0) {  // Highlight high CPU usage in red
            wattron(win, COLOR_PAIR(2));
        }
        mvwprintw(win, row, 1, "%-8d %-16s %-8.2f %-12ld", processes[i].pid, processes[i].name, processes[i].cpu_usage, processes[i].memory);
        wattroff(win, COLOR_PAIR(2));
        row++;
    }
    wrefresh(win);
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

// Initialize color pairs for ncurses
void init_colors() {
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);  // Cyan text for headers
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Red text for high CPU usage
    init_pair(3, COLOR_GREEN, COLOR_BLACK); // Green text for normal processes
}

int main() {
    // Initialize ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(FALSE);
    keypad(stdscr, TRUE);  // Enable keyboard input
    timeout(1000);  // Set a timeout for getch

    // Initialize color support
    if (has_colors()) {
        init_colors();
    }

    WINDOW *proc_win = newwin(DISPLAY_ROWS, 80, 0, 0);

    ProcessInfo processes[MAX_PROCESSES];
    int start = 0;  // Variable for scrolling through the list
    int num_processes = 0;  // Total number of processes

    while (1) {
        // Read process info
        num_processes = read_process_info(processes);

        // Sort processes by CPU usage (optional)
        for (int i = 0; i < num_processes - 1; i++) {
            for (int j = 0; j < num_processes - i - 1; j++) {
                if (processes[j].cpu_usage < processes[j + 1].cpu_usage) {
                    ProcessInfo temp = processes[j];
                    processes[j] = processes[j + 1];
                    processes[j + 1] = temp;
                }
            }
        }

        // Display process information in the window
        display_process_info(proc_win, processes, num_processes, start);

        // Scroll through processes with arrow keys or 'j' and 'k'
        int ch = getch();
        if (ch == 'q') break;  // Press 'q' to quit
        else if ((ch == KEY_DOWN || ch == 'j') && start + DISPLAY_ROWS < num_processes) start++;  // Scroll down
        else if ((ch == KEY_UP || ch == 'k') && start > 0) start--;  // Scroll up
    }

    // End ncurses mode
    endwin();
    return 0;
}
