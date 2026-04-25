#!/bin/bash
./clangd_cmake.sh
cmake --build cmake
./bin/app
