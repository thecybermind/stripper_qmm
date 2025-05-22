for %%x in (
        Q3A
        RTCWMP
		RTCWSP
		WET
		JAMP
		JK2MP
		SOF2MP
		STVOYHM
		STEF2
		MOHAA
		MOHBT
		MOHSH
		QUAKE2
       ) do (
         msbuild .\msvc\stripper_qmm.vcxproj /p:Configuration=Release-%%x /p:Platform=x86
         msbuild .\msvc\stripper_qmm.vcxproj /p:Configuration=Release-%%x /p:Platform=x64
       )

msbuild .\msvc\stripper_qmm.vcxproj /p:Configuration=Release-Q2R /p:Platform=x64
