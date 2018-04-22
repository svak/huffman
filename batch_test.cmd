@echo off

setlocal

for /f "tokens=* " %%f in ('dir /b') do if exist %%f (
    call :CheckFile "%CD%\%%f"
)
exit /b 0

:CheckFile
    set SRC=%1
    set ENCODED=%TEMP%\%RANDOM%
    set DECODED=%TEMP%\%RANDOM%

    encode.exe "%SRC%" "%ENCODED%"
    decode.exe "%ENCODED%" "%DECODED%"

    set ORIGMD5=%TEMP%\%RANDOM%
    set DECDMD5=%TEMP%\%RANDOM%

    md5sum "%SRC%" > "%ORIGMD5%"
    md5sum "%DECODED%" > "%DECDMD5%"

    for /f "tokens=1" %%v in (%ORIGMD5%) do set ORIG=%%v
    for /f "tokens=1" %%v in (%DECDMD5%) do set DECD=%%v

    if "%ORIG%"=="%DECD%" (
        echo.PASS %SRC%
    ) else (
        echo.FAIL %ORIG% %DECD% %SRC%
    )

    del /q %ENCODED%
    del /q %DECODED%
    del /q %ORIGMD5%
    del /q %DECDMD5%

    exit /b 0

