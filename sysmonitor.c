#include <ncurses.h>
#include <unistd.h> 
#include <stdlib.h>  
//#include "cpuusage.c"

#define CPU_CORES 4
#define MEM_TOTAL 8000

void draw_memory_bar(int y, int x, float mem_usage);
void draw_cpu_bars(int y, int x, float cpu_usages[CPU_CORES]);

int main() {
    initscr();
    raw();
    keypad(stdscr, true);
    noecho();
    int height, width;
    getmaxyx(stdscr, height, width);

    start_color();          // Enable color
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Color for CPU bars: init_pair(pair_number, foreground_color, background_color)
    init_pair(2, COLOR_BLUE, COLOR_BLACK);  // Color for memory bars

    // Simulated data (replace this with actual system data)
    float cpu_usages[CPU_CORES];
    float mem_usage;

    int top_height = height / 4;
    int middle_height = height / 2;
    int bottom_height = height - (top_height + middle_height);

    WINDOW *top_win = newwin(top_height, width, 0, 0);                 // Top window
    WINDOW *middle_win = newwin(middle_height, width, top_height, 0);  // Middle window
    WINDOW *bottom_win = newwin(bottom_height, width, top_height + middle_height, 0);  // Bottom window
    
    box(top_win, 0, 0);
    box(middle_win, 0, 0);
    box(bottom_win, 0, 0);

    while (1) {
        // Simulate random CPU and memory usage for demo purposes
        for (int i = 0; i < CPU_CORES; i++) {
            cpu_usages[i] = (float)(rand() % 100) / 100.0;
        }
        
        mem_usage = (float)(rand() % 100) / 100.0;

        clear();  // Clear the screen before updating

        // Draw CPU usage bars
        draw_cpu_bars(2, 2, cpu_usages);

        // Draw memory usage bar
        draw_memory_bar(CPU_CORES + 4, 2, mem_usage);

        // Simulated system info (replace with actual data)
        // mvprintw moves cursor to (y, x) position and prints formatted string there
        mvprintw(0, 0, "Tasks: %d total, %d running", 123, 3);
        mvprintw(1, 0, "Load average: 0.15, 0.10, 0.05");

        refresh();  // Update the screen

        sleep(1);  // Update every second
    }

    //mvwprintw(top_win, 1, 1, "Top Window");
    //mvwprintw(middle_win, 1, 1, "Middle Window");
    mvwprintw(bottom_win, 1, 1, "Bottom Window");

    nodelay(stdscr, TRUE);  // Non-blocking input

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
    getch();
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

// Function to draw memory usage
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