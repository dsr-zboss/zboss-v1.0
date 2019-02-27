#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tp_nwk_bv_22_i_zc.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = tp_nwk_bv_22_i_zr.c
OBJS2 = ${SRCS2:.c=.o}
SRCS3 = tp_nwk_bv_22_i_zed.c
OBJS3 = ${SRCS3:.c=.ed.o}

$(BIN)tp_nwk_bv_22_i_zc$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_nwk_bv_22_i_zr$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tp_nwk_bv_22_i_zed$(EXE) : $(OBJS3)  $(LIBS_ED)
	$(LINK_BINARY_ED)

BINS = $(BIN)tp_nwk_bv_22_i_zc$(EXE) \
       $(BIN)tp_nwk_bv_22_i_zr$(EXE) \
       $(BIN)tp_nwk_bv_22_i_zed$(EXE)

all: $(BINS)

#BINS_TEST='run_test.sh'
