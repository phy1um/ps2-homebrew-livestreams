PS2SDK=/usr/local/ps2dev/ps2sdk

EE_BIN=test.elf
EE_OBJS=main.o gs.o mesh.o draw.o math.o pad.o
EE_LIBS=-ldma -lgraph -ldraw -lkernel -ldebug -lmath3d -lm -lpad
EE_CFLAGS += -Wall --std=c99
EE_LDFLAGS = -L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib

include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref

.PHONY: clean
clean:
	rm -rf $(EE_BIN) $(EE_OBJS)


