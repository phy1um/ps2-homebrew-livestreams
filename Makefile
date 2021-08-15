
EE_BIN=test.elf
EE_OBJS=main.o gs.o mesh.o draw.o math.o

EE_LIBS=-ldma -lgraph -ldraw -lkernel -ldebug -lmath3d -lm

EE_CFLAGS += -Wall --std=c99
EE_LDFLAGS = -L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib


PS2SDK=/usr/local/ps2dev/ps2sdk

ISO_TGT=test.iso

ifdef PLATFORM
include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref
endif

$(ISO_TGT): $(EE_BIN)
	mkisofs -l -o $(ISO_TGT) $(EE_BIN) SYSTEM.CNF

.PHONY: docker-build
docker-build:
	docker run -v $(shell pwd):/src ps2build make $(ISO_TGT)


.PHONY: clean
clean:
	rm -rf $(ISO_TGT) $(EE_BIN) $(EE_OBJS)

.PHONY: run
run:
	PCSX2 --elf=$(PWD)/$(EE_BIN) 
