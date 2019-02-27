#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Nwk sync test
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS1 = nwk_sync_zc.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = nwk_sync_ze.c
OBJS2 = ${SRCS2:.c=.ed.o}

$(BIN)nwk_sync_zc$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)nwk_sync_ze$(EXE) : $(OBJS2)  $(LIBS_ED)
	$(LINK_BINARY_ED)

BINS = $(BIN)nwk_sync_zc$(EXE) $(BIN)nwk_sync_ze$(EXE)
all: $(BINS)

#BINS_TEST='run_test.sh'
