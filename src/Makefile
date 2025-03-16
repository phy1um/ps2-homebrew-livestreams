PS2SDK=/usr/local/ps2dev/ps2sdk
DVP-AS:=dvp-as
HOST_OBJS=$(CORE_OBJ) main.o ps2luaprog.o liblua.a
MAIN_OBJ=utils.o gs.o pad.o core.o director.o debug/vpu.o
CORE_OBJ=core/bindings.o core/buffer.o core/dma.o core/draw.o core/gslua.o core/log.o core/pad.o core/slotlist.o core/tga.o core/io.o core/vu.o core/vif.o math/vec2lua.o math/vec3lua.o math/mat3lua.o math/float.o math/mat4lua.o math/vec4lua.o
DRAW_OBJ=draw/draw2d.o draw/common.o draw/buffer.o draw/draw3d.o draw/vu.o
MATH_OBJ=ps2math.o 

EXAMPLES_SRC=$(wildcard $(PREFIX)examples/*.c)
EXAMPLES_BIN=$(patsubst %.c,%.elf,$(EXAMPLES_SRC))

VU1PROGS_SRC=$(wildcard $(PREFIX)vu1prog/*.asm)
VU1PROGS_OBJ=$(patsubst %.asm,%.vuobj,$(VU1PROGS_SRC))
VU1PROGS_BIN=$(patsubst %.vuobj,%.vuprog,$(VU1PROGS_OBJ))

BINDIR=$(PREFIX)bin/
DEBUG_OPTS=-DLOG_TRACE

EE_OBJS=$(MAIN_OBJ) $(DRAW_OBJ) $(MATH_OBJ) 
EE_LIBS=-ldma -lgraph -ldraw -lkernel -ldebug -lmath3d -lm -lpad 

EE_OBJS:=$(addprefix $(PREFIX), $(EE_OBJS))
HOST_OBJS:=$(addprefix $(PREFIX), $(HOST_OBJS))

EE_INCS+=-I$(PS2SDK)/ports/include -I./$(PREFIX)include
EE_LDFLAGS=-L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib -L$(PS2SDK)/ports/lib

SIM_OBJS=vdma.o vkernel.o vdraw.o vgraphics.o vpad.o draw_environment.o draw_clear.o
SIM_OBJS:=$(addprefix $(PREFIX)linux/, $(SIM_OBJS))

SIM_BIN=$(BINDIR)test.exe
P2G_LIB=$(BINDIR)libp2g.a

EE_CFLAGS+=-O0 -Wall --std=gnu17 -Wno-sign-compare -g -fno-strict-aliasing -fno-exceptions -DLUA_USE_PS2 $(DEBUG_OPTS)

SCRIPT_VU_EXTRACT=./$(PREFIX)build_scripts/extract_vu_prog.py

ifeq ($(platform), PS2)
$(info platform=PS2 detected)
include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref
else
INCS=-I $(PREFIX)linux/include -I./$(PREFIX)include
%.o: %.c
	$(CC) $(EE_CFLAGS) $(INCS) -c -o $@ $<
endif

$(P2G_LIB): $(EE_OBJS)
	$(EE_AR) rcs $@ $(EE_OBJS)

$(BINDIR)p2g.elf: $(P2G_LIB) $(EE_OBJS) $(HOST_OBJS)
	$(EE_CC) -T$(EE_LINKFILE) $(EE_OPTFLAGS) -o $@ $(EE_OBJS) $(HOST_OBJS) $(EE_LDFLAGS) $(EXTRA_LDFLAGS) $(EE_LIBS)

$(PREFIX)examples/%.elf: $(PREFIX)/examples/%.c $(P2G_LIB)
	$(EE_CC) -T$(EE_LINKFILE) $(DEBUG_OPTS) $(EE_INCS) $(EE_OPTFLAGS) -o $@ $< $(P2G_LIB) $(EE_LDFLAGS) $(EXTRA_LDFLAGS) $(EE_LIBS)

$(PREFIX)vu1prog/%.vuobj: $(PREFIX)vu1prog/%.asm
	$(DVP-AS) $(DVP_FLAGS) -o $@ $<

$(PREFIX)vu1prog/%.vuprog: $(PREFIX)vu1prog/%.vuobj
	$(SCRIPT_VU_EXTRACT) $< $@

$(SIM_BIN): $(EE_OBJS) $(SIM_OBJS)
	$(CC) $(EE_CFLAGS) -llua -lm -o $@ $(EE_OBJS) $(SIM_OBJS)

examples: $(EXAMPLES_BIN)

vu1progs: $(VU1PROGS_BIN)

bins: $(P2G_LIB) $(EXAMPLES_BIN) $(VU1PROGS_BIN) $(BINDIR)p2g.elf
	cp src/examples/*.elf src/bin
	cp src/vu1prog/*.vuprog src/bin

.PHONY: clean
clean:
	rm -rf $(EE_BIN) $(EE_OBJS) $(HOST_OBJS) $(SIM_BIN) $(SIM_OBJS) $(VU1PROGS_OBJ) $(VU1PROGS_BIN)

