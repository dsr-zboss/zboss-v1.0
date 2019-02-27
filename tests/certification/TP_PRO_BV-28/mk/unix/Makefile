#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE:
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_pro_bv_28_DUTZC.c
OBJS1 = ${SRCS1:.c=.o}

SRCS2 = tp_pro_bv_28_DUTZR.c
OBJS2 = ${SRCS2:.c=.o}

SRCS3 = tp_pro_bv_28_gZED.c
OBJS3 = ${SRCS3:.c=.o}

$(BIN)tp_pro_bv_28_DUTZC$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_28_DUTZR$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_28_gZED$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)tp_pro_bv_28_DUTZC$(EXE) $(BIN)tp_pro_bv_28_DUTZR$(EXE) $(BIN)tp_pro_bv_28_gZED$(EXE)

all: $(BINS)
