# PURPOSE: Trivial NWK test makefile
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

$(BIN)zdo_start_zc_s06_1$(EXE) : zdo_start_zc_s06.c $(LIBS)
	$(CC) $(CFLAGS) -DZB_TEST_COORD_NUMBER=1 $(DEFINES) $(INCPATH) -o $@ $< $(LIBS) $(LIBS)

$(BIN)zdo_start_zc_s06_2$(EXE) : zdo_start_zc_s06.c $(LIBS)
	$(CC) $(CFLAGS) -DZB_TEST_COORD_NUMBER=2 $(DEFINES) $(INCPATH) -o $@ $< $(LIBS) $(LIBS)

$(BIN)zdo_start_zc_s06_3$(EXE) : zdo_start_zc_s06.c $(LIBS)
	$(CC) $(CFLAGS) -DZB_TEST_COORD_NUMBER=3 $(DEFINES) $(INCPATH) -o $@ $< $(LIBS) $(LIBS)


$(BIN)start_ze_s06$(EXE) : start_ze_s06.c $(LIBS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS)

BINS = $(BIN)zdo_start_zc_s06_1$(EXE) $(BIN)zdo_start_zc_s06_2$(EXE) $(BIN)zdo_start_zc_s06_3$(EXE) $(BIN)start_ze_s06$(EXE)

all: $(BINS)

BINS_TEST='test.sh'
