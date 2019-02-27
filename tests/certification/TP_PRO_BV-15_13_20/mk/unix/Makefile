#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_pro_bv_15_zc.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = tp_pro_bv_15_zr1.c
OBJS2 = ${SRCS2:.c=.o}
SRCS3 = tp_pro_bv_15_zr2.c
OBJS3 = ${SRCS3:.c=.o}
SRCS4 = tp_pro_bv_15_zed1.c
OBJS4 = ${SRCS4:.c=.o}

$(BIN)tp_pro_bv_15_zc$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_15_zr1$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_15_zr2$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_15_zed1$(EXE) : $(OBJS4)  $(LIBS_ED)
	$(LINK_BINARY_ED)


BINS = $(BIN)tp_pro_bv_15_zc$(EXE) $(BIN)tp_pro_bv_15_zr1$(EXE) $(BIN)tp_pro_bv_15_zr2$(EXE) $(BIN)tp_pro_bv_15_zed1$(EXE)

all: $(BINS)

#BINS_TEST='run_test.sh'
