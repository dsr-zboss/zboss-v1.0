#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_pro_bv_30_gZC.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = tp_pro_bv_30_DUTZR1.c
OBJS2 = ${SRCS2:.c=.o}
SRCS3 = tp_pro_bv_30_DUTZR2.c
OBJS3 = ${SRCS3:.c=.o}

$(BIN)tp_pro_bv_30_gZC$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_30_DUTZR1$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_30_DUTZR2$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)


BINS = $(BIN)tp_pro_bv_30_gZC$(EXE) $(BIN)tp_pro_bv_30_DUTZR1$(EXE) $(BIN)tp_pro_bv_30_DUTZR2$(EXE)

all: $(BINS)

#BINS_TEST='run_test.sh'
