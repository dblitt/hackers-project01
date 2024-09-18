CC = gcc
LD = gcc
CFLAGS = -g -Wall -std=gnu99
LDFLAGS = -lncurses

TARGETS = sysmonitor cpuusage_main 

all: $(TARGETS)

sysmonitor: sysmonitor.o cpuusage.o proc.o
	$(LD) $(LDFLAGS) -o $@ $^

cpuusage_main: cpuusage_main.o cpuusage.o
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	@echo Cleaning...
	rm $(TARGETS) *.o
