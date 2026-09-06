EE_BIN = OPL_NEXT_BETA.ELF
EE_OBJS = src/main.o

EE_INCS = -I$(PS2SDK)/ee/include \
          -I$(PS2SDK)/common/include

EE_LDFLAGS = -L$(PS2SDK)/ee/lib

EE_LIBS = -ldebug -lkernel -lsifrpc -lps2ip

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eecomp