PS2SDK=/usr/local/ps2dev/ps2sdk

MAIN_OBJ=main.o utils.o gs.o ps2luaprog.o
CORE_OBJ=core/bindings.o core/buffer.o core/dma.o core/draw.o core/gslua.o core/log.o core/pad.o core/slotlist.o core/tga.o core/io.o
DRAW_OBJ=draw/draw2d.o draw/common.o draw/buffer.o
MATH_OBJ=ps2math.o math/vec2lua.o math/vec3lua.o math/mat3lua.o math/float.o

EE_BIN=$(PREFIX)test.elf
EE_OBJS=$(MAIN_OBJ) $(CORE_OBJ) $(DRAW_OBJ) $(MATH_OBJ) liblua.a
EE_LIBS=-ldma -lgraph -ldraw -lkernel -ldebug -lmath3d -lm -lpad 
EE_CFLAGS+=-O0 -Wall --std=c99 -Wno-sign-compare -g -fno-strict-aliasing -fno-exceptions -DLUA_USE_PS2 -DLOG_TRACE

EE_OBJS:=$(addprefix $(PREFIX), $(EE_OBJS))

EE_INCS+=-I$(PS2SDK)/ports/include -I./$(PREFIX)include
EE_LDFLAGS=-L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib -L$(PS2SDK)/ports/lib

SIM_OBJS=vdma.o vkernel.o vdraw.o vgraphics.o vpad.o draw_environment.o draw_clear.o
SIM_OBJS:=$(addprefix $(PREFIX)linux/, $(SIM_OBJS))

SIM_BIN=$(PREFIX)test.exe

ifeq ($(PLATFORM), ps2)
$(info Platform=ps2 detected)
include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref
else
INCS=-I $(PREFIX)linux/include -I./$(PREFIX)include
%.o: %.c
	$(CC) $(EE_CFLAGS) $(INCS) -c -o $@ $<
endif

# TODO(Tom Marks): update ISO building to include everything in dist/
$(ISO_TGT): $(EE_BIN)
	mkisofs $(ISO_FLAGS) -o $(ISO_TGT) ./dist/*

$(SIM_BIN): $(EE_OBJS) $(SIM_OBJS)
	$(CC) $(EE_CFLAGS) -llua -lm -o $@ $(EE_OBJS) $(SIM_OBJS)


.PHONY: clean
clean:
	rm -rf $(EE_BIN) $(EE_OBJS) $(SIM_BIN) $(SIM_OBJS)




