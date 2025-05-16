#!/bin/sh
mkdir -p package
cd package
rm -f *
cp ../README.md ./
cp ../LICENSE ./
cp ../q3dm1.ini ./
cp ../global.ini ./

for x in Q3A RTCWMP RTCWSP WET JAMP JK2MP STVOYHM SOF2MP STEF2 MOHAA MOHBT MOHSH QUAKE2; do
  cp ../bin/release-$x/x86/stripper_qmm_$x.so ./
  cp ../bin/release-$x/x86_64/stripper_qmm_x86_64_$x.so ./
done 

cd ..
