for %%G in (COD11MP CODMP CODUOMP JAMP JASP JK2MP JK2SP MOHAA MOHBT MOHSH Q2R Q3A QUAKE2 RTCWMP RTCWSP SIN SOF2MP STEF2 STVOYHM STVOYSP WET) do (
    msbuild .\msvc\stripper_qmm.vcxproj /p:Configuration=Release-%%G /p:Platform=x86
    msbuild .\msvc\stripper_qmm.vcxproj /p:Configuration=Release-%%G /p:Platform=x64
)
