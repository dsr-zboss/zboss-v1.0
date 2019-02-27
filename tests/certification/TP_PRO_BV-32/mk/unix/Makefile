#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_pro_bv_32_gZC1.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = tp_pro_bv_32_gZC2.c
OBJS2 = ${SRCS2:.c=.o}
SRCS3 = tp_pro_bv_32_gZR1.c
OBJS3 = ${SRCS3:.c=.o}
SRCS4 = tp_pro_bv_32_DUTZR2.c
OBJS4 = ${SRCS4:.c=.o}

$(BIN)tp_pro_bv_32_gZC1$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_32_gZC2$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_32_gZR1$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_32_DUTZR2$(EXE) : $(OBJS4)  $(LIBS)
	$(LINK_BINARY)


BINS = $(BIN)tp_pro_bv_32_gZC1$(EXE) $(BIN)tp_pro_bv_32_gZC2$(EXE) $(BIN)tp_pro_bv_32_gZR1$(EXE) $(BIN)tp_pro_bv_32_DUTZR2$(EXE)

all: $(BINS)

#BINS_TEST='run_test.sh'
