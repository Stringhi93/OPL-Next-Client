EE_BIN = OPL_NEXT_BETA.ELF
EE_OBJS = src/main.o

PS2DEV ?= /usr/local/ps2dev
PS2SDK ?= $(PS2DEV)/ps2sdk

EE_CC = ee-gcc
EE_CFLAGS = -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -O2 -G0 -Wall
EE_LDFLAGS = -L$(PS2SDK)/ee/lib
EE_LIBS = -ldebug -lps2ip -lkernel -lsifrpc

all: $(EE_BIN)

%.o: %.c
	$(EE_CC) $(EE_CFLAGS) -c $< -o $@

$(EE_BIN): $(EE_OBJS)
	$(EE_CC) $(EE_CFLAGS) $(EE_LDFLAGS) -o $(EE_BIN) $(EE_OBJS) $(EE_LIBS)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)
