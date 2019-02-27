# PURPOSE: Trace test makefile
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS = bufpool_test.c

$(BIN)bufpool_test$(EXE) : $(OBJS) $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)bufpool_test$(EXE)

BINS_TEST=$(BINS)

all: $(BINS)
