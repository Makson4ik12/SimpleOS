extern "C" int kmain();
#include "win32_crt_math.cpp"

#define VIDEO_BUF_PTR (0xb8000)
#define PIC1_PORT (0x20)
#define IDT_TYPE_INTR (0x0E)
#define IDT_TYPE_TRAP (0x0F)
#define GDT_CS (0x8)
#define CURSOR_PORT (0x3D4)
#define VIDEO_WIDTH (80)

int tmp_text_color, text_color;
char buffer[100];
int pos = 0;
int line = 3;

char hello[] = "Welcome to SimpleOS!";
char g_test[] = "Input some cmd...";
char info1[] = "SimpleOS v1.0. Made by milky VVay";
char info2[] = "Assembler compiler - FASM, kernel compiler - Microsoft C Compiler.";

void clear(int mode);

#pragma pack(push, 1)
struct idt_entry
{
	unsigned short base_lo;
	unsigned short segm_sel;
	unsigned char always0;
	unsigned char flags;
	unsigned short base_hi;
};

struct idt_ptr
{
	unsigned short limit;
	unsigned int base;
};
#pragma pack(pop)

struct idt_entry g_idt[256];
struct idt_ptr g_idtp;

__declspec(naked) void startup()
{
	__asm {
		call kmain;
	}
}

__declspec(naked) void default_intr_handler()
{
	__asm {
		pusha
	}

	__asm {
		popa
		iretd
	}
}

typedef void (*intr_handler)();
void intr_reg_handler(int num, unsigned short segm_sel, unsigned short
	flags, intr_handler hndlr)
{
	unsigned int hndlr_addr = (unsigned int)hndlr;
	g_idt[num].base_lo = (unsigned short)(hndlr_addr & 0xFFFF);
	g_idt[num].segm_sel = segm_sel;
	g_idt[num].always0 = 0;
	g_idt[num].flags = flags;
	g_idt[num].base_hi = (unsigned short)(hndlr_addr >> 16);
}

void intr_start()
{
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
	g_idtp.base = (unsigned int)(&g_idt[0]);
	g_idtp.limit = (sizeof(struct idt_entry) * idt_count) - 1;
	__asm {
		lidt g_idtp
	}
}

void intr_enable()
{
	__asm sti;
}

void intr_disable()
{
	__asm cli;
}

__inline unsigned char inb(unsigned short port)
{
	unsigned char data;
	__asm {
		push dx
		mov dx, port
		in al, dx
		mov data, al
		pop dx
	}
	return data;
}

__inline void outb(unsigned short port, unsigned char data)
{
	__asm {
		push dx
		mov dx, port
		mov al, data
		out dx, al
		pop dx
	}
}

__inline void outw(unsigned short port, unsigned short data)
{
	__asm {
		push dx
		mov dx, port
		mov ax, data
		out dx, ax
		pop dx
	}
}

void cursor_moveto(unsigned int strnum, unsigned int pos)
{
	unsigned short new_pos = (strnum * VIDEO_WIDTH) + pos;
	outb(CURSOR_PORT, 0x0F);
	outb(CURSOR_PORT + 1, (unsigned char)(new_pos & 0xFF));
	outb(CURSOR_PORT, 0x0E);
	outb(CURSOR_PORT + 1, (unsigned char)((new_pos >> 8) & 0xFF));
}

void out_str(char* ptr, unsigned int strnum)
{
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	video_buf += 80 * 2 * strnum;

	while (*ptr)
	{
		video_buf[0] = (unsigned char)*ptr; // Символ (код)
		video_buf[1] = text_color; // Цвет символа
		video_buf += 2;
		ptr++;
	}

}

void clear_buffer() {
	while (pos) {
		buffer[pos--] = '\0';
	}

	line++;
	cursor_moveto(++line, pos);
}

int parse_scan_code(unsigned char scan_code) {
	if (scan_code == 2) buffer[pos] = '1';
	else if (scan_code == 3) buffer[pos] = '2';
	else if (scan_code == 4) buffer[pos] = '3';
	else if (scan_code == 5) buffer[pos] = '4';
	else if (scan_code == 6) buffer[pos] = '5';
	else if (scan_code == 7) buffer[pos] = '6';
	else if (scan_code == 8) buffer[pos] = '7';
	else if (scan_code == 9) buffer[pos] = '8';
	else if (scan_code == 10) buffer[pos] = '9';
	else if (scan_code == 11) buffer[pos] = '0';

	else if (scan_code == 16) buffer[pos] = 'q';
	else if (scan_code == 17) buffer[pos] = 'w';
	else if (scan_code == 18) buffer[pos] = 'e';
	else if (scan_code == 19) buffer[pos] = 'r';
	else if (scan_code == 20) buffer[pos] = 't';
	else if (scan_code == 21) buffer[pos] = 'y';
	else if (scan_code == 22) buffer[pos] = 'u';
	else if (scan_code == 23) buffer[pos] = 'i';
	else if (scan_code == 24) buffer[pos] = 'o';
	else if (scan_code == 25) buffer[pos] = 'p';

	else if (scan_code == 30) buffer[pos] = 'a';
	else if (scan_code == 31) buffer[pos] = 's';
	else if (scan_code == 32) buffer[pos] = 'd';
	else if (scan_code == 33) buffer[pos] = 'f';
	else if (scan_code == 34) buffer[pos] = 'g';
	else if (scan_code == 35) buffer[pos] = 'h';
	else if (scan_code == 36) buffer[pos] = 'j';
	else if (scan_code == 37) buffer[pos] = 'k';
	else if (scan_code == 38) buffer[pos] = 'l';

	else if (scan_code == 44) buffer[pos] = 'z';
	else if (scan_code == 45) buffer[pos] = 'x';
	else if (scan_code == 46) buffer[pos] = 'c';
	else if (scan_code == 47) buffer[pos] = 'v';
	else if (scan_code == 48) buffer[pos] = 'b';
	else if (scan_code == 49) buffer[pos] = 'n';
	else if (scan_code == 50) buffer[pos] = 'm';
	else if (scan_code == 57) buffer[pos] = ' ';

	else if (scan_code == 78) buffer[pos] = '+';
	else if (scan_code == 74) buffer[pos] = '-';
	else if (scan_code == 55) buffer[pos] = '*';
	else if (scan_code == 43) buffer[pos] = '/';
	else return 0;

	return 1;
}

void error(const char* err) {
	out_str((char *) err, ++line);
}

void info() {
	out_str(info1, ++line);
	out_str(info2, ++line);

	char info_gray[] = "Current color - gray";
	char info_white[] = "Current color - white";
	char info_yellow[] = "Current color - yellow";
	char info_blue[] = "Current color - blue";
	char info_red[] = "Current color - red";
	char info_green[] = "Current color - green";

	switch (tmp_text_color) {
	case 1:
		out_str(info_gray, ++line);
		break;
	case 2:
		out_str(info_white, ++line);
		break;
	case 3:
		out_str(info_yellow, ++line);
		break;
	case 4:
		out_str(info_blue, ++line);
		break;
	case 5:
		out_str(info_red, ++line);
		break;
	case 6:
		out_str(info_green, ++line);
		break;
	}
}

void clear(int mode) {
	char* video_buf = (char*)VIDEO_BUF_PTR;
	unsigned int i = 0;

	while (i < (80 * 25 * 2))
	{
		video_buf[i++] = ' ';
		video_buf[i++] = text_color;
	}

	if (mode) {
		line = 1;
		clear_buffer();

		out_str(hello, 0);
		out_str(g_test, 1);
		return;
	}

	line = 2;

	out_str(hello, 0);
	out_str(g_test, 1);
}

void shutdown() {
	outw(0x604, 0x2000);
}

int pow(int i, int degree) {
	int res = 1;

	while (degree--) {
		res *= i;
	}

	if (res < 0) error("Error: buffer overflow in pow");
	return res;
}

char* to_some_system(int i, int to) {
	char result[30];
	static char send[30];
	int pointer = 0;
	int tmp = 0;
	
	while (1) {
		if (pointer > 29) return (char*) "Error: buffer overflow";

		int div = i / to;
		int mod = i - (to * div);

		if (mod > 9) {
			result[pointer++] = mod + 87;
		}
		else {
			result[pointer++] = mod + 48;
		}

		if (div < to) {
			if (div > 9) {
				result[pointer++] = div + 87;
			}
			else {
				result[pointer++] = div + 48;
			}

			pointer--;
			break;
		}
		else {
			i = div;
		}
	}

	while (pointer >= 0) send[tmp++] = result[pointer--];
	send[tmp] = '\0';

	return send;
}

int to_decimal(char i[], int from, int end) {
	int degree = 0;
	int result = 0;

	while (end >= 0) {
		if ((i[end] >= 'a') && (i[end] <= 'z')) { // if character
			if ((i[end] - 87) >= from) return -1;
			result += (i[end] - 87) * pow(from, degree++);
			if (result < 0) return -2;
		}
		else if ((i[end] >= '0') && (i[end] <= '9')) { // if number
			if ((i[end] - '0') >= from) return -1;
			result += (i[end] - '0') * pow(from, degree++);
			if (result < 0) return -2;
		}
		else {
			return -1;
		}
		end--;
	}

	return result;
}

void nsconv() {
	char num[10]; int num_p = 0; int end = 0;
	char tmp[2];
	int from = 0;
	int to = 0;
	int ten = 1;
	pos = 7;

	while (buffer[pos] != ' ') {
		if (num_p > 9) {
			error("Error: buffer overflow, number too long");
			return;
		}

		num[num_p++] = buffer[pos++];
	}

	num[num_p] = '\0';
	end = num_p - 1;
	pos++;
	num_p = 0;

	while (buffer[pos] != ' ') {
		if (num_p > 1) {
			error("Error: buffer overflow, arg2 too long");
			return;
		}

		tmp[num_p++] = buffer[pos++];
	}

	num_p--;

	while (num_p >= 0) {
		from += (tmp[num_p--] - '0') * ten;
		ten *= 10;
	}

	pos++;
	num_p = 0;
	ten = 1;

	while (buffer[pos] != '\0') {
		if (num_p > 1) {
			error("Error: buffer overflow, arg3 too long");
			return;
		}

		tmp[num_p++] = buffer[pos++];
	}

	num_p--;

	while (num_p >= 0) {
		to += (tmp[num_p--] - '0') * ten;
		ten *= 10;
	}

	if (((from > 36) || (from < 2)) || ((to > 36) || (to < 2))) {
		error("Error: number system will be between 2 .. 36");
		return;
	}

	int decimal = to_decimal(num, from, end);

	if (decimal == -1) {
		error("Error: number contains uncorrect character");
		return;
	}
	else if (decimal == -2) {
		error("Error: buffer overflow, converted number too long");
		return;
	}

	out_str(to_some_system(decimal, to), ++line);
}

void posixtime() {
	int months[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	char num[11]; int num_p = 0;
	pos = 10;
	long a = 0;
	char date[50];
	int date_pointer = 0;

	while (buffer[pos] != '\0') {
		if (num_p > 9) {
			error("Error: number too long");
			return;
		}

		num[num_p++] = buffer[pos++];
	}

	if (num_p == 0) {
		error("Error: incorrect command, posixtime [timestamp]");
		return;
	}

	num[num_p--] = '\0';

	int ten = 1;

	while (num_p >= 0) {
		if ((num[num_p] < '0') || (num[num_p] > '9')) {
			error("Error: posixtime stamp has incorrect symbol");
			return;
		}

		a += (num[num_p--] - '0') * ten;
		ten *= 10;
	}

	if (a < 0) {
		error("Error: posixtime buffer overflow");
		return;
	}
	
	int year = a / 31536000 + 1970; // год
	int all_days = ((a / 86400) - ((year - 1969) / 4)) % 365; // кол-во дней в этом году
	int day = 0;
	int month = 0;

	if ((year % 4 == 0) || ((year % 100 == 0) && (year % 400 == 0))) months[1] = 29; // если високосный год

	while (1) {
		if ((all_days - months[month]) >= 0) {
			all_days -= months[month++];
		}
		else {
			day = all_days + 1;
			month++;
			break;
		}
	}

	long int h = (a / 3600) % 24;
	long int m = (a / 60) % 60;
	long int s = a % 60;
	
	date[date_pointer++] = (day / 10) + '0';
	date[date_pointer++] = (day % 10) + '0';
	date[date_pointer++] = '.';

	date[date_pointer++] = (month / 10) + '0';
	date[date_pointer++] = (month % 10) + '0';
	date[date_pointer++] = '.';

	date[date_pointer++] = (year / 1000) + '0';
	date[date_pointer++] = ((year % 1000) / 100) + '0';
	date[date_pointer++] = ((year % 100) / 10) + '0';
	date[date_pointer++] = (year % 10) + '0';
	date[date_pointer++] = ' ';

	date[date_pointer++] = (h / 10) + '0';
	date[date_pointer++] = (h % 10) + '0';
	date[date_pointer++] = '.';

	date[date_pointer++] = (m / 10) + '0';
	date[date_pointer++] = (m % 10) + '0';
	date[date_pointer++] = '.';

	date[date_pointer++] = (s / 10) + '0';
	date[date_pointer++] = (s % 10) + '0';
	date[date_pointer++] = '\0';

	out_str(date, ++line);
}

void wintime() {
	int months[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	char num[40]; int num_p = 0;
	pos = 8;
	long  mod = 0;
	char date[50];
	int date_pointer = 0;
	long long a = 1;

	while (buffer[pos] != '\0') {
		if (num_p > 18) {
			error("Error: number too long");
			return;
		}

		num[num_p++] = buffer[pos++];
	}

	if (num_p == 0) {
		error("Error: incorrect command, wintime [timestamp]");
		return;
	}

	num[num_p--] = '\0';

	long long ten = 1;
	long long ten2 = 1;
	int end = num_p;

	while (num_p >= 0) {
		if ((num[num_p] < '0') || (num[num_p] > '9')) {
			error("Error: wintime stamp has incorrect symbol");
			return;
		}

		if ((end - num_p) >= 7) {
			a += (num[num_p--] - '0') * ten;
			ten *= 10;
		}
		else {
			mod += (num[num_p--] - '0') * ten2;
			ten2 *= 10;
		}
	}

	a -= 1;

	int year = a / 31536000 + 1601; // год

	int tmp = 0;

	for (int i = 1601; ((i < 1970) && (i < year)); i++) {
		if (((i % 4 == 0) && (i % 100 != 0)) || ((i % 100 == 0) && (i % 400 == 0))) {
			tmp++;
		}
	}

	for (int i = 1970; i < year; i++) {
		if (i % 4 == 0) {
			tmp++;
		}
	}
	
	int all_days = ((a / 86400) - tmp) % 365; // кол-во дней в этом году
	int day = 0;
	int month = 0;

	if (year % 4 == 0) {
		months[1] = 29; // если високосный год
	}

	while (1) {
		if ((all_days - months[month]) >= 0) {
			all_days -= months[month++];
		}
		else {
			day = all_days + 1;
			month++;
			break;
		}
	}

	long int h = (a / 3600) % 24;
	long int m = (a / 60) % 60;
	long int s = a % 60;

	date[date_pointer++] = (day / 10) + '0';
	date[date_pointer++] = (day % 10) + '0';
	date[date_pointer++] = '.';

	date[date_pointer++] = (month / 10) + '0';
	date[date_pointer++] = (month % 10) + '0';
	date[date_pointer++] = '.';

	date[date_pointer++] = (year / 1000) + '0';
	date[date_pointer++] = ((year % 1000) / 100) + '0';
	date[date_pointer++] = ((year % 100) / 10) + '0';
	date[date_pointer++] = (year % 10) + '0';
	date[date_pointer++] = ' ';

	date[date_pointer++] = (h / 10) + '0';
	date[date_pointer++] = (h % 10) + '0';
	date[date_pointer++] = '.';

	date[date_pointer++] = (m / 10) + '0';
	date[date_pointer++] = (m % 10) + '0';
	date[date_pointer++] = '.';

	date[date_pointer++] = (s / 10) + '0';
	date[date_pointer++] = (s % 10) + '0';
	date[date_pointer++] = '\0';

	out_str(date, ++line);
}

void parse_cmd() {
	if (((buffer[0] == 'i') && (buffer[1] == 'n') && (buffer[2] == 'f') && (buffer[3] == 'o')) && (pos == 4)) {
		info();
	}
	else if (((buffer[0] == 'c') && (buffer[1] == 'l') && (buffer[2] == 'e') && (buffer[3] == 'a') && (buffer[4] == 'r')) && (pos == 5)) {
		clear(1);
		return;
	}
	else if (((buffer[0] == 's') && (buffer[1] == 'h') && (buffer[2] == 'u') && (buffer[3] == 't')
		&& (buffer[4] == 'd') && (buffer[5] == 'o') && (buffer[6] == 'w') && (buffer[7] == 'n')) && (pos == 8)) {
		shutdown();
	}
	else if (((buffer[0] == 'n') && (buffer[1] == 's') && (buffer[2] == 'c') && (buffer[3] == 'o')
		&& (buffer[4] == 'n') && (buffer[5] == 'v') && (buffer[6] == ' ')) && (pos > 6)) {
		nsconv();
	}
	else if (((buffer[0] == 'p') && (buffer[1] == 'o') && (buffer[2] == 's') && (buffer[3] == 'i')
		&& (buffer[4] == 'x') && (buffer[5] == 't') && (buffer[6] == 'i') && (buffer[7] == 'm') && (buffer[8] == 'e') && (buffer[9] == ' ')) && (pos > 9)) {
		posixtime();
	}
	else if (((buffer[0] == 'w') && (buffer[1] == 'i') && (buffer[2] == 'n') 
		&& (buffer[3] == 't') && (buffer[4] == 'i') && (buffer[5] == 'm') && (buffer[6] == 'e') && (buffer[7] == ' ')) && (pos > 7)) {
		wintime();
	}
	else {
		error("Error: unknown command");
	}

	if (line >= 24) {
		clear(0);
		parse_cmd();
	}
	else {
		clear_buffer();
	}
}

void keyb_process_keys()
{
	if (inb(0x64) & 0x01)
	{
		unsigned char scan_code;
		unsigned char state;
		scan_code = inb(0x60);

		if (scan_code < 128) {
			if (scan_code == 28) {
				if (pos == 0) return;
				buffer[pos] = '\0';
				if (line == 24) clear(0);
				parse_cmd();
				return;
			}
			else if (scan_code == 14) {
				unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
				if ((80 * 2 * line + pos * 2 - 2) < 80 * 2 * line) return;

				video_buf[80 * 2 * line + pos * 2 - 2] = ' ';
				video_buf[80 * 2 * line + pos * 2 - 1] = text_color;

				buffer[--pos] = ' ';
				cursor_moveto(line, pos);
				return;
			}

			if (pos > 48) {
				pos--;
				return;
			}

			if (!parse_scan_code(scan_code)) {
				return;
			}

			out_str(buffer, line);
			cursor_moveto(line, ++pos);
		}
	}
}

__declspec(naked) void keyb_handler()
{
	__asm pusha;
	keyb_process_keys();
	outb(PIC1_PORT, 0x20);
	__asm {
		popa
		iretd
	}
}

void keyb_init()
{
	intr_reg_handler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, keyb_handler);
	outb(PIC1_PORT + 1, 0xFF ^ 0x02);
}

void select_console_color() {
	switch (tmp_text_color) {
		case 1:
			text_color = 0x08;
			break;
		case 2:
			text_color = 0x0F;
			break;
		case 3:
			text_color = 0x0E;
			break;
		case 4:
			text_color = 0x09;
			break;
		case 5:
			text_color = 0x04;
			break;
		case 6:
			text_color = 0x02;
			break;
	}
}

extern "C" int kmain()
{
	_asm mov tmp_text_color, ecx;

	select_console_color();
	
	out_str(hello, 0);
	out_str(g_test, 1);
	
	cursor_moveto(line, 0);

	intr_start();
	intr_enable();
	keyb_init();

	while (1)
	{
		__asm hlt;
	}

	return 0;
}