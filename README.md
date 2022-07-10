# P2Garage Library

Playstation 2 Homebrew for Humans

## What It Is 

This is a framework for creating interactive media on the Playstation 2. 

## What This Is Not

P2Garage is _NOT_ a game engine. This library does a lot to simplify building PS2 software.
It has a lot of useful abstractions, makes graphics nice and provides a Lua scripting interface. 
to Playstation 2 hardware. The library does not have strong opinions on how to build a game.
There are still lots of problems to solve.

## How To Use

This framework is built on PS2SDK, a community SDK that can be installed from [here.](https://github.com/ps2dev/ps2dev).
The Makefiles in this project assume Docker is installed and will use a docker image to compile everything.
Building without Docker is supported, but not supported as a first-class way of compiling the project.

Running `make dist` will compile the code and assemble some key assets. The .elf file in the `/dist` folder
can be executed on a PS2 using a homebrew launcher such as [wLaunchELF](https://github.com/ps2homebrew/wLaunchELF)
or [ps2link](https://github.com/ps2dev/ps2link). It can also be run in the [PCSX2](https://pcsx2.net/) emulator.

When the 

## Current Status

-[x] Lua Scripting
-[x] 2D Renderer
-[ ] 3D Renderer
-[ ] 

