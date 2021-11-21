PS2SDK=/usr/local/ps2dev/ps2sdk

EE_BIN=test.elf
EE_OBJS=main.o gslua.o dmalua.o bufferlua.o tga.o padlua.o draw/draw2d.o draw/buffer.o
EE_LIBS=-ldma -lgraph -ldraw -lkernel -ldebug -lmath3d -lm -lpad -llua
EE_CFLAGS+=-Wall --std=c99 -Wno-sign-compare -fno-strict-aliasing -fno-exceptions -DLUA_USE_PS2 -DFROM_HOST

EE_INCS+=-I$(PS2SDK)/ports/include
EE_LDFLAGS=-L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib -L$(PS2SDK)/ports/lib

ifeq ($(platform), PS2)
include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref
endif

.PHONY: clean
clean:
	rm -rf $(EE_BIN) $(EE_OBJS)




