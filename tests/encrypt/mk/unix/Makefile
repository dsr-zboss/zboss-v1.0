
# PURPOSE: APS binding test makefile
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

INCPATH += -I$(BUILD_HOME)

SRCS=crypt_test.c

$(BIN)crypt_test$(EXE) : $(OBJS) $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)crypt_test$(EXE)

BINS_TEST=$(BINS)

all: $(BINS)
