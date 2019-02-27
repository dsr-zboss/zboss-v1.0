# PURPOSE: Trivial NWK test makefile
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

$(BIN)zdo_start_zc$(EXE) : zdo_start_zc.c $(LIBS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCPATH) -o $@ $< $(LIBS) $(LIBS)

$(BIN)start_ze$(EXE) : start_ze.c $(LIBS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS)

BINS = $(BIN)zdo_start_zc$(EXE) $(BIN)start_ze$(EXE)

all: $(BINS)

BINS_TEST='test.sh'
