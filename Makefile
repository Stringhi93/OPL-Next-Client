
EE_BIN = OPL_NEXT_BETA.ELF
EE_OBJS = src/main.o

EE_INCS = -I$(PS2DEV)/ps2sdk/ee/include -I$(PS2DEV)/ps2sdk/common/include
EE_LDFLAGS = -L$(PS2DEV)/ps2sdk/ee/lib
EE_LIBS = -ldebug -lps2ip -lkernel -lsifrpc

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

include $(PS2DEV)/ps2sdk/samples/Makefile.pref
include $(PS2DEV)/ps2sdk/samples/Makefile.eecomp
