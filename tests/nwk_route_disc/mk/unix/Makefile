#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Route discovery test
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

$(BIN)nwk_route_discovery_source$(EXE) : nwk_route_discovery.c $(LIBS_WO_APS)
	$(CC) $(CFLAGS) -DNWK_ROUTE_SOURCE -Wno-format-zero-length  $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS)

$(BIN)nwk_route_discovery_destination$(EXE) : nwk_route_discovery.c $(LIBS)
	$(CC) $(CFLAGS) -DNWK_ROUTE_DESTINATION  $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS)

$(BIN)nwk_route_discovery_coordinator$(EXE) : nwk_route_discovery.c $(LIBS)
	$(CC) $(CFLAGS) -DNWK_ROUTE_COORDINATOR  $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS)

$(BIN)nwk_route_discovery$(EXE) : nwk_route_discovery.c $(LIBS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS)

$(BIN)nwk_route_discovery_no_router$(EXE) : nwk_route_discovery.c $(LIBS)
	$(CC) $(CFLAGS) -DNWK_NO_ROUTER $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS)

BINS = $(BIN)nwk_route_discovery_source$(EXE) $(BIN)nwk_route_discovery_destination$(EXE) $(BIN)nwk_route_discovery_coordinator$(EXE) $(BIN)nwk_route_discovery$(EXE) $(BIN)nwk_route_discovery_no_router$(EXE)

all: $(BINS)

BINS_TEST='run_test.sh'