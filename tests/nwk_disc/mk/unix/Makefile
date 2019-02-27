#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS1 = nwk_formation_test.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = nwk_disc_test.c
OBJS2 = ${SRCS2:.c=.o}

$(BIN)nwk_formation_test$(EXE) : $(OBJS1)  $(LIBS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS) $(APS_LIB) $(ZDO_LIB)

$(BIN)nwk_disc_test$(EXE) : $(OBJS2)  $(LIBS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCPATH) -o $@ $< $(LIBS_WO_APS) $(LIBS_WO_APS) $(APS_LIB) $(ZDO_LIB)

BINS = $(BIN)nwk_formation_test$(EXE) $(BIN)nwk_disc_test$(EXE)

all: $(BINS)

#BINS_TEST='run_test.sh'
