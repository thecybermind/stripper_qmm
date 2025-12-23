#!/bin/sh
mkdir -p package
cd package
rm -f *
cp ../README.md ./
cp ../LICENSE ./
cp ../q3dm1.ini ./
cp ../global.ini ./

for x in JAMP JASP JK2MP JK2SP MOHAA MOHBT MOHSH QUAKE2 Q3A RTCWMP RTCWSP SIN SOF2MP STEF2 STVOYHM STVOYSP WET; do
  cp ../bin/release-$x/x86/stripper_qmm_$x.so ./
  cp ../bin/release-$x/x86_64/stripper_qmm_x86_64_$x.so ./
done 

cd ..
