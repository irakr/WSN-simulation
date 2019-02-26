# Makefile for WSN simulator

# Directories
CORE_DIR=./core/
BIN_DIR=./
ALL_DIR=$(CORE_DIR)

# Compiler flags
CC=g++
CFLAGS=-Wall -Wno-write-strings -std=c++11
DEBUG=-g
INCLUDE_DIRS=-I$(ALL_DIR)
DEFINES=

# PThread library
LIB_PTHREAD=-lpthread

# Files
CORE_SRC=$(CORE_DIR)net.cc $(CORE_DIR)node.cc $(CORE_DIR)mac.cc  \
	$(CORE_DIR)simulator.cc $(CORE_DIR)trace.cc $(CORE_DIR)energy.cc \
	$(CORE_DIR)link.cc $(CORE_DIR)senseEvent.cc $(CORE_DIR)clTable.cc \
	$(CORE_DIR)cbrEvent.cc

ALL_SRC=$(CORE_SRC) ./main.cc

ALL_HEADERS=$(CORE_DIR)net.h $(CORE_DIR)node.h $(CORE_DIR)mac.h  \
	$(CORE_DIR)simulator.h $(CORE_DIR)trace.h $(CORE_DIR)energy.h \
	$(CORE_DIR)link.h $(CORE_DIR)senseEvent.h $(CORE_DIR)cbrEvent.h	\

OUTPUT=$(BIN_DIR)sim

# Compilation
sim:	$(ALL_SRC) $(ALL_HEADERS)
	-mkdir $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(ALL_SRC) -o $(OUTPUT) $(LIB_PTHREAD)

debug:	$(ALL_SRC) $(ALL_HEADERS)
	-mkdir $(BIN_DIR)
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDE_DIRS) $(ALL_SRC) -o $(OUTPUT) $(LIB_PTHREAD)

clean:
	-rm *.o
	-rm *.tr
	#-rm $(CORE_DIR)*.o
	-rm -rf $(BIN_DIR)sim
