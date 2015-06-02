#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "fs.h"
#include "font.h"
#include "draw.h"

int current_y = 0;

void ClearScreen(unsigned char *screen, int color)
{
    int i;
    unsigned char *screenPos = screen;
    for (i = 0; i < (SCREEN_HEIGHT * SCREEN_WIDTH); i++) {
        *(screenPos++) = color >> 16;  // B
        *(screenPos++) = color >> 8;   // G
        *(screenPos++) = color & 0xFF; // R
    }
}

void DrawCharacter(unsigned char *screen, int character, int x, int y, int color, int bgcolor)
{
    int yy, xx;
    for (yy = 0; yy < 8; yy++) {
        int xDisplacement = (x * BYTES_PER_PIXEL * SCREEN_WIDTH);
        int yDisplacement = ((SCREEN_WIDTH - (y + yy) - 1) * BYTES_PER_PIXEL);
        unsigned char *screenPos = screen + xDisplacement + yDisplacement;

        unsigned char charPos = font[character * 8 + yy];
        for (xx = 7; xx >= 0; xx--) {
            if ((charPos >> xx) & 1) {
                *(screenPos + 0) = color >> 16;  // B
                *(screenPos + 1) = color >> 8;   // G
                *(screenPos + 2) = color & 0xFF; // R
            } else {
                *(screenPos + 0) = bgcolor >> 16;  // B
                *(screenPos + 1) = bgcolor >> 8;   // G
                *(screenPos + 2) = bgcolor & 0xFF; // R
            }
            screenPos += BYTES_PER_PIXEL * SCREEN_WIDTH;
        }
    }
}

void DrawString(unsigned char *screen, const char *str, int x, int y, int color, int bgcolor)
{
    int i;
    for (i = 0; i < strlen(str); i++)
        DrawCharacter(screen, str[i], x + i * 8, y, color, bgcolor);
}

void DrawStringF(int x, int y, const char *format, ...)
{
    char str[256];
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);

    DrawString(TOP_SCREEN0, str, x, y, RGB(0, 0, 0), RGB(255, 255, 255));
    DrawString(TOP_SCREEN1, str, x, y, RGB(0, 0, 0), RGB(255, 255, 255));
}

void Debug(const char *format, ...)
{
    char str[256];
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);

    DrawString(TOP_SCREEN0, str, 10, current_y, RGB(255, 255, 255), RGB(0,0, 0));
    DrawString(TOP_SCREEN1, str, 10, current_y, RGB(255, 255, 255), RGB(0, 0, 0));

    current_y += 10;
}

void DebugNoNewLine(const char *format, ...)
{
	char str[256];
	va_list va;

	va_start(va, format);
	vsnprintf(str, 256, format, va);
	va_end(va);

	DrawString(TOP_SCREEN0, str, 10, current_y, RGB(255, 255, 255), RGB(0, 0, 0));
	DrawString(TOP_SCREEN1, str, 10, current_y, RGB(255, 255, 255), RGB(0, 0, 0));
}

void drawTop(char splash_file[]) {
	unsigned int n = 0, bin_size;
	FileOpen(splash_file);
	//Load the spash image
	bin_size = 0;
	while ((n = FileRead((void*)((u32)TOP_SCREEN0 + bin_size), 0x100000, bin_size)) > 0) {
		bin_size += n;
	}
	u32 *fb1 = (u32*)TOP_SCREEN0;
	u32 *fb2 = (u32*)TOP_SCREEN1;
	for (n = 0; n < bin_size; n += 4){
		*fb2++ = *fb1++;
	}
}

void drawBottom(char splash_file[]) {
	unsigned int n = 0, bin_size;
	FileOpen(splash_file);
	//Load the spash image
	bin_size = 0;
	while ((n = FileRead((void*)((u32)BOT_SCREEN0 + bin_size), 0x100000, bin_size)) > 0) {
		bin_size += n;
	}
	u32 *fb1 = (u32*)BOT_SCREEN0;
	u32 *fb2 = (u32*)BOT_SCREEN1;
	for (n = 0; n < bin_size; n += 4){
		*fb2++ = *fb1++;
	}
}
