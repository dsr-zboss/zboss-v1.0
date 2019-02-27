BUILD_HOME=../..
include $(BUILD_HOME)/Options

SRCS = mac_linux_transport.c

$(BIN)mac_linux_transport$(EXE) : $(OBJS) $(LIBS)
	$(LINK_BINARY)

BINS = $(BIN)mac_linux_transport$(EXE)

BINS_TEST=$(BINS)

all: $(BINS)
