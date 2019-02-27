# /***************************************************************************
# *                                                                          *
# * INSERT COPYRIGHT HERE!                                                   *
# *                                                                          *
# ****************************************************************************
# PURPOSE: Scheduler test makefile
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS = sched_test.c

$(BIN)sched_test$(EXE) : $(OBJS) $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)sched_test$(EXE)

BINS_TEST=$(BINS)

all: $(BINS)
