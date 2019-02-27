#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS1 = request_key_zc.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = request_key_zr1.c
OBJS2 = ${SRCS2:.c=.o}

$(BIN)request_key_zc$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)request_key_zr1$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)request_key_zr2$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)


BINS = $(BIN)request_key_zc$(EXE) $(BIN)request_key_zr1$(EXE)

all: $(BINS)

#BINS_TEST='run_test.sh'
