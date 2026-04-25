#!/bin/bash
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B cmake
cmake --build cmake
gdb bin/app
