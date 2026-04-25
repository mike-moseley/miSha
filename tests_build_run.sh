#!/bin/bash
cmake --build cmake --target tests
failed=0
for test in bin/test/*
do
	./$test || failed=1
done
exit $failed
