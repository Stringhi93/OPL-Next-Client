EE_BIN = OPL_NEXT_BETA.ELF

EE_OBJS = \
	src/main.o \
	src/ui.o \
	src/input.o

EE_INCS = \
	-I$(PS2DEV)/gsKit/ee/gs/include \
	-I$(PS2DEV)/gsKit/ee/dma/include \
	-I$(PS2DEV)/gsKit/ee/toolkit/include

EE_CFLAGS += $(EE_INCS)

EE_LIBS = \
	-L$(PS2DEV)/gsKit/lib \
	-ldebug \
	-lpad \
	-lgskit_toolkit \
	-lgskit \
	-ldmakit \
	-lc

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
