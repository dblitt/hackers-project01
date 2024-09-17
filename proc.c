#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

// Define a limit for how many processes you can display
#define MAX_PROCESSES 1024

// Structure to hold process information
typedef struct {
    char pid[10];
    char user[64];
    char comm[256];
    char state;
} ProcessInfo;

// Function to get process owner (username) by UID
char* get_username(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return pw->pw_name;
    }
    return "unknown";
}

// Function to get the list of processes
int get_process_info(ProcessInfo processes[], int max_processes) {
    DIR *dir;
    struct dirent *entry;
    FILE *fp;
    char path[256], line[1024], uid_str[10];
    int ppid, uid;
    int process_count = 0;

    // Open the /proc directory
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir(/proc)");
        return 0;
    }

    // Iterate through each entry in /proc directory
    while ((entry = readdir(dir)) != NULL && process_count < max_processes) {
        // Only process directories with numeric names (which are process IDs)
        if (isdigit(*entry->d_name)) {
            strcpy(processes[process_count].pid, entry->d_name);
            snprintf(path, sizeof(path), "/proc/%s/stat", processes[process_count].pid);

            // Open the /proc/[pid]/stat file
            fp = fopen(path, "r");
            if (fp == NULL) {
                continue;
            }

            // Read the relevant process information from the stat file
            fscanf(fp, "%d %s %c", &ppid, processes[process_count].comm, &processes[process_count].state);
            fclose(fp);

            // Get the UID and username from /proc/[pid]/status
            snprintf(path, sizeof(path), "/proc/%s/status", processes[process_count].pid);
            fp = fopen(path, "r");
            if (fp) {
                while (fgets(line, sizeof(line), fp)) {
                    if (strncmp(line, "Uid:", 4) == 0) {
                        sscanf(line, "Uid:\t%d", &uid);
                        break;
                    }
                }
                fclose(fp);
                strcpy(processes[process_count].user, get_username(uid));
            } else {
                strcpy(processes[process_count].user, "unknown");
            }

            process_count++;
        }
    }

    closedir(dir);
    return process_count;
}

int main() {
    ProcessInfo processes[MAX_PROCESSES];
    int num_processes, display_limit, start_index = 0;
    int ch;
    
    // Initialize ncurses
    initscr();
    noecho();        // Don't echo typed characters
    curs_set(FALSE); // Hide the cursor
    keypad(stdscr, TRUE); // Enable arrow keys

    // Main loop to capture input and display the process list
    while (1) {
        clear();    // Clear the screen

        // Fetch and store process information
        num_processes = get_process_info(processes, MAX_PROCESSES);
        
        // Calculate how many processes fit in the window (excluding header)
        display_limit = LINES - 3;

        // Display the table header
        printw("%-10s %-10s %-20s %-10s\n", "PID", "USER", "Process Name", "State");
        printw("------------------------------------------------------------\n");

        // Display the process list from start_index
        for (int i = start_index; i < num_processes && i < start_index + display_limit; i++) {
            printw("%-10s %-10s %-20s %-10c\n", 
                   processes[i].pid, 
                   processes[i].user, 
                   processes[i].comm, 
                   processes[i].state);
        }

        refresh();  // Refresh the screen to update the displayed info

        // Capture user input for scrolling
        ch = getch();
        switch (ch) {
            case 'q': // Quit the program
                endwin();
                exit(0);
                break;
            case KEY_UP: // Scroll up
                if (start_index > 0) {
                    start_index--;
                }
                break;
            case KEY_DOWN: // Scroll down
                if (start_index + display_limit < num_processes) {
                    start_index++;
                }
                break;
            default:
                break;
        }

        // Sleep for a brief time before refreshing again
        usleep(100000); // 0.1 second delay
    }

    // End ncurses mode
    endwin();
    return 0;
}

