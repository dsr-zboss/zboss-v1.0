#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
# PURPOSE: Formation and discovery tests.
# */

BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS1 = nwk_report_update_zc1.c
OBJS1 = ${SRCS1:.c=.o}
SRCS2 = nwk_report_update_zc2.c
OBJS2 = ${SRCS2:.c=.o}
SRCS3 = nwk_report_update_zr1.c
OBJS3 = ${SRCS3:.c=.o}
SRCS4 = nwk_report_update_zr2.c
OBJS4 = ${SRCS4:.c=.o}
SRCS5 = nwk_report_update_zr3.c
OBJS5 = ${SRCS5:.c=.o}

$(BIN)nwk_report_update_zc1$(EXE) : $(OBJS1)  $(LIBS)
	$(LINK_BINARY)

$(BIN)nwk_report_update_zc2$(EXE) : $(OBJS2)  $(LIBS)
	$(LINK_BINARY)

$(BIN)nwk_report_update_zr1$(EXE) : $(OBJS3)  $(LIBS)
	$(LINK_BINARY)

$(BIN)nwk_report_update_zr2$(EXE) : $(OBJS4)  $(LIBS)
	$(LINK_BINARY)

$(BIN)nwk_report_update_zr3$(EXE) : $(OBJS5)  $(LIBS)
	$(LINK_BINARY)



BINS = $(BIN)nwk_report_update_zc1$(EXE) $(BIN)nwk_report_update_zc2$(EXE) $(BIN)nwk_report_update_zr1$(EXE) $(BIN)nwk_report_update_zr2$(EXE) $(BIN)nwk_report_update_zr3$(EXE)

all: $(BINS)

#BINS_TEST='run_test.sh'
