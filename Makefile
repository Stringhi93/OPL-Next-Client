EE_BIN = OPL_NEXT_BETA.ELF

EE_OBJS = \
	src/main.o \
	src/ui.o \
	src/input.o \
	src/network.o \
	src/SIO2MAN_irx.o \
	src/PADMAN_irx.o \
	src/DEV9_irx.o \
	src/NETMAN_irx.o \
	src/SMAP_irx.o

EE_LIBS = \
	-lnetman \
	-lps2ip \
	-ldebug \
	-lpatches \
	-lpad \
	-lc

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN)
	rm -f $(EE_OBJS)

	rm -f src/SIO2MAN_irx.c
	rm -f src/PADMAN_irx.c
	rm -f src/DEV9_irx.c
	rm -f src/NETMAN_irx.c
	rm -f src/SMAP_irx.c

src/SIO2MAN_irx.c: $(PS2SDK)/iop/irx/sio2man.irx
	bin2c $< src/SIO2MAN_irx.c SIO2MAN_irx

src/PADMAN_irx.c: $(PS2SDK)/iop/irx/padman.irx
	bin2c $< src/PADMAN_irx.c PADMAN_irx

src/DEV9_irx.c: $(PS2SDK)/iop/irx/ps2dev9.irx
	bin2c $< src/DEV9_irx.c DEV9_irx

src/NETMAN_irx.c: $(PS2SDK)/iop/irx/netman.irx
	bin2c $< src/NETMAN_irx.c NETMAN_irx

src/SMAP_irx.c: $(PS2SDK)/iop/irx/smap.irx
	bin2c $< src/SMAP_irx.c SMAP_irx

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
