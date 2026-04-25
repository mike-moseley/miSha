#!/bin/bash
cmake -S . -G "Unix Makefiles" -B cmake
if [ ! -f ./compile_commands.json ]; then
	ln -s cmake/compile_commands.json .
fi
