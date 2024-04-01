use16
org 0x7C00 ; Указание адреса загрузки кода

start:
        ; Инициализация адресов сегментов
        mov ax, cs ; Сохранение адреса сегмента кода в ax
        mov ds, ax ; Сохранение этого адреса как начало сегмента данных
        mov ss, ax ; И сегмента стека
        mov sp, start ; Сохранение адреса стека как адрес первой инструкции этого кода. Стек будет расти вверх и не перекроет код

        ; считывание заданного кол-ва секторов с диска в память
        mov ax, 0x1100 ; адрес .text
        mov es, ax ; [ES:BX] => адрес буфера для считывания данных: ES = segment, BX = offset
        mov bx, 0  ;
        mov ah, 2  ; функция для считывания секторов
        mov dl, 1  ; номер диска
        mov dh, 0  ; номер головки
        mov cl, 2  ; номер сектора
        mov ch, 0  ; номер цилиндра
        mov al, 21 ; количество секторов
        int 0x13 ; прерывание дискового ввода вывода
     
        mov ax, 0x1400 ; адрес .data
        mov es, ax
        mov bx, 0
        mov ah, 2
        mov dl, 1
        mov dh, 0
        mov cl, 22
        mov ch, 0
        mov al, 29
        int 0x13

        mov ecx, 1 ; счетчик выбранного цвета

        mov ax, 0002h ; очистка экрана - функция 02h прерывания 10h
        int 10h

        cld  ; Очищает флаг DF, который управляет поведением команд обработки строк.
             ; Если этот флаг установлен в 0, то строки обрабатываются в сторону увеличения адресов.

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

        mov     si, gray ; загружаем адрес константы цвета в si
        mov     ah, 0x0E
f_gray:
        lodsb ; выводим посимвольно цвет
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
        int 0x16 ; считываем нажатие клавиши в al
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
        cli ; Отключение прерываний

        lgdt [gdt_info] ; Загрузка размера и адреса таблицы дескрипторов

        ; Включение адресной линии А20
        in al, 0x92
        or al, 2
        out 0x92, al

        ; Установка бита PE регистра CR0 - процессор перейдет в защищенный режим
        mov eax, cr0
        or al, 1
        mov cr0, eax
        jmp 0x8:protected_mode ; "Дальний" переход для загрузки корректной информации в cs

gray db "gray", 0
white db "white", 0
yellow db "yellow", 0
blue db "blue", 0
red db "red", 0
green db "green", 0

use32
protected_mode:
        ; Загрузка селекторов сегментов для стека и данных в регистры
        mov ax, 0x10  ; Используется дескриптор с номером 2 в GDT
        mov es, ax
        mov ds, ax
        mov ss, ax

        call 0x11000 ; Передача управления загруженному ядру (0х11000 так как это физический адрес)

gdt:
        ; Нулевой дескриптор
        db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

        ; Сегмент кода: base=0, size=4Gb, P=1, DPL=0, S=1(user), Type=1(code), Access=00A, G=1, B=32bit
        db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00

        ; Сегмент данных: base=0, size=4Gb, P=1, DPL=0, S=1(user), Type=0(data), Access=0W0, G=1, B=32bit
        db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00

gdt_info:
        dw gdt_info - gdt ; Размер таблицы (2 байта)
        dw gdt, 0         ; 32-битный физический адрес таблицы

times (512 - ($ - start) - 2) db 0 ; Заполнение нулями до границы 512 - 2 текущей точки
db 0x55, 0xAA ; 2 необходимых байта, чтобы сектор считался загрузочным