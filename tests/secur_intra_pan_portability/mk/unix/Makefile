#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS1 = ipp_secur_zc.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = ipp_secur_zr1.c
OBJS2 = ${SRCS2:.c=.o}
SRCS3 = ipp_secur_zr2.c
OBJS3 = ${SRCS3:.c=.o}


$(BIN)ipp_secur_zc$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)ipp_secur_zr1$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)ipp_secur_zr2$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)


BINS = $(BIN)ipp_secur_zc$(EXE) $(BIN)ipp_secur_zr1$(EXE) $(BIN)ipp_secur_zr2$(EXE)
all: $(BINS)

#BINS_TEST='run_test.sh'
