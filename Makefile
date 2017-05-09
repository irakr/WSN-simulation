# Makefile for WSN simulator

# Directories
CORE_DIR=./core/
BIN_DIR=./
ALL_DIR=$(CORE_DIR)

# Compiler flags
CC=g++
CFLAGS=-Wall -Wno-write-strings -O3 -std=c++11
DEBUG=-g
INCLUDE_DIRS=-I$(ALL_DIR)
DEFINES=

# Files
CORE_SRC=$(CORE_DIR)net.cc $(CORE_DIR)node.cc $(CORE_DIR)mac.cc  \
	$(CORE_DIR)simulator.cc $(CORE_DIR)trace.cc $(CORE_DIR)energy.cc \
	$(CORE_DIR)link.cc $(CORE_DIR)senseEvent.cc

ALL_SRC=$(CORE_SRC) ./proto.cc ./main.cc

OUTPUT=$(BIN_DIR)sim

# Compilation
sim:	$(ALL_SRC)
	-mkdir $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(ALL_SRC) -o $(OUTPUT)

debug:	$(ALL_SRC)
	-mkdir $(BIN_DIR)
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDE_DIRS) $(ALL_SRC) -o $(OUTPUT)

clean:
	-rm *.o
	#-rm $(CORE_DIR)*.o
	-rm -rf $(BIN_DIR)sim
