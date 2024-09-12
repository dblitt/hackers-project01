#include <ncurses.h>
#include <unistd.h>  // For sleep()
#include <stdlib.h>  // For random data generation (for demo purposes)

#define CPU_CORES 4
#define MEM_TOTAL 8000

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

int main() {
    initscr();              // Initialize ncurses
    start_color();          // Enable color
    curs_set(0);            // Hide the cursor
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Color for CPU bars: init_pair(pair_number, foreground_color, background_color)
    init_pair(2, COLOR_BLUE, COLOR_BLACK);  // Color for memory bars
    int height, width;
    getmaxyx(stdscr, height, width);

    // Simulated data (replace this with actual system data)
    float cpu_usages[CPU_CORES];
    float mem_usage;

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

    endwin();  // Restore terminal settings
    return 0;
}

