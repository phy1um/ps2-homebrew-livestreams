PS2SDK=/usr/local/ps2dev/ps2sdk

EE_BIN=test.elf
EE_OBJS=main.o gs.o mesh.o draw.o math.o pad.o
EE_LIBS=-ldma -lgraph -ldraw -lkernel -ldebug -lmath3d -lm -lpad
EE_CFLAGS += -Wall --std=c99
EE_LDFLAGS = -L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib

DOCKER_IMG=ps2build
DOCKERFLAGS=--user "$(shell id -u):$(shell id -g)"
DOCKER?=sudo docker


ifdef PLATFORM
include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref
endif

.PHONY: docker-build
docker-build:
	$(DOCKER) run -v $(shell pwd):/src $(DOCKER_IMG) make $(EE_BIN)

.PHONY: clean
clean:
	rm -rf $(EE_BIN) $(EE_OBJS)

.PHONY: run
run:
	PCSX2 --elf=$(PWD)/$(EE_BIN) 

.PHONY: runps2
runps2:
	ps2client -h $(PS2HOST) -t 10 execee host:$(EE_BIN)

.PHONY: resetps2
resetps2:
	ps2client -h $(PS2HOST) -t 5 reset

.PHONY: format
format:
	$(DOCKER) run $(DOCKERFLAGS) -v $(shell pwd):/workdir unibeautify/clang-format -i -sort-includes *.c *.h

