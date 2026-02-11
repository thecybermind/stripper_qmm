mkdir package
pushd package
del /q *
rem copy ..\README.md .\
rem copy ..\LICENSE .\

for %%G in (COD11MP CODMP CODUOMP JAMP JASP JK2MP JK2SP MOHAA MOHBT MOHSH Q2R Q3A QUAKE2 RTCWMP RTCWSP SIN SOF2MP SOF2SP STEF2 STVOYHM STVOYSP WET) do (
    copy ..\bin\Release-%%G\x86\stripper_qmm_%%G.dll .\
    copy ..\bin\Release-%%G\x64\stripper_qmm_x86_64_%%G.dll .\     
)
popd
