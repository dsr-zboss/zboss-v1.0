# PURPOSE: Trivial NWK test makefile
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

$(BIN)nwk_data_test_first$(EXE) : nwk_data_test.c $(LIBS)
	$(CC) $(CFLAGS) -DFIRST_TEST  $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS)

$(BIN)nwk_data_test_second$(EXE) : nwk_data_test.c $(LIBS)
	$(CC) $(CFLAGS) -DSECOND_TEST  $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS)

BINS = $(BIN)nwk_data_test_first$(EXE) $(BIN)nwk_data_test_second$(EXE)

all: $(BINS)

BINS_TEST='run_test.sh'
