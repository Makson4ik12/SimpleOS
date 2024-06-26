use16
org 0x7C00 ; �������� ������ �������� ����

start:
        ; ������������� ������� ���������
        mov ax, cs ; ���������� ������ �������� ���� � ax
        mov ds, ax ; ���������� ����� ������ ��� ������ �������� ������
        mov ss, ax ; � �������� �����
        mov sp, start ; ���������� ������ ����� ��� ����� ������ ���������� ����� ����. ���� ����� ����� ����� � �� ��������� ���

        ; ���������� ��������� ���-�� �������� � ����� � ������
        mov ax, 0x1100 ; ����� .text
        mov es, ax ; [ES:BX] => ����� ������ ��� ���������� ������: ES = segment, BX = offset
        mov bx, 0  ;
        mov ah, 2  ; ������� ��� ���������� ��������
        mov dl, 1  ; ����� �����
        mov dh, 0  ; ����� �������
        mov cl, 2  ; ����� �������
        mov ch, 0  ; ����� ��������
        mov al, 21 ; ���������� ��������
        int 0x13 ; ���������� ��������� ����� ������
     
        mov ax, 0x1400 ; ����� .data
        mov es, ax
        mov bx, 0
        mov ah, 2
        mov dl, 1
        mov dh, 0
        mov cl, 22
        mov ch, 0
        mov al, 29
        int 0x13

        mov ecx, 1 ; ������� ���������� �����

        mov ax, 0002h ; ������� ������ - ������� 02h ���������� 10h
        int 10h

        cld  ; ������� ���� DF, ������� ��������� ���������� ������ ��������� �����.
             ; ���� ���� ���� ���������� � 0, �� ������ �������������� � ������� ���������� �������.

print_colors:
         cmp ecx, 1
         jz put_gray
         cmp ecx, 2
         jz put_white
         cmp ecx, 3
         jz put_yellow
         cmp ecx, 4
         jz put_blue
         cmp ecx, 5
         jz put_red
         cmp ecx, 6
         jz put_green

put_gray:
        mov ax, 0002h
        int 10h

        mov     si, gray ; ��������� ����� ��������� ����� � si
        mov     ah, 0x0E
f_gray:
        lodsb ; ������� ����������� ����
        or      al, al
        jz      wait_key
        int     0x10
        jmp     f_gray


put_white:
        mov ax, 0002h
        int 10h

        mov     si, white
        mov     ah, 0x0E
f_white:
        lodsb
        or      al, al
        jz      wait_key
        int     0x10
        jmp     f_white


put_yellow:
        mov ax, 0002h
        int 10h

        mov     si, yellow
        mov     ah, 0x0E
f_yellow:
        lodsb
        or      al, al
        jz      wait_key
        int     0x10
        jmp     f_yellow


put_blue:
        mov ax, 0002h
        int 10h

        mov     si, blue
        mov     ah, 0x0E
f_blue:
        lodsb
        or      al, al
        jz      wait_key
        int     0x10
        jmp     f_blue


put_red:
        mov ax, 0002h
        int 10h

        mov     si, red
        mov     ah, 0x0E
f_red:
        lodsb
        or      al, al
        jz      wait_key
        int     0x10
        jmp     f_red


put_green:
        mov ax, 0002h
        int 10h

        mov     si, green
        mov     ah, 0x0E
f_green:
        lodsb
        or      al, al
        jz      wait_key
        int     0x10
        jmp     f_green


wait_key:
        mov ah, 0x00
        int 0x16 ; ��������� ������� ������� � al
        cmp al, 0x0D ; 'enter' key
        je load_to_protect_mode
        cmp al, 0x77 ; 'w' key
        je key_up
        cmp al, 0x73 ; 's' key
        je key_down
        jmp wait_key

key_up:
        inc ecx
        cmp ecx, 7

        jz first
        jmp print_colors

key_down:
        dec ecx
        cmp ecx, 0

        jz last
        jmp print_colors

last:
        mov ecx, 6
        jmp print_colors

first:
        mov ecx, 1
        jmp print_colors

load_to_protect_mode:
        cli ; ���������� ����������

        lgdt [gdt_info] ; �������� ������� � ������ ������� ������������

        ; ��������� �������� ����� �20
        in al, 0x92
        or al, 2
        out 0x92, al

        ; ��������� ���� PE �������� CR0 - ��������� �������� � ���������� �����
        mov eax, cr0
        or al, 1
        mov cr0, eax
        jmp 0x8:protected_mode ; "�������" ������� ��� �������� ���������� ���������� � cs

gray db "gray", 0
white db "white", 0
yellow db "yellow", 0
blue db "blue", 0
red db "red", 0
green db "green", 0

use32
protected_mode:
        ; �������� ���������� ��������� ��� ����� � ������ � ��������
        mov ax, 0x10  ; ������������ ���������� � ������� 2 � GDT
        mov es, ax
        mov ds, ax
        mov ss, ax

        call 0x11000 ; �������� ���������� ������������ ���� (0�11000 ��� ��� ��� ���������� �����)

gdt:
        ; ������� ����������
        db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

        ; ������� ����: base=0, size=4Gb, P=1, DPL=0, S=1(user), Type=1(code), Access=00A, G=1, B=32bit
        db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00

        ; ������� ������: base=0, size=4Gb, P=1, DPL=0, S=1(user), Type=0(data), Access=0W0, G=1, B=32bit
        db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00

gdt_info:
        dw gdt_info - gdt ; ������ ������� (2 �����)
        dw gdt, 0         ; 32-������ ���������� ����� �������

times (512 - ($ - start) - 2) db 0 ; ���������� ������ �� ������� 512 - 2 ������� �����
db 0x55, 0xAA ; 2 ����������� �����, ����� ������ �������� �����������