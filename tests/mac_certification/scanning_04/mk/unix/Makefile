# PURPOSE: Trivial NWK test makefile
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

$(BIN)zdo_start_zc_s04$(EXE) : zdo_start_zc_s04.c $(LIBS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCPATH) -o $@ $< $(LIBS) $(LIBS)

$(BIN)start_ze_s04$(EXE) : start_ze_s04.c $(LIBS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS)

BINS = $(BIN)zdo_start_zc_s04$(EXE) $(BIN)start_ze_s04$(EXE)

all: $(BINS)

BINS_TEST='test.sh'
