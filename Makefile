ISO_TGT=test.iso
BIN=src/test.elf

PS2HOST?=192.168.20.99

DOCKER_IMG=ps2build
DOCKERFLAGS=--user "$(shell id -u):$(shell id -g)"
DOCKER?=docker

VERSION?=$(shell git rev-parse --short HEAD)
ISO_FLAGS?=-l --allow-lowercase -A "P2Garage Engine by Tom Marks -- coding.tommarks.xyz" -V "P2GARAGE:$(VERSION)"

LUA_FILES=$(shell find script -type f -name "*.lua")

CPPCHECK_REPORT=cppcheck_report.xml
CPPCHECK_IMG=ghcr.io/facthunder/cppcheck:latest
CPPCHECK_OUT=html/

PREFIX=src/

DIST_BIN_NAME=P2G_$(VERSION).elf

include .lintvars

.PHONY: dist
dist: docker-elf assets
	cp $(BIN) dist/$(DIST_BIN_NAME)

.PHONY: assets
assets: scripts
	if ! [ -d dist ]; then mkdir dist; fi
	$(MAKE) -C asset
	cp asset/*.bin dist/
	cp asset/*.tga dist/
	cp distfiles/* dist/
	cp LICENSE dist/

.PHONY: scripts
scripts:
	if ! [ -d dist/script ]; then mkdir -p dist/script; fi
	cp -r script/* dist/script

.PHONY: release
release: clean-all assets docker-elf 
	zip -r ps2-engine-$(VERSION).zip dist


# Docker rules
.PHONY: docker-image
docker-image:
	$(DOCKER) build -t $(DOCKER_IMG) .

.PHONY: docker-elf
docker-elf:
	$(DOCKER) run --rm $(DOCKERFLAGS) -v $(shell pwd):/src $(DOCKER_IMG) make $(BIN)

.PHONY: docker-iso
docker-iso:
	$(DOCKER) run --rm $(DOCKERFLAGS) -v $(shell pwd):/src $(DOCKER_IMG) make $(ISO_TGT)

.PHONY: docker-lua
docker-lua:
	$(DOCKER) run --rm $(DOCKERFLAGS) -v $(shell pwd):/src $(DOCKER_IMG) bash -c "platform=PS2 make lualib"

.PHONY: lualib
lualib:
	make -C lua -f makefile clean
	make -C lua -f makefile
	cp lua/liblua.a src/

# Run the engine
.PHONY: run
run: scripts
	pcsx2 --elf=$(shell pwd)/dist/$(DIST_BIN_NAME)

.PHONY: runps2
runps2: scripts
	cd dist && ps2client -h $(PS2HOST) -t 10 execee host:$(DIST_BIN_NAME)

.PHONY: resetps2
resetps2:
	ps2client -h $(PS2HOST) -t 5 reset

# Cleanup etc
.PHONY: clean-all
clean-all: 
	$(MAKE) -C src clean
	$(MAKE) -C asset clean
	rm -f $(CPPCHECK_REPORT)
	rm -rf $(CPPCHECK_OUT)
	rm -rf dist/


include src/Makefile
include quality.makefile

