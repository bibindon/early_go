@echo off
@rem visual studio�̃r���h�C�x���g��32bit���[�h�Ŏ��s�����B
@rem �����bat�t�@�C�����_�u���N���b�N���ċN�������ꍇ�A64bit���[�h�ŋN������B
@rem sqlite��DLL��32bit�łȂ̂�32bit���[�h�ŋN������悤�ɒ��߂���B
if "%PROCESSOR_ARCHITECTURE%" NEQ "x86" (
    C:\Windows\SysWOW64\cmd.exe /C %0
) else (
    powershell -ExecutionPolicy RemoteSigned -File ./resource_importer.ps1
)