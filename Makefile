ISO_TGT=test.iso
BIN=dist/test.elf

PS2HOST?=192.168.20.99

DOCKER_IMG=ps2build
DOCKERFLAGS=--user "$(shell id -u):$(shell id -g)"
DOCKER?=sudo docker

include .lintvars

dist: $(BIN) assets

.PHONY: assets
assets:
	if ! [ -d dist ]; then mkdir dist; fi
	$(MAKE) -C asset
	cp asset/*.bin dist/

$(BIN):
	if ! [ -d dist ]; then mkdir dist; fi
	export PLATFORM=ps2
	$(MAKE) -C src test.elf
	cp src/test.elf dist/test.elf

# TODO(phy1um): update ISO building to include everything in dist/
$(ISO_TGT): $(EE_BIN)
	mkisofs -l -o $(ISO_TGT) $(BIN) dist/SYSTEM.CNF

.PHONY: docker-elf
docker-elf:
	$(DOCKER) run -v $(shell pwd):/src $(DOCKER_IMG) make $(BIN)


.PHONY: clean
clean:
	$(MAKE) -C src clean
	$(MAKE) -C asset clean
	rm -rf dist/

.PHONY: run
run:
	PCSX2 --elf=$(PWD)/$(BIN)

# TODO(phy1um): this could be improved, hard-coded ELF name is bad
.PHONY: runps2
runps2:
	cp dist && ps2client -h $(PS2HOST) -t 10 execee host:test.elf

.PHONY: resetps2
resetps2:
	ps2client -h $(PS2HOST) -t 5 reset

.PHONY: lint
lint:
	cpplint --filter=$(CPPLINT_FILTERS) --counting=total --linelength=$(CPPLINT_LINE_LENGTH) --extensions=c,h --recursive .

.PHONY: format
format:
	$(DOCKER) run $(DOCKERFLAGS) -v $(shell pwd):/workdir unibeautify/clang-format -i -sort-includes *.c *.h


