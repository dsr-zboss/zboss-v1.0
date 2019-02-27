
# PURPOSE: APS binding test makefile
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS=aps_binding_test.c

$(BIN)aps_binding_test$(EXE) : $(OBJS) $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)aps_binding_test$(EXE)

BINS_TEST=$(BINS)

all: $(BINS)
