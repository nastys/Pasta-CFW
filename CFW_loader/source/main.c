#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "fs.h"
#include "hid.h"
#include "textmenu.h"
#include "platform.h"
#include "draw.h"

//VARIABLES
char* systemVersion;
char type;
void ioDelay(u32);
bool dump_arm9;
int menu_idx=0;
#define MENU_ITEMS 5
#define SETTINGS_ITEMS 1

void ClearTop() {
	ClearScreen(TOP_SCREEN0, RGB(0, 0, 0));
	ClearScreen(TOP_SCREEN1, RGB(0, 0, 0));
	ClearScreen(BOT_SCREEN0, RGB(0, 0, 0));
	ClearScreen(BOT_SCREEN1, RGB(0, 0, 0));
	current_y = 0;
}

void wait_key() {
	Debug("");
	Debug("Press key to continue");
	InputWait();
}

void getSystemVersion()
{
	char settings[16];
	if (FileOpen("/3ds/PastaCFW/system.txt")){
		FileRead(settings, 16, 0);
		FileClose();
	}

	switch (settings[0])
	{
	case '1': // 4.x
		type = '1';
		systemVersion = "Old 3DS V. 4.1 - 4.5";
		break;
	case '2': // 5.0
		type = '2';
		systemVersion = "Old 3DS V. 5.0";
		break;
	case '3': // 5.1
		type = '3';
		systemVersion = "Old 3DS V. 5.1";
		break;
	case '4': // 6.0
		type = '4';
		systemVersion = "Old 3DS V. 6.0";
		break;
	case '5': // 6.1 - 6.3
		type = '5';
		systemVersion = "Old 3DS V. 6.1 - 6.3";
		break;
	case '6': // 7.0-7.1
		type = '6';
		systemVersion = "Old 3DS V. 7.0 - 7.1";
		break;
	case '7': // 7.2
		type = '7';
		systemVersion = "Old 3DS V. 7.2";
		break;
	case '8': // 8.x
		type = '8';
		systemVersion = "Old 3DS V. 8.0 - 8.1";
		break;
	case '9': // 9.x
		type = '9';
		systemVersion = "Old 3DS V. 9.0 - 9.2";
		break;
	case 'a': // 8.x
		type = 'a';
		systemVersion = "New 3DS V. 8.1";
		break;
	case 'b': // 9.x
		type = 'b';
		systemVersion = "New 3DS V. 9.0 - 9.2";
		break;
	}

	//Check if to use the ARM9 Dumper
	if (settings[2] == '1') dump_arm9 = true;
}

void bootCFW_SecondStage()
{
	u8 patch[] = { 0x00, 0x20, 0x3B, 0xE0 };
	u8 patch1[] = { 0x00, 0x20, 0x08, 0xE0 };
	u8 patch2[] = { 0x01, 0x27, 0x1E, 0xF5 };
	u8 patch3[] = { 0xB4, 0xF9, 0xD0, 0xAB };
	u8 patch4[] = { 0x6D, 0x20, 0xCE, 0x77 };
	u8 patch5[] = { 0x5A, 0xC5, 0x73, 0xC1 };
	//Apply patches
	DebugNoNewLine("Apply patch for type %c...", type);
	if (type == '1'){ // 4.x
		u32 *dest = 0x080549C4;
		u32 *dest1 = 0x0804239C;
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '2'){ // 5.0
		u32 *dest = 0x08051650;
		u32 *dest1 = 0x0803C838;
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '3'){ // 5.1
		u32 *dest = 0x0805164C;
		u32 *dest1 = 0x0803C838;
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '4'){ // 6.0
		u32 *dest = 0x0805235C;
		u32 *dest1 = 0x08057FE4;
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '5'){ // 6.1 - 6.3
		u32 *dest = 0x08051B5C;
		u32 *dest1 = 0x08057FE4;
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '6'){ // 7.0-7.1
		u32 *dest = 0x080521C4;
		u32 *dest1 = 0x08057E98;
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '7'){ // 7.2
		u32 *dest = 0x080521C8;
		u32 *dest1 = 0x08057E9C;
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '8'){ // 8.x
		u32 *dest = 0x080523C4;
		u32 *dest1 = 0x08058098;
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == '9'){ // 9.x
		u32 *dest = 0x0805235C;
		u32 *dest1 = 0x08058100;
		memcpy(dest, patch, 4);
		memcpy(dest1, patch1, 4);
	}
	else if (type == 'a'){ // 8.x
		u32 *dest = 0x08053114;
		u32 *dest1 = 0x080587E0;
		memcpy(dest, patch2, 4);
		memcpy(dest1, patch3, 4);
	}
	else if (type == 'b'){ // 9.x
		u32 *dest = 0x08052FD8;
		u32 *dest1 = 0x08058804;
		memcpy(dest, patch4, 4);
		memcpy(dest1, patch5, 4);
	}
	Debug("Apply patch for type %c...                  Done!", type);
}

void arm9dumper()
{
	int settings_idx = 0;
	while (true)
	{
		//DRAW GUI
		drawTop("/3ds/PastaCFW/appTOP.bin");
		drawBottom("/3ds/PastaCFW/dumper0.bin");
		//APP CONTROLS
		u32 pad_state = InputWait();
		if (pad_state & BUTTON_A) //CONTINUE TO DUMPER
		{
			drawBottom("/3ds/PastaCFW/dumper1.bin"); //draw UI
			u32 written = 0;
			u32 total = 0;
			u32 result = 0;
			u32 num = 0;
			void *addr = 0x08000000;
			u32 size = 0x00100000;
			const u32 sz_chunk = 0x10000;

			if (FileCreate("/3ds/PastaCFW/RAM.bin", true)) {
				while (total < size) {
					num = size - total < sz_chunk ? size - total : sz_chunk;
					written = FileWrite((u8 *)addr + total, num, total);
					if (written != num)  break;
					total += written;
					DebugNoNewLine("Dumping:                         %07d/%d", total, size);
				}
				FileClose();
				result = (size == total);
			}

			//draw last screen
			if (result) drawBottom("/3ds/PastaCFW/dumper2OK.bin"); //DUMPED SUCCESSFULLY
			else  drawBottom("/3ds/PastaCFW/dumper2E.bin"); //DUMP ERROR

			while (true)//Press A to continue
			{
				pad_state = InputWait();
				if (pad_state == BUTTON_A) break;
			}		
		}
		else if (pad_state & BUTTON_B) break; //EXIT BACK TO MENU 
		break;
	}
}

void bootCFW()
{
	//BOOT
	ClearTop();
	Debug("--------------- PASTA CFW LOADER ---------------");
	Debug("");
	DebugNoNewLine("Getting system information...");
	getSystemVersion();
	Debug("Getting system information...              Done!");
	Debug("");
	Debug("Your system is %s", systemVersion);
	Debug("");
	bootCFW_SecondStage();
}

void settings()
{
	int settings_idx=0;
	while (true)
	{
		//DRAW GUI
		drawTop("/3ds/PastaCFW/appTOP.bin");
		drawBottom("/3ds/PastaCFW/options.bin");
        //APP CONTROLS
		u32 pad_state = InputWait();
		if (pad_state & BUTTON_DOWN && settings_idx != SETTINGS_ITEMS - 1) settings_idx++; //MOVE DOWN
		else if (pad_state & BUTTON_UP && settings_idx != 0) settings_idx--; //MOVE UP
		else if (pad_state & BUTTON_A)
		{
			//SAVE SETTINGS
			break;
		}
		else if (pad_state & BUTTON_B) break; //EXIT WITHOUT SAVING 
	}
}



int main() {	
	
	InitFS();
	ClearTop();
	//MENU
	while (true)
	{
		//DRAW GUI
		if (menu_idx == MENU_ITEMS - 1)
		{
			drawTop("/3ds/PastaCFW/creditsTOP.bin");
			drawBottom("/3ds/PastaCFW/creditsBOT.bin");
		}
		else
		{
			char path[] = "/3ds/PastaCFW/menu0.bin";
			path[18] = menu_idx + 48;
            drawBottom(path); //BOTTOM SCREEN
			drawTop("/3ds/PastaCFW/menuTOP.bin"); //TOP SCREEN
		}
		
		//MENU CONTROLS
		u32 pad_state = InputWait();
		if (pad_state & BUTTON_RIGHT && menu_idx != MENU_ITEMS - 1) menu_idx++; //MOVE RIGHT
		else if (pad_state & BUTTON_LEFT && menu_idx != 0)menu_idx--; //MOVE LEFT
		else if (pad_state & BUTTON_A)//SELECT
		{
			if (menu_idx == 0){bootCFW(); break;}//BOOT CFW
			else if (menu_idx == 1)break; //REBOOT
			else if (menu_idx == 2)arm9dumper(); //ARM9 RAM DUMPER
			else if (menu_idx == 3)settings(); //SETTINGS
		}
	}
	// return control to FIRM ARM9 code (performs firmlaunch)
	return 0;
}

