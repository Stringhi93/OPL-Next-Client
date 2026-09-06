EE_BIN = OPL_NEXT_BETA.ELF
EE_OBJS = src/main.o

EE_LIBS = -ldebug -lc

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal