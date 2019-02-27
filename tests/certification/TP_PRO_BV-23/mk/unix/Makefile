#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_pro_bv_23_zc1.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = tp_pro_bv_23_zc2.c
OBJS2 = ${SRCS2:.c=.o}
SRCS3 = tp_pro_bv_23_zr2.c
OBJS3 = ${SRCS3:.c=.o}
SRCS4 = tp_pro_bv_23_zr3.c
OBJS4 = ${SRCS4:.c=.o}

$(BIN)tp_pro_bv_23_zc1$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_23_zc2$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_23_zr2$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_pro_bv_23_zr3$(EXE) : $(OBJS4)  $(LIBS)
	$(LINK_BINARY)



BINS = $(BIN)tp_pro_bv_23_zc1$(EXE) $(BIN)tp_pro_bv_23_zc2$(EXE) $(BIN)tp_pro_bv_23_zr2$(EXE) $(BIN)tp_pro_bv_23_zr3$(EXE)

all: $(BINS)

#BINS_TEST='run_test.sh'
