#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE:
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tr_pro_bv_37_DUTZC.c
OBJS1 = ${SRCS1:.c=.o}

SRCS2 = tr_pro_bv_37_DUTZR1.c
OBJS2 = ${SRCS2:.c=.o}

SRCS3 = tr_pro_bv_37_DUTZR2.c
OBJS3 = ${SRCS3:.c=.o}


$(BIN)tr_pro_bv_37_DUTZC$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tr_pro_bv_37_DUTZR1$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tr_pro_bv_37_DUTZR2$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)tr_pro_bv_37_DUTZC$(EXE) $(BIN)tr_pro_bv_37_DUTZR1$(EXE) $(BIN)tr_pro_bv_37_DUTZR2$(EXE)

all: $(BINS)
