#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE:
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_zdo_bv_06_DUTZC.c
OBJS1 = ${SRCS1:.c=.o}

SRCS2 = tp_zdo_bv_06_gZED1.c
OBJS2 = ${SRCS2:.c=.o}

SRCS3 = tp_zdo_bv_06_gZED2.c
OBJS3 = ${SRCS3:.c=.o}

$(BIN)tp_zdo_bv_06_DUTZC$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_zdo_bv_06_gZED1$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_zdo_bv_06_gZED2$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)tp_zdo_bv_06_DUTZC$(EXE) $(BIN)tp_zdo_bv_06_gZED1$(EXE) $(BIN)tp_zdo_bv_06_gZED2$(EXE)

all: $(BINS)
