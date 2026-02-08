for %%x in (COD11MP CODMP CODUOMP JAMP JASP JK2MP JK2SP MOHAA MOHBT MOHSH Q2R Q3A QUAKE2 RTCWMP RTCWSP SIN SOF2MP SOF2SP STEF2 STVOYHM STVOYSP WET) do (
    if [%%G] NEQ [Q2R] msbuild .\msvc\stripper_qmm.vcxproj /p:Configuration=Release-%%x /p:Platform=x86
    if errorlevel 1 exit /b errorlevel
    msbuild .\msvc\stripper_qmm.vcxproj /p:Configuration=Release-%%x /p:Platform=x64
    if errorlevel 1 exit /b errorlevel
)
