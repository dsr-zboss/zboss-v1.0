# PURPOSE: Trivial NWK test makefile
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

$(BIN)nwk_data_test_first$(EXE) : nwk_data_test.c $(LIBS)
	$(CC) $(CFLAGS) -DFIRST_TEST  -DZB8051 $(DEFINES) $(INCPATH) -o $@ $< $(LIBS)
	packihx nwk_data_test_first.ihx > nwk_data_test_first.hex

$(BIN)nwk_data_test_second$(EXE) : nwk_data_test.c $(LIBS)
	$(CC) $(CFLAGS) -DSECOND_TEST -DZB8051 $(DEFINES) $(INCPATH) -o $@ $< $(LIBS)
	packihx nwk_data_test_second.ihx > nwk_data_test_second.hex

BINS = $(BIN)nwk_data_test_first$(EXE) $(BIN)nwk_data_test_second$(EXE)

BINS_TEST=$(BINS)

all: $(BINS)
