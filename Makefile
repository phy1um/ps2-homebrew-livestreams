ISO_TGT=test.iso
BIN=src/test.elf

PS2HOST?=192.168.20.99
PLATFORM=ps2

VERSION?=$(shell git rev-parse --short HEAD)
ISO_FLAGS?=-l --allow-lowercase -A "P2Garage Engine by Tom Marks -- coding.tommarks.xyz" -V "P2GARAGE:$(VERSION)"

LUA_FILES=$(shell find script -type f -name "*.lua")

LUA_LIB=src/liblua.a

CPPCHECK_REPORT=cppcheck_report.xml
CPPCHECK_IMG=ghcr.io/facthunder/cppcheck:latest
CPPCHECK_OUT=html/

PREFIX=src/

DIST_BIN_NAME=p2g.elf
PCSX2=pcsx2-qt 
 
include .lintvars

ifeq ($(USE_DOCKER), true)
.PHONY: dist
dist: docker-lua docker-elf assets
	cp $(BIN) dist/$(DIST_BIN_NAME)
else
.PHONY: dist
dist: $(LUA_LIB) $(BIN) assets
	cp $(BIN) dist/$(DIST_BIN_NAME)
endif

.PHONY: assets
assets: scripts
	if ! [ -d dist ]; then mkdir dist; fi
	$(MAKE) -C asset
	cp asset/*.tga dist/
	cp asset/*.bin dist/
	cp distfiles/* dist/
	cp LICENSE dist/

.PHONY: scripts
scripts:
	if ! [ -d dist/script ]; then mkdir -p dist/script; fi
	cp -r script/* dist/script

lua:
	git clone --depth 1 https://github.com/ps2dev/lua -b ee-v5.4.4
	cd lua && git apply ../lua.patch

$(LUA_LIB): lua
	make -C lua -f makefile
	cp lua/liblua.a src/

.PHONY: release
release: clean-all dist
	zip -r ps2-engine-$(VERSION).zip dist


# Run the engine
.PHONY: run
run: scripts
	$(PCSX2) $(shell pwd)/dist/$(DIST_BIN_NAME) 

.PHONY: runps2
runps2: scripts
	cd dist && ps2client -h $(PS2HOST) -t 10 execee host:$(DIST_BIN_NAME)

.PHONY: resetps2
resetps2:
	ps2client -h $(PS2HOST) -t 5 reset

.PHONY: runsim
runsim: scripts
	cd dist && ./sim

# Cleanup etc
.PHONY: clean-all
clean-all: 
	$(MAKE) -C src clean
	$(MAKE) -C asset clean
	$(MAKE) -C lua -f makefile clean || true
	rm -f $(CPPCHECK_REPORT)
	rm -rf $(CPPCHECK_OUT)
	rm -rf dist/


include src/Makefile
include quality.makefile

.PHONY: sim
sim: clean-all $(SIM_BIN) assets
	cp $(SIM_BIN) dist/sim
	
