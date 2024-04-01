"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.33.31629\bin\Hostx86\x86\cl.exe" /GS- /c "kernel.cpp"
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.33.31629\bin\Hostx86\x86\link.exe" /OUT:kernel.bin /BASE:0x10000 /FIXED /FILEALIGN:512 /MERGE:.rdata=.data /IGNORE:4254 /NODEFAULTLIB /ENTRY:kmain /SUBSYSTEM:NATIVE "kernel.obj"
pause