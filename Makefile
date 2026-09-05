EE_BIN = OPL_NEXT_BETA.ELF
EE_OBJS = src/main.o

EE_INCS = -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include
EE_LDFLAGS = -L$(PS2SDK)/ee/lib
EE_LIBS = -ldebug -lps2ip -lkernel -lsifrpc

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

# Inclui os arquivos de configuracao oficiais da PS2SDK moderna
-include $(PS2SDK)/samples/Makefile.pref
-include $(PS2SDK)/samples/Makefile.eecomp
-include $(PS2SDK)/Defs.make
-include $(PS2SDK)/rules/Makefile.eecomp
