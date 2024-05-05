PS2SDK=/usr/local/ps2dev/ps2sdk
DVP-AS:=dvp-as

HOST_OBJS=$(CORE_OBJ) main.o ps2luaprog.o liblua.a
MAIN_OBJ=utils.o gs.o pad.o
CORE_OBJ=core/bindings.o core/buffer.o core/dma.o core/draw.o core/gslua.o core/log.o core/pad.o core/slotlist.o core/tga.o core/io.o
DRAW_OBJ=draw/draw2d.o draw/common.o draw/buffer.o draw/draw3d.o
MATH_OBJ=ps2math.o math/vec2lua.o math/vec3lua.o math/mat3lua.o math/float.o math/mat4lua.o math/vec4lua.o

EE_BIN=$(PREFIX)test.elf
# EE_OBJS=$(MAIN_OBJ) $(CORE_OBJ) $(DRAW_OBJ) $(MATH_OBJ) liblua.a
EE_OBJS=$(MAIN_OBJ) $(DRAW_OBJ) $(MATH_OBJ) 
EE_LIBS=-ldma -lgraph -ldraw -lkernel -ldebug -lmath3d -lm -lpad 
EE_CFLAGS+=-O0 -Wall --std=c99 -Wno-sign-compare -g -fno-strict-aliasing -fno-exceptions -DLUA_USE_PS2 -DLOG_TRACE

EE_OBJS:=$(addprefix $(PREFIX), $(EE_OBJS))
HOST_OBJS:=$(addprefix $(PREFIX), $(HOST_OBJS))

EE_INCS+=-I$(PS2SDK)/ports/include -I./$(PREFIX)include
EE_LDFLAGS=-L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib -L$(PS2SDK)/ports/lib

SIM_OBJS=vdma.o vkernel.o vdraw.o vgraphics.o vpad.o draw_environment.o draw_clear.o
SIM_OBJS:=$(addprefix $(PREFIX)linux/, $(SIM_OBJS))

SIM_BIN=$(PREFIX)test.exe

ifeq ($(platform), PS2)
$(info platform=PS2 detected)
include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref
else
INCS=-I $(PREFIX)linux/include -I./$(PREFIX)include
%.o: %.c
	$(CC) $(EE_CFLAGS) $(INCS) -c -o $@ $<
endif

vu1prog/%.vuobj: vu1prog/%.asm
	$(DVP-AS) $(DVP_FLAGS) -o $@ $<

# TODO(Tom Marks): update ISO building to include everything in dist/
$(ISO_TGT): $(EE_BIN)
	mkisofs $(ISO_FLAGS) -o $(ISO_TGT) ./dist/*

$(SIM_BIN): $(EE_OBJS) $(SIM_OBJS)
	$(CC) $(EE_CFLAGS) -llua -lm -o $@ $(EE_OBJS) $(SIM_OBJS)


.PHONY: clean
clean:
	rm -rf $(EE_BIN) $(EE_OBJS) $(HOST_OBJS) $(SIM_BIN) $(SIM_OBJS)

$(PREFIX)libp2g.a: $(EE_OBJS)
	$(EE_AR) rcs $@ $(EE_OBJS)

$(PREFIX)p2g.elf: $(PREFIX)libp2g.a $(EE_OBJS) $(HOST_OBJS)
	$(EE_CC) -T$(EE_LINKFILE) $(EE_OPTFLAGS) -o $@ $(EE_OBJS) $(HOST_OBJS) $(EE_LDFLAGS) $(EXTRA_LDFLAGS) $(EE_LIBS)


