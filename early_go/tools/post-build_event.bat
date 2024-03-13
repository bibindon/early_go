REM This batch file is executed in the project directory.

REM Prepare sqlite.dll compatible with platform target.
COMP /M sqlite3.dll sqlite3%1.dll > NUL
IF NOT %ERRORLEVEL% == 0 (
    ECHO Copy "sqlite3%1.dll" to "sqlite3.dll"
    COPY /Y sqlite3%1.dll sqlite3.dll > NUL
)

REM Copy the git hook script if it does not exist or it is different.
COMP /M tools\pre-commit ..\.git\hooks\pre-commit > NUL
IF NOT %ERRORLEVEL% == 0 (
    ECHO Copy "tools\pre-commit" to "..\.git\hooks\pre-commit"
    COPY /Y tools\pre-commit ..\.git\hooks\pre-commit > NUL
)

REM Copy dll

IF NOT EXIST "..\x64\Debug\opencv_imgcodecs320d.dll" (
    COPY /Y "..\packages\opencv.win.native.redist.320.1.1-vs141\build\native\bin\x64\v141\Debug\opencv_imgcodecs320d.dll" "..\x64\Debug\opencv_imgcodecs320d.dll"
)

IF NOT EXIST "..\x64\Debug\opencv_core320d.dll" (
    COPY /Y "..\packages\opencv.win.native.redist.320.1.1-vs141\build\native\bin\x64\v141\Debug\opencv_core320d.dll" "..\x64\Debug\opencv_core320d.dll"
)

IF NOT EXIST "..\x64\Debug\opencv_imgproc320d.dll" (
    COPY /Y "..\packages\opencv.win.native.redist.320.1.1-vs141\build\native\bin\x64\v141\Debug\opencv_imgproc320d.dll" "..\x64\Debug\opencv_imgproc320d.dll"
)


