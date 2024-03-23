@echo off
@rem visual studioのビルドイベントは32bitモードで実行される。
@rem 一方でbatファイルをダブルクリックして起動した場合、64bitモードで起動する。
@rem sqliteのDLLが32bit版なので32bitモードで起動するように調節する。
if "%PROCESSOR_ARCHITECTURE%" NEQ "x86" (
    C:\Windows\SysWOW64\cmd.exe /C %0
) else (
    powershell -ExecutionPolicy RemoteSigned -File ./resource_importer.ps1
)