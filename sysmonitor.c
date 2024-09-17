#include <ncurses.h>
#include <unistd.h> 
#include <stdlib.h>  
#include "cpuusage.h"

#define CPU_CORES 32
#define MEM_TOTAL 8000

void draw_memory_bar(int y, int x, float mem_usage, const char *label);
void draw_cpu_bars(int y, int x, float cpu_usages[CPU_CORES]);

int main() {
    initscr();
    raw();
    keypad(stdscr, true);
    noecho();
    nodelay(stdscr, TRUE);
    int height, width;
    getmaxyx(stdscr, height, width);
    int ch;

    start_color();          // Enable color
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Color for CPU bars: init_pair(pair_number, foreground_color, background_color)
    init_pair(2, COLOR_BLUE, COLOR_BLACK);  // Color for memory bars
    init_pair(3, COLOR_RED, COLOR_BLACK); // Color for swap bars

    // Simulated data (replace this with actual system data)
    float cpu_usages[CPU_CORES];
    float mem_usage, swap_usage;

    //int top_height = height / 4;
    //int middle_height = height / 2;
    //int bottom_height = height - (top_height + middle_height);

    //WINDOW *top_win = newwin(top_height, width, 0, 0);                 // Top window
    //WINDOW *middle_win = newwin(middle_height, width, top_height, 0);  // Middle window
    //WINDOW *bottom_win = newwin(bottom_height, width, top_height + middle_height, 0);  // Bottom window
    
    //box(top_win, 0, 0);
    //box(middle_win, 0, 0);
    //box(bottom_win, 0, 0);

    while (1) {
        ch = getch();
        if (ch == 'q') break;  // Exit loop on 'q'
        // Simulate random CPU and memory usage for demo purposes
        for (int i = 0; i < CPU_CORES; i++) {
            cpu_usages[i] = get_cpu_load(i);
        }
        
        // Get memory information
        MemInfo mem_info;
        get_mem_info(&mem_info);
        mem_usage = (float)mem_info.used_mem / (float)mem_info.total_mem;
        swap_usage = (float)mem_info.used_swap / (float)mem_info.total_swap;

        clear();  // Clear the screen before updating

        // Display task summary and load averages (mocked for now)
        mvprintw(0, 0, "Tasks: %d total, %d running", 145, 3);  // Replace with actual task data if available
        mvprintw(1, 0, "Load average: 0.25, 0.20, 0.10");       // Replace with actual load averages
        mvprintw(2, 0, "Uptime: 12:34:56");                     // Replace with actual system uptime if available

        // Draw CPU usage bars
        draw_cpu_bars(4, 2, cpu_usages);

        // Draw memory usage bar
        draw_memory_bar(CPU_CORES + 6, 2, mem_usage, "Mem");

        // Draw swap usage bar
        draw_memory_bar(CPU_CORES + 8, 2, swap_usage, "Swap");

        // Display detailed memory and swap info
        mvprintw(CPU_CORES + 10, 2, "Total Memory: %.1f GB", mem_info.total_mem / 1024.0 / 1024.0);
        mvprintw(CPU_CORES + 11, 2, "Used Memory: %lld MB", mem_info.used_mem / 1024);
        mvprintw(CPU_CORES + 12, 2, "Free Memory: %lld MB", mem_info.free_mem / 1024);
        mvprintw(CPU_CORES + 13, 2, "Cached Memory: %lld MB", mem_info.cached_mem / 1024);
        mvprintw(CPU_CORES + 14, 2, "Total Swap: %lld MB", mem_info.total_swap / 1024);
        mvprintw(CPU_CORES + 15, 2, "Used Swap: %lld MB", mem_info.used_swap / 1024);

        refresh();  // Update the screen

        sleep(1);  // Update every second
    }

    //mvwprintw(top_win, 1, 1, "Top Window");
    //mvwprintw(middle_win, 1, 1, "Middle Window");
    //mvwprintw(bottom_win, 1, 1, "Bottom Window");

    //nodelay(stdscr, TRUE);  // Non-blocking input

    /*
    while(1) {
        int ch = getch();
        if (ch == 'q') break;  // Exit loop on 'q'

        // Refresh windows to display updated data
        wrefresh(top_win);
        wrefresh(middle_win);
        wrefresh(bottom_win);

        // Small delay to limit CPU usage
        napms(500);
    }


    wrefresh(top_win);
    wrefresh(middle_win);
    wrefresh(bottom_win);
    */

    /*
    printw("type a character: ");
    int ch = getch();
    printw("\n");

    
    if (ch == KEY_F(1)) {
        printw("F1 pressed");
    } else {
        printw("The key was: ");
        attron(A_BOLD);
        printw("%c", ch);
        attroff(A_BOLD);
    } */

    //refresh();
    //getch();
    endwin();

    return 0;
}



// Function to draw CPU usage bars
void draw_cpu_bars(int y, int x, float cpu_usages[CPU_CORES]) {
    for (int i = 0; i < CPU_CORES; i++) {
        mvprintw(y + i, x, "CPU %d: ", i);
        int usage_bar = (int)(cpu_usages[i] * 50); // Scale usage to fit bar
        attron(COLOR_PAIR(1));
        for (int j = 0; j < usage_bar; j++) {
            printw("|");
        }
        attroff(COLOR_PAIR(1));
        printw(" %.1f%%", cpu_usages[i] * 100);
    }
}

// Function to draw memory or swap usage bars
void draw_memory_bar(int y, int x, float usage, const char *label) {
    mvprintw(y, x, "%s: ", label);
    int usage_bar = (int)(usage * 50);  // Scale usage to fit bar
    if (strcmp(label, "Mem") == 0) {
        attron(COLOR_PAIR(2));  // Memory bar in blue
    } else {
        attron(COLOR_PAIR(3));  // Swap bar in red
    }
    for (int i = 0; i < usage_bar; i++) {
        printw("|");
    }
    if (strcmp(label, "Mem") == 0) {
        attroff(COLOR_PAIR(2));
    } else {
        attroff(COLOR_PAIR(3));
    }
    printw(" %.1f%%", usage * 100);
}


// Function to draw memory usage
/*
void draw_memory_bar(int y, int x, float mem_usage) {
    mvprintw(y, x, "Memory: ");
    int mem_bar = (int)(mem_usage * 50);
    attron(COLOR_PAIR(2));
    for (int i = 0; i < mem_bar; i++) {
        printw("|");
    }
    attroff(COLOR_PAIR(2));
    printw(" %.1f%% of %dMB", mem_usage * 100, MEM_TOTAL);
}
*/