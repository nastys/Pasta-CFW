#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "fs.h"
#include "hid.h"
#include "textmenu.h"
#include "platform.h"
#include "draw.h"

//This contains the System Firmware Version String.
char* cfw_FWString;
//This is the System Firmware Version in system.txt
char cfw_FWValue;
//if true, dump the ARM9 ram
bool cfw_arm9dump;

//variables needed for menu and settings gui
int menu_idx = 0;
#define MENU_ITEMS 5
#define SETTINGS_ITEMS 1

//[Unused]
//void ioDelay(u32);

// @breif  [Unused]Simply wait for a key, but not use its value.
// @note   Work like a pause.
void CFW_WaitKey(void) {
	DrawDebug(1,"");
	DrawDebug(1,"Press key to continue");
	HidWaitForInput();
}

// @breif  This reads the system version from the configuration file.
// @note   So please make sure you've have the file in your SD.
//         Also, if we use binary data to store the system version..
void CFW_getSystemVersion(void) {
	char settings[16];
	if (FSFileOpen("/3ds/PastaCFW/system.txt")){
		FSFileRead(settings, 16, 0);
		FSFileClose();
	}
    cfw_FWValue = settings[0];
	switch (settings[0]) {
    	case '1': // 4.x
    		cfw_FWString = platform_FWStrings[0];
    		break;
    	case '2': // 5.0
    		cfw_FWString = platform_FWStrings[1];
    		break;
    	case '3': // 5.1
    		cfw_FWString = platform_FWStrings[2];
    		break;
    	case '4': // 6.0
    		cfw_FWString = platform_FWStrings[3];
    		break;
    	case '5': // 6.1 - 6.3
    		cfw_FWString = platform_FWStrings[4];
    		break;
    	case '6': // 7.0-7.1
    		cfw_FWString = platform_FWStrings[5];
    		break;
    	case '7': // 7.2
    		cfw_FWString = platform_FWStrings[6];
    		break;
    	case '8': // 8.x
    		cfw_FWString = platform_FWStrings[7];
    		break;
    	case '9': // 9.x
    		cfw_FWString = platform_FWStrings[8];
    		break;
    	case 'a': // 8.x
    		cfw_FWString = platform_FWStrings[9];
    		break;
    	case 'b': // 9.x
    		cfw_FWString = platform_FWStrings[10];
    		break;
	}
	//Check if to use the ARM9 Dumper
	if (settings[2] == '1') cfw_arm9dump = true;
}

// @breif  Patch the offsets to pass the signs.
void CFW_SecondStage(void) {
    u8 patchO0[] = { 0x00, 0x20, 0x3B, 0xE0 };
	u8 patchO1[] = { 0x00, 0x20, 0x08, 0xE0 };
	u8 patchN0[] = { 0x01, 0x27, 0x1E, 0xF5 };
	u8 patchN1[] = { 0xB4, 0xF9, 0xD0, 0xAB };
	u8 patchN2[] = { 0x6D, 0x20, 0xCE, 0x77 };
	u8 patchN3[] = { 0x5A, 0xC5, 0x73, 0xC1 };
	//Apply patches
	DrawDebug(0,"Apply patch for type %c...", cfw_FWValue);
    switch(cfw_FWValue) {
        //Old-3DS
    	case '1': // 4.x
    		memcpy((u32*)0x080549C4, patchO0, 4);
    		memcpy((u32*)0x0804239C, patchO1, 4);
    		break;
    	case '2': // 5.0
    		memcpy((u32*)0x08051650, patchO0, 4);
    		memcpy((u32*)0x0803C838, patchO1, 4);
    		break;
    	case '3': // 5.1
    		memcpy((u32*)0x0805164C, patchO0, 4);
    		memcpy((u32*)0x0803C838, patchO1, 4);
    		break;
    	case '4': // 6.0
    		memcpy((u32*)0x0805235C, patchO0, 4);
    		memcpy((u32*)0x08057FE4, patchO1, 4);
    		break;
    	case '5': // 6.1 - 6.3
    		memcpy((u32*)0x08051B5C, patchO0, 4);
    		memcpy((u32*)0x08057FE4, patchO1, 4);
    		break;
    	case '6': // 7.0-7.1
    		memcpy((u32*)0x080521C4, patchO0, 4);
    		memcpy((u32*)0x08057E98, patchO1, 4);
    		break;
    	case '7': // 7.2
    		memcpy((u32*)0x080521C8, patchO0, 4);
    		memcpy((u32*)0x08057E9C, patchO1, 4);
    		break;
    	case '8': // 8.x
    		memcpy((u32*)0x080523C4, patchO0, 4);
    		memcpy((u32*)0x08058098, patchO1, 4);
    		break;
    	case '9': // 9.x
    		memcpy((u32*)0x0805235C, patchO0, 4);
    		memcpy((u32*)0x08058100, patchO1, 4);
    		break;
        //New-3DS
    	case 'a': // 8.x
    		memcpy((u32*)0x08053114, patchN0, 4);
    		memcpy((u32*)0x080587E0, patchN1, 4);
    		break;
    	case 'b': // 9.x
    		memcpy((u32*)0x08052FD8, patchN2, 4);
    		memcpy((u32*)0x08058804, patchN3, 4);
    		break;
    }
	DrawDebug(1,"Apply patch for type %c...                  Done!", cfw_FWValue);
}

// @breif  Dump ARM9 Ram to file.
void CFW_ARM9Dumper(void) {
	DrawClearScreenAll();
	DrawDebug(1,"");
	DrawDebug(1,"");
	DrawDebug(1,"");
	DrawDebug(1,"---------------- ARM9 RAM DUMPER ---------------");
	DrawDebug(1,"");
	DrawDebug(0,"Press A to DUMP, B to skip.");

	u32 pad_state = HidWaitForInput();
	if (pad_state & BUTTON_B) DrawDebug(1,"Skipping...");
	else {
		u32 bytesWritten = 0;
		u32 currentWritten = 0;
		u32 result = 0;
		u32 currentSize = 0;
		void *dumpAddr = (void*)0x08000000;
		u32 fullSize = 0x00100000;
		const u32 chunkSize = 0x10000;

		if (FSFileCreate("/3ds/PastaCFW/RAM.bin", true)) {
			while (currentWritten < fullSize) {
				currentSize = fullSize - currentWritten < chunkSize ? fullSize - currentWritten : chunkSize;
				bytesWritten = FSFileWrite((u8 *)dumpAddr + currentWritten, currentSize, currentWritten);
				if (bytesWritten != currentSize) break;
				currentWritten += bytesWritten;
				DrawDebug(0,"Dumping:                         %07d/%d", currentWritten, fullSize);
			}
			FSFileClose();
			result = (fullSize == currentWritten);
		}
		DrawDebug(1,"");
		DrawDebug(1,"");
		DrawDebug(1,"Dump %s! Press any key to boot CFW.", result ? "finished" : "failed");
		HidWaitForInput();
	}
}

void CFW_Settings(void)
{
	int settings_idx = 0;
	while (true)
	{
		//DRAW GUI
		DrawTopSplash("/3ds/PastaCFW/appTOP.bin");
		DrawBottomSplash("/3ds/PastaCFW/options.bin");
		//APP CONTROLS
		u32 pad_state = HidWaitForInput();
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

void CFW_Boot(void){
	//BOOT
	DrawClearScreenAll();
	DrawDebug(1, "--------------- PASTA CFW LOADER ---------------");
	DrawDebug(1, "");
	DrawDebug(0, "Getting system information...");
	CFW_getSystemVersion();
	DrawDebug(1, "Getting system information...              Done!");
	DrawDebug(1, "");
	DrawDebug(1, "Your system is %s", cfw_FWString);
	DrawDebug(1, "");
	CFW_SecondStage();
}

// @breif  Main routine surely.
int main(void) {

	FSInit();
	DrawClearScreenAll();

	DrawDebug(1, "--------------- PASTA CFW LOADER ---------------");
	HidWaitForInput();
	//MENU
	while (true)
	{
		//DRAW GUI
		if (menu_idx == MENU_ITEMS - 1)
		{
			DrawTopSplash("/3ds/PastaCFW/creditsTOP.bin");
			DrawBottomSplash("/3ds/PastaCFW/creditsBOT.bin");
		}
		else
		{
			char path[] = "/3ds/PastaCFW/menu0.bin";
			path[18] = menu_idx + 48;
			DrawBottomSplash(path); //BOTTOM SCREEN
			DrawTopSplash("/3ds/PastaCFW/menuTOP.bin"); //TOP SCREEN
		}

		//MENU CONTROLS
		u32 pad_state = HidWaitForInput();
		if (pad_state & BUTTON_RIGHT && menu_idx != MENU_ITEMS - 1) menu_idx++; //MOVE RIGHT
		else if (pad_state & BUTTON_LEFT && menu_idx != 0)menu_idx--; //MOVE LEFT
		else if (pad_state & BUTTON_A)//SELECT
		{
			if (menu_idx == 0){ CFW_Boot(); break; }//BOOT CFW
			else if (menu_idx == 1)break; //REBOOT
			else if (menu_idx == 2)CFW_ARM9Dumper(); //ARM9 RAM DUMPER
			else if (menu_idx == 3)CFW_Settings(); //SETTINGS
		}
	}

	// return control to FIRM ARM9 code (performs firmlaunch)
	return 0;
}
