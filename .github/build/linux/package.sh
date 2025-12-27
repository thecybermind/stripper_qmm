#!/bin/sh
mkdir -p package
cd package
rm -f *
cp ../README.md ./
cp ../LICENSE ./
cp ../global.ini ./
cp ../q3dm1.ini ./

for f in COD11MP CODMP CODUOMP JAMP JASP JK2MP JK2SP MOHAA MOHBT MOHSH Q3A QUAKE2 RTCWMP RTCWSP SIN SOF2MP STEF2 STVOYHM STVOYSP WET; do
  cp ../bin/release-$f/x86/stripper_qmm_$f.so ./
  cp ../bin/release-$f/x86_64/stripper_qmm_x86_64_$f.so ./
done

cd ..
