# PURPOSE: Trivial NWK test makefile
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

$(BIN)aps_data_test_first$(EXE) : aps_data_test.c $(LIBS)
	$(CC) $(CFLAGS) -DFIRST_TEST  $(DEFINES) $(INCPATH) -o $@ $< $(LIBS) $(LIBS)

$(BIN)aps_data_test_second$(EXE) : aps_data_test.c $(LIBS)
	$(CC) $(CFLAGS) -DSECOND_TEST  $(DEFINES) $(INCPATH) -o $@ $< $(LIBS) $(LIBS)

BINS = $(BIN)aps_data_test_first$(EXE) $(BIN)aps_data_test_second$(EXE)

all: $(BINS)

BINS_TEST='run_test.sh'
