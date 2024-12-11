#include <ncurses.h>
#include <unistd.h> 
#include <stdlib.h>  
#include <signal.h>
#include "cpuusage.h"
#include "proc.h"
#include "hashtable.h"

// #define term_height 20  // Number of rows for displaying processes
#define MAX_PROCESSES 1024
int pid_col_width, user_col_width, name_col_width, cpu_col_width, mem_col_width;
int total_width;

void draw_memory_bar(WINDOW *win, int y, int x, float mem_usage, const char *label, int bar_width);
void draw_cpu_bars(WINDOW *win, int y, int x, float *cpu_usages, int num_cores, int bar_width);
void display_process_info(WINDOW *win, ProcessInfo *processes, int num_processes, int start, int selected, int term_height, int term_width);
void adjust_layout(int term_width);


// Signal handler function
void handle_sigint(int sig) {
    endwin();  // End ncurses mode
    exit(0);
}

int main() {
    initscr();
    cbreak();
    keypad(stdscr, true);
    noecho();
    nodelay(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);  // Enable mouse events
    signal(SIGINT, handle_sigint);

    int term_height, term_width;
    getmaxyx(stdscr, term_height, term_width);
    int left_window_width = term_width / 2;
    int right_window_width = term_width - left_window_width;
    int ch;
    start_color();          // Enable color
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Color for CPU bars: init_pair(pair_number, foreground_color, background_color)
    init_pair(2, COLOR_BLUE, COLOR_BLACK);  // Color for memory bars
    init_pair(3, COLOR_RED, COLOR_BLACK); // Color for swap bars

    float mem_usage, swap_usage;


    ProcessInfo processes[MAX_PROCESSES];
    int start = 0;  // Variable for scrolling through the list
    int selected = 0;
    int num_processes = 0;  // Total number of processes

    HashTable *process_time_table = hashtable_create();
    uint64_t last_total_cpu_time;
    read_total_cpu_time(&last_total_cpu_time);

    WINDOW *left_win = newwin(term_height, left_window_width, 0, 0);          // Left window for process info
    WINDOW *right_win = newwin(term_height, right_window_width, 0, left_window_width);

    while (1) {
        bool quit = false;

        while ((ch = getch()) != ERR) {
            if (ch == 'q') quit = true;  // Exit loop on 'q'

            // Handle mouse input
            // MEVENT event;
            // if (ch == KEY_MOUSE) {
            //     if (getmouse(&event) == OK) {
            //         if (event.bstate & BUTTON4_PRESSED) {  // Scroll up
            //             if (start > 0) {
            //                 start--;
            //             }
            //         } else if (event.bstate & BUTTON5_PRESSED) {  // Scroll down
            //             if (start < num_processes - term_height) {
            //                 start++;
            //             }
            //         }
            //     }
            // }

            // Move selection up or down
            if ((ch == KEY_DOWN || ch == 'j') && selected < num_processes - 1) {
                selected++;
                // Only scroll down when the selected process reaches the last visible row
                if (selected - start >= term_height - 1) {
                    start++;
                }
            } else if ((ch == KEY_UP || ch == 'k') && selected > 0) {
                selected--;
                // Scroll up only if the selected process goes above the visible window
                if (selected < start) {
                    start--;
                }
            }
        }
        if (quit) break;

        getmaxyx(stdscr, term_height, term_width);
        int bar_width = term_width/2 - 20; // Leave space for labels and percentages
        if (bar_width < 10) bar_width = 10; // Set a minimum width for bars
        left_window_width = term_width / 2;
        right_window_width = term_width - left_window_width;

        wresize(left_win, term_height, left_window_width);
        wresize(right_win, term_height, right_window_width);
        mvwin(right_win, 0, left_window_width);

        wclear(left_win);
        wclear(right_win);
        
        display_process_info(right_win, processes, num_processes, start, selected, term_height, term_width);
        // Get CPU load information
        cpu_load_info_t *info = get_cpu_load_info();
        if (info == NULL) {
            mvprintw(0, 0, "Failed to retrieve CPU load information.");
            wrefresh(left_win);
            wrefresh(right_win);
            sleep(1);
            continue;
        }

        uint64_t new_total_cpu_time;
        read_total_cpu_time(&new_total_cpu_time);
        num_processes = read_process_info(processes, process_time_table, new_total_cpu_time - last_total_cpu_time, info->num_cores);
        last_total_cpu_time = new_total_cpu_time;
        for (int i = 0; i < num_processes - 1; i++) {
            for (int j = 0; j < num_processes - i - 1; j++) {
                if (processes[j].cpu_usage < processes[j + 1].cpu_usage) {
                    ProcessInfo temp = processes[j];
                    processes[j] = processes[j + 1];
                    processes[j + 1] = temp;
                }
            }
        }

        // uncomment to artificially limit the number of CPUs shown
        // info->num_cores = 8;

        // Get memory information
        MemInfo mem_info;
        get_mem_info(&mem_info);
        mem_usage = (float)mem_info.used_mem / (float)mem_info.total_mem;
        swap_usage = (float)mem_info.used_swap / (float)mem_info.total_swap;

        wclear(left_win);  // Clear the screen before updating

        // Additional system information
        float load1, load5, load15;
        get_load_average(&load1, &load5, &load15);
        double uptime;
        get_uptime(&uptime);
        int tasks = get_task_count();
        
        wclear(left_win);

        // Display task summary and load averages
        mvwprintw(left_win,0, 0, "Tasks: %d total", tasks);
        mvwprintw(left_win,1, 0, "Load average: %.2f, %.2f, %.2f", load1, load5, load15);
        mvwprintw(left_win,2, 0, "Uptime: %.0f seconds", uptime);

        // Draw CPU usage bars
        draw_cpu_bars(left_win, 4, 2, info->loads, info->num_cores, bar_width);

        // Draw memory usage bar
        draw_memory_bar(left_win, info->num_cores + 5, 2, mem_usage, "Mem", bar_width);

        // Draw swap usage bar
        draw_memory_bar(left_win, info->num_cores + 6, 2, swap_usage, "Swap", bar_width);

        // Display detailed memory and swap info
        mvwprintw(left_win, info->num_cores + 10 - 2, 2, "Total Memory: %.1f GB", mem_info.total_mem / 1024.0 / 1024.0);
        mvwprintw(left_win, info->num_cores + 11 - 2, 2, "Used Memory: %lld MB", mem_info.used_mem / 1024);
        mvwprintw(left_win, info->num_cores + 12 - 2, 2, "Free Memory: %lld MB", mem_info.free_mem / 1024);
        mvwprintw(left_win, info->num_cores + 13 - 2, 2, "Cached Memory: %lld MB", mem_info.cached_mem / 1024);
        mvwprintw(left_win, info->num_cores + 14 - 2, 2, "Total Swap: %lld MB", mem_info.total_swap / 1024);
        mvwprintw(left_win, info->num_cores + 15 - 2, 2, "Used Swap: %lld MB", mem_info.used_swap / 1024);

        wrefresh(left_win);  // Update the screen
        wrefresh(right_win); // Update the screen

        free_cpu_load_info(info);

        sleep(1);  // Update every second
    }
    delwin(left_win);
    delwin(right_win);
    endwin();

    return 0;
}


// Function to draw CPU usage bars
void draw_cpu_bars(WINDOW *win, int y, int x, float *cpu_usages, int num_cores, int bar_width) {
    for (int i = 0; i < num_cores; i++) {
        mvwprintw(win, y + i, x, "CPU %2d: |", i);
        int usage_bar = (int)(cpu_usages[i] * bar_width); // Scale usage to fit bar
        wattron(win, COLOR_PAIR(1));
        for (int j = 0; j < usage_bar; j++) {
            wprintw(win, "|");
        }
        wattroff(win, COLOR_PAIR(1));
        for (int j = 0; j < bar_width - usage_bar; j++) {
            wprintw(win, " ");
        }
        wprintw(win, "|");
        wprintw(win, " %.1f%%", cpu_usages[i] * 100);
    }
    wrefresh(win);
}

// Function to draw memory or swap usage bars
void draw_memory_bar(WINDOW *win, int y, int x, float usage, const char *label, int bar_width) {
    mvwprintw(win, y, x, "%6s: |", label);
    int usage_bar = (int)(usage * bar_width);  // Scale usage to fit bar
    if (strcmp(label, "Mem") == 0) {
        wattron(win, COLOR_PAIR(2));  // Memory bar in blue
    } else {
        wattron(win, COLOR_PAIR(3));  // Swap bar in red
    }
    for (int i = 0; i < usage_bar; i++) {
        wprintw(win, "|");
    }
    if (strcmp(label, "Mem") == 0) {
        wattroff(win, COLOR_PAIR(2));
    } else {
        wattroff(win, COLOR_PAIR(3));
    }
    for (int i = 0; i < bar_width - usage_bar; i++) {
        wprintw(win, " ");
    }
    wprintw(win, "|");
    wprintw(win, " %.1f%%", usage * 100);
    wrefresh(win);
}

void display_process_info(WINDOW *win, ProcessInfo *processes, int num_processes, int start, int selected, int term_height, int term_width) {
    // int row = 1;
    adjust_layout(term_width);
    wclear(win);
     // Header row
    mvwprintw(win, 0, 1, "%-*s %-*s %-*s %-*s %-*s",
              pid_col_width, "PID",
              user_col_width, "USER",
              name_col_width, "NAME",
              cpu_col_width, "CPU(%)",
              mem_col_width, "MEM(KB)");

    // Process rows
    int row = 2;
    for (int i = start; i < num_processes && row < term_height; i++) {
        if (processes[i].cpu_usage > 50.0) {
            if (i == selected) wattron(win, A_REVERSE);
            wattron(win, COLOR_PAIR(3));  // Highlight high CPU usage
        }

        mvwprintw(win, row, 1, "%-*d %-*s %-*s %-*.2f %-*ld",
                  pid_col_width, processes[i].pid,
                  user_col_width, processes[i].user,
                  name_col_width, processes[i].name,
                  cpu_col_width, processes[i].cpu_usage,
                  mem_col_width, processes[i].memory);

        wattroff(win, COLOR_PAIR(3));
        wattroff(win, A_REVERSE);
        row++;
    }

    wrefresh(win);
}

void adjust_layout(int term_width) {
    total_width = term_width/2 - 2; // Account for padding
    pid_col_width = total_width * 0.15;  // 15% for PID
    user_col_width = total_width * 0.20; // 20% for USER
    name_col_width = total_width * 0.30; // 30% for NAME
    cpu_col_width = total_width * 0.15;  // 15% for CPU
    mem_col_width = total_width * 0.20;  // 20% for MEMORY
}

