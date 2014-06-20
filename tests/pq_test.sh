#!/bin/sh
./pq_test | cmp pq_expected.txt /dev/stdin
