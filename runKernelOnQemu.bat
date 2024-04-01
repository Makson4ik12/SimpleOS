@echo off
chcp 65001
cd qemu
qemu-system-x86_64 -fda bootsect.bin -fdb kernel.bin