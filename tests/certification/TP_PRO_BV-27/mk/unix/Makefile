#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE:
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_pro_bv_27_gZC.c
OBJS1 = ${SRCS1:.c=.o}

SRCS2 = tp_pro_bv_27_DUTZED1.c
OBJS2 = ${SRCS2:.c=.ed.o}

SRCS3 = tp_pro_bv_27_DUTZED2.c
OBJS3 = ${SRCS3:.c=.ed.o}

$(BIN)tp_pro_bv_27_gZC$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_27_DUTZED1$(EXE) : $(OBJS2)  $(LIBS_ED)
	$(LINK_BINARY_ED)

$(BIN)tp_pro_bv_27_DUTZED2$(EXE) : $(OBJS3)  $(LIBS_ED)
	$(LINK_BINARY_ED)

BINS = $(BIN)tp_pro_bv_27_gZC$(EXE) $(BIN)tp_pro_bv_27_DUTZED1$(EXE) $(BIN)tp_pro_bv_27_DUTZED2$(EXE)

all: $(BINS)
