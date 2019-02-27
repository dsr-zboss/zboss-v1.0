# PURPOSE: Trace test makefile
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS = serial_test.c

# $(BIN)serial_test$(EXE) : $(OBJS) (LIBS)

$(BIN)serial_test$(EXE) : $(OBJS) $(LIBS)
	$(LINK_BINARY)
	packihx serial_test.ihx > serial_test.hex

BINS = $(BIN)serial_test$(EXE)

BINS_TEST=$(BINS)

all: $(BINS)
