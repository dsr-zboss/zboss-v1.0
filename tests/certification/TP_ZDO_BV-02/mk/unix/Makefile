#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE:
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_zdo_bv_02_zc.c
OBJS1 = ${SRCS1:.c=.o}

SRCS2 = tp_zdo_bv_02_zr.c
OBJS2 = ${SRCS2:.c=.o}

SRCS3 = tp_zdo_bv_02_zed.c
OBJS3 = ${SRCS3:.c=.ed.o}

$(BIN)tp_zdo_bv_02_zc$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_zdo_bv_02_zr$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_zdo_bv_02_zed$(EXE) : $(OBJS3)  $(LIBS_ED)
	$(LINK_BINARY_ED)

BINS = $(BIN)tp_zdo_bv_02_zc$(EXE) $(BIN)tp_zdo_bv_02_zr$(EXE) $(BIN)tp_zdo_bv_02_zed$(EXE)

all: $(BINS)
