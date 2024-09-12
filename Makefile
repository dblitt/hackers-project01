CC = gcc
CFLAGS = -g -Wall -std=gnu99 -lncurses

TARGETS = main

all: $(TARGETS)

clean:
	@echo Cleaning...
	rm $(TARGETS)
