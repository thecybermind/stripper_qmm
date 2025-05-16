mkdir package
pushd package
del /q *
rem copy ..\README.md .\
rem copy ..\LICENSE .\
rem copy ..\q3dm1.ini .\
rem copy ..\global.ini .\

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
         copy ..\bin\Release-%%x\x86\stripper_qmm_%%x.dll .\
         copy ..\bin\Release-%%x\x64\stripper_qmm_x86_64_%%x.dll .\         
       )
copy ..\bin\Release-Q2R\x64\stripper_qmm_x86_64_Q2R.dll .\         
popd
