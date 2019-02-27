#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_sec_bv_01_zc.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = tp_sec_bv_01_zr1.c
OBJS2 = ${SRCS2:.c=.o}
SRCS3 = tp_sec_bv_01_zr2.c
OBJS3 = ${SRCS3:.c=.o}
SRCS4 = tp_sec_bv_01_zed1.c
OBJS4 = ${SRCS4:.c=.o}
SRCS5 = tp_sec_bv_01_zr3.c
OBJS5 = ${SRCS5:.c=.o}

$(BIN)tp_sec_bv_01_zc$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_sec_bv_01_zr1$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_sec_bv_01_zr2$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_sec_bv_01_zr3$(EXE) : $(OBJS5)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_sec_bv_01_zed1$(EXE) : $(OBJS4)  $(LIBS)
	$(LINK_BINARY)


BINS = $(BIN)tp_sec_bv_01_zc$(EXE) $(BIN)tp_sec_bv_01_zr1$(EXE) $(BIN)tp_sec_bv_01_zr2$(EXE) $(BIN)tp_sec_bv_01_zr3$(EXE) $(BIN)tp_sec_bv_01_zed1$(EXE)

all: $(BINS)

#BINS_TEST='run_test.sh'
