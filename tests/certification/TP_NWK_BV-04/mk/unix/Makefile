#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE:
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_nwk_bv_04_gZC.c
OBJS1 = ${SRCS1:.c=.o}

SRCS2 = tp_nwk_bv_04_ZR.c
OBJS2 = ${SRCS2:.c=.o}

$(BIN)tp_nwk_bv_04_gZC$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_nwk_bv_04_ZR$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)tp_nwk_bv_04_gZC$(EXE) $(BIN)tp_nwk_bv_04_ZR$(EXE)

all: $(BINS)
