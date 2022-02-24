ISO_TGT=test.iso
BIN=dist/test.elf

PS2HOST?=192.168.20.99

DOCKER_IMG=ps2build
DOCKERFLAGS=--user "$(shell id -u):$(shell id -g)"
DOCKER?=docker

LUA_BRANCH=ee-v5.4.4

VERSION?=$(shell git rev-parse --short HEAD)
ISO_FLAGS?=-l --allow-lowercase -A "game by Tom Marks" -V "LGJ21 $(VERSION)"

LUA_FILES=$(shell find script -type f -name "*.lua")

include .lintvars

dist: $(BIN) assets

.PHONY: assets
assets: scripts
	if ! [ -d dist ]; then mkdir dist; fi
	$(MAKE) -C asset
	cp asset/*.bin dist/
	cp asset/*.tga dist/
	cp distfiles/* dist/
	cp LICENSE dist/

$(BIN): src/test.elf
	if ! [ -d dist ]; then mkdir dist; fi
	cp src/test.elf dist/test.elf

.PHONY: src/test.elf
src/test.elf:
	$(MAKE) platform=PS2 -C src test.elf


.PHONY: scripts
scripts:
	if ! [ -d dist/script ]; then mkdir -p dist/script; fi
	cp -r script/* dist/script

# TODO(Tom Marks): update ISO building to include everything in dist/
$(ISO_TGT): $(EE_BIN)
	mkisofs $(ISO_FLAGS) -o $(ISO_TGT) ./dist/*

.PHONY: docker-elf
docker-elf:
	$(DOCKER) run $(DOCKERFLAGS) -v $(shell pwd):/src $(DOCKER_IMG) make $(BIN)

.PHONY: docker-iso
docker-iso:
	$(DOCKER) run $(DOCKERFLAGS) -v $(shell pwd):/src $(DOCKER_IMG) make $(ISO_TGT)

.PHONY: clean
clean: 
	$(MAKE) -C src clean
	$(MAKE) -C asset clean
	rm -rf dist/

.PHONY: run
run: scripts
	pcsx2 --elf=$(PWD)/$(BIN) 

# TODO(Tom Marks): this could be improved, hard-coded ELF name is bad
.PHONY: runps2
runps2: scripts
	cd dist && ps2client -h $(PS2HOST) -t 10 execee host:test.elf

.PHONY: resetps2
resetps2:
	ps2client -h $(PS2HOST) -t 5 reset

.PHONY: lint
lint:
	cpplint --filter=$(CPPLINT_FILTERS) --counting=total --linelength=$(CPPLINT_LINE_LENGTH) --extensions=c,h --recursive .

.PHONY: lualint
lualint:
	luac5.3 -p $(LUA_FILES)

.PHONY: format
format:
	$(DOCKER) run $(DOCKERFLAGS) -v $(shell pwd):/workdir unibeautify/clang-format -i -sort-includes **/*.c **/*.h

.PHONY: release
release: clean assets docker-elf 
	mv $(BIN) dist/$(VERSION)-release.elf
	zip -r ps2-engine-$(VERSION).zip dist

deps:
	git clone https://github.com/ps2dev/lua --depth 1 --branch $(LUA_BRANCH) --single-branch src/lua

lua-samples:
	$(DOCKER) run $(DOCKERFLAGS) -v $(shell pwd):/src $(DOCKER_IMG) make -C src/lua/sample

docker-image:
	$(DOCKER) build -t $(DOCKER_IMG) .

