xcopy ..\..\..\eng-software-pod\FIRMWARE\PROJECT_CODE\LCCM656__RLOOP__PI_COMMS\* ..\pi_comm /s /e /h /Y

SET FileToDelete="version.d"
IF EXIST %FileToDelete% del /F %FileToDelete%

SET FileToDelete="version.obj"
IF EXIST %FileToDelete% del /F %FileToDelete%
