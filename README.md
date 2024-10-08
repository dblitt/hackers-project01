# NCURSES System Monitor

## How to use

`sysmonitor` runs on Linux systems. If you have basic C build tools installed, you can run `make` to build the binary. Then, just run `./sysmonitor` to start the program. You can use the arrow keys to navigate the process list, and you can quit using `q` or `Ctrl+C`.

## Context

The “new curses” or ncurses was released in 1990s as improvement to 1980s “curses”. Curses was created by Ken Arnold in the 1980s at the University of California, Berkeley and helped develop text-based programs with features like windows, colors, keyboard inputs. The curses library was crucial in the early days of Unix systems when graphical user interfaces (GUIs) were not prevalent or feasible due to limited hardware capabilities. ncurses improved curses by adding better support for modern terminals, portability, and advanced features. One of the key features of ncurses is terminal independence, portability. The library has been very influential and has been used to develop tools such as vim and htop. The ncurses library is used to build TUIs, or text-based user interfaces, which predates GUIs (graphical user interfaces). Ncurses is crucial for creating terminal-based interfaces that offer more than just sequential command-line output. They are able to give the user a pseudo-graphical environment inside of the terminal by enabling full-screen interfaces that accept keyboard and mouse input. Lastly, they are lightweight and easy to run on the underpowered and GPU-less systems of earlier technology, as well as easy to run over remote connections like ssh. 

In our project, we made a replica of htop which is a system monitoring tool built in the ncurses library. The originally htop provides a visually rich, interactive, real-time display of system processes, CPU, memory usage, and other key metrics. Unlike the traditional top command, which is more basic, htop offers a more user-friendly experience by allowing users to scroll through processes and color-coded resource usage. 
