#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS1 = zdo_start_zc.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = zdo_start_zr1.c
OBJS2 = ${SRCS2:.c=.o}
SRCS3 = zdo_start_zr2.c
OBJS3 = ${SRCS3:.c=.o}


$(BIN)zdo_start_zc$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)zdo_start_zr1$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)zdo_start_zr2$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)zdo_start_zc$(EXE) $(BIN)zdo_start_zr1$(EXE) $(BIN)zdo_start_zr2$(EXE)

all: $(BINS)

#BINS_TEST='run_test.sh'
