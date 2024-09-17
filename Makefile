CC = gcc
LD = gcc
CFLAGS = -g -Wall -std=gnu99
LDFLAGS = -lncurses

TARGETS = sysmonitor cpuusage_main htop-display

all: $(TARGETS)

sysmonitor: sysmonitor.o cpuusage.o
	$(LD) $(LDFLAGS) -o $@ $^

cpuusage_main: cpuusage_main.o cpuusage.o
	$(LD) $(LDFLAGS) -o $@ $^

htop-display: htop-display.o
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	@echo Cleaning...
	rm $(TARGETS) *.o
