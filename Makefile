EE_BIN = OPL_NEXT_BETA.ELF

EE_OBJS = \
	src/main.o \
	src/ui.o \
	src/input.o \
	src/network.o \
	ps2dev9_irx.o \
	netman_irx.o \
	smap_irx.o

EE_LIBS = \
	-lsbv_patches \
	-lnetman \
	-lps2ip \
	-ldebug \
	-lpad \
	-lc

all: $(EE_BIN)

# Embute os drivers de rede dentro do ELF.
ps2dev9_irx.s:
	bin2s $(PS2SDK)/iop/irx/ps2dev9.irx ps2dev9_irx.s ps2dev9_irx

netman_irx.s:
	bin2s $(PS2SDK)/iop/irx/netman.irx netman_irx.s netman_irx

smap_irx.s:
	bin2s $(PS2SDK)/iop/irx/smap.irx smap_irx.s smap_irx

ps2dev9_irx.o: ps2dev9_irx.s
netman_irx.o: netman_irx.s
smap_irx.o: smap_irx.s

clean:
	rm -f $(EE_OBJS) $(EE_BIN)
	rm -f ps2dev9_irx.s netman_irx.s smap_irx.s

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
