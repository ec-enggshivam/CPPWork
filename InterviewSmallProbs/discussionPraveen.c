tresos_cmd.bat -Dtarget=AURIX2G -Dderivate=TC399 -DecuResourceModuleIds=Resource_AurixAS422 -Dsubderivative=TC399x legacy convert Fls_17_Dmu.xdm Fls_17_Dmu399.bmd@asc:4.2.2

tresos_cmd.bat -Dtarget=AURIX2G -Dderivate=TC397 -DecuResourceModuleIds=Resource_AurixAS422 -Dsubderivative=TC397x legacy convert Fls_17_Dmu.xdm Fls_17_Dmu397.bmd@asc:4.2.2

Amdc.exe -s Fls_17_Dmu399.bmd > Fls_17_Dmu399.txt

Amdc.exe -s Fls_17_Dmu397.bmd > Fls_17_Dmu397.txt