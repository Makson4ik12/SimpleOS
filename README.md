# SimpleOS
A primitive OS that allows you to convert numbers from one number system to another, as well as convert POSIX and WINTIME stamps to a date

In addition, when booting the OS, you can choose the console color for the kernel

## Requirements
- Microsoft Visual Studio 2022 (cl.exe and link.exe)
- FASM translator
- qemu 

## Build
1. Compile bootsect.ASM using FASM
2. Compile kernel.cpp via compileKernel.bat (change cl.exe and link.exe paths to your)
3. Run SimpleOS via runKernelOnQemu (change path to qemu to your)
