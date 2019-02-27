# PURPOSE: nwkAddrMap routines test makefile
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS=nwk_ext_neighbor_test.c

$(BIN)nwk_ext_neighbor_test$(EXE) : $(OBJS) $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)nwk_ext_neighbor_test$(EXE)

BINS_TEST=$(BINS)

all: $(BINS)
