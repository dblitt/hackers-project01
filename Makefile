CC = gcc
LD = gcc
CFLAGS = -g -Wall -std=gnu99
LDFLAGS = -lncurses

TARGETS = sysmonitor cpuusage_main 

all: $(TARGETS)

sysmonitor: sysmonitor.o cpuusage.o proc.o hashtable.o
	$(LD) -o $@ $^ $(LDFLAGS)

cpuusage_main: cpuusage_main.o cpuusage.o
	$(LD) -o $@ $^ $(LDFLAGS)

clean:
	@echo Cleaning...
	rm $(TARGETS) *.o
