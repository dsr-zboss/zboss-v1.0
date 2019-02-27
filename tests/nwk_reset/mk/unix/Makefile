#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE:
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS1 = nwk_reset_zc.c
OBJS1 = ${SRCS1:.c=.o}

SRCS2 = nwk_reset_ze.c
OBJS2 = ${SRCS2:.c=.ed.o}

$(BIN)nwk_reset_zc$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)nwk_reset_ze$(EXE) : $(OBJS2)  $(LIBS_ED)
	$(LINK_BINARY_ED)

BINS = $(BIN)nwk_reset_zc$(EXE) $(BIN)nwk_reset_ze$(EXE)

all: $(BINS)
