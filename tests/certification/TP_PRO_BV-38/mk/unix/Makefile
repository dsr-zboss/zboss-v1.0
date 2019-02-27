#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE:
# */

BUILD_HOME=../../..
include $(BUILD_HOME)/Options

SRCS1 = tr_pro_bv_38_DUTZC.c
OBJS1 = ${SRCS1:.c=.o}

SRCS2 = tr_pro_bv_38_DUTZR1.c
OBJS2 = ${SRCS2:.c=.o}


$(BIN)tr_pro_bv_38_DUTZC$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)tr_pro_bv_38_DUTZR1$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)tr_pro_bv_38_DUTZC$(EXE) $(BIN)tr_pro_bv_38_DUTZR1$(EXE)

all: $(BINS)
