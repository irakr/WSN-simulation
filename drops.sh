#!/bin/sh

file=$1

awk -f drops.awk traces/trace_16_cbr.tr nodes=16 > $file
awk -f drops.awk traces/trace_40_cbr.tr nodes=40 >> $file
awk -f drops.awk traces/trace_64_0.tr nodes=64 >> $file
awk -f drops.awk traces/trace_94_0.tr nodes=94 >> $file
