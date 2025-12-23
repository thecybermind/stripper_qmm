for %%x in (
        JAMP
		JASP
		JK2MP
		JK2SP
		MOHAA
		MOHBT
		MOHSH
		QUAKE2
		Q3A
		RTCWMP
		RTCWSP
		SIN
		SOF2MP
		STEF2
		STVOYHM
		STVOYSP
		WET
       ) do (
         msbuild .\msvc\stripper_qmm.vcxproj /p:Configuration=Release-%%x /p:Platform=x86
         msbuild .\msvc\stripper_qmm.vcxproj /p:Configuration=Release-%%x /p:Platform=x64
       )

msbuild .\msvc\stripper_qmm.vcxproj /p:Configuration=Release-Q2R /p:Platform=x64
