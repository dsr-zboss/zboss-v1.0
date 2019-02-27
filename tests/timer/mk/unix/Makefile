# /***************************************************************************
# *                                                                          *
# * INSERT COPYRIGHT HERE!                                                   *
# *                                                                          *
# ****************************************************************************
# PURPOSE: Timer test makefile
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS = timer_test.c

$(BIN)timer_test$(EXE) : $(OBJS) $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)timer_test$(EXE)

BINS_TEST=$(BINS)

all: $(BINS)
