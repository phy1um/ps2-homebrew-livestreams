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
	$(MAKE) -C asset
	cp asset/*.bin dist/

$(BIN):
	$(MAKE) -C src test.elf
	cp src/test.elf dist/test.elf

$(ISO_TGT): $(EE_BIN)
	mkisofs -l -o $(ISO_TGT) $(EE_BIN) SYSTEM.CNF

.PHONY: docker-elf
docker-elf:
	$(DOCKER) run -v $(shell pwd):/src $(DOCKER_IMG) make $(BIN)


.PHONY: clean
clean:
	$(MAKE) -C src clean
	$(MAKE) -C asset clean
	rm -rf dist/*

.PHONY: run
run:
	PCSX2 --elf=$(PWD)/$(BIN)

.PHONY: runps2
runps2:
	cp dist && ps2client -h $(PS2HOST) -t 10 execee host:$(BIN)

.PHONY: resetps2
resetps2:
	ps2client -h $(PS2HOST) -t 5 reset

.PHONY: lint
lint:
	cpplint --filter=$(CPPLINT_FILTERS) --counting=total --linelength=$(CPPLINT_LINE_LENGTH) --extensions=c,h --recursive .

.PHONY: format
format:
	$(DOCKER) run $(DOCKERFLAGS) -v $(shell pwd):/workdir unibeautify/clang-format -i -sort-includes *.c *.h

