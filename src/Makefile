PS2SDK=/usr/local/ps2dev/ps2sdk

MAIN_OBJ=main.o utils.o 
LUA_OBJ=gslua.o dmalua.o bufferlua.o tga.o padlua.o drawlua.o loglua.o ps2luaprog.o slotlistlua.o
DRAW_OBJ=draw/draw2d.o draw/buffer.o
MATH_OBJ=ps2math.o math/vec2lua.o math/vec3lua.o math/mat3lua.o math/float.o

EE_BIN=$(PREFIX)test.elf
EE_OBJS= $(MAIN_OBJ) $(LUA_OBJ) $(DRAW_OBJ) $(MATH_OBJ)
EE_LIBS=-ldma -lgraph -ldraw -lkernel -ldebug -lmath3d -lm -lpad -llua
EE_CFLAGS+=-Wall --std=c99 -Wno-sign-compare -fno-strict-aliasing -fno-exceptions -DLUA_USE_PS2 -DLOG_TRACE

EE_OBJS:=$(addprefix $(PREFIX), $(EE_OBJS))

EE_INCS+=-I$(PS2SDK)/ports/include
EE_LDFLAGS=-L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib -L$(PS2SDK)/ports/lib

ifeq ($(PLATFORM), ps2)
$(info Platform=ps2 detected)
include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref
endif

# TODO(Tom Marks): update ISO building to include everything in dist/
$(ISO_TGT): $(EE_BIN)
	mkisofs $(ISO_FLAGS) -o $(ISO_TGT) ./dist/*

.PHONY: clean
clean:
	rm -rf $(EE_BIN) $(EE_OBJS)




