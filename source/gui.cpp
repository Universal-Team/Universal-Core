/*
 *   This file is part of Universal-Core
 *   Copyright (C) 2021 Universal-Team
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
 *       * Requiring preservation of specified reasonable legal notices or
 *         author attributions in that material or in the Appropriate Legal
 *         Notices displayed by works containing it.
 *       * Prohibiting misrepresentation of the origin of that material,
 *         or requiring that modified versions of such material be marked in
 *         reasonable ways as different from the original version.
 */

#include "gui.hpp"
#include "gui.hpp"

#include "tonccpy.h"

#include <nds.h>

int Gui::bg3Main, Gui::bg2Main, Gui::bg3Sub, Gui::bg2Sub;
bool Gui::widescreen = false;
bool Gui::top = false;

void Gui::init(void) {
	// Initialize video mode
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);

	// initialize all the VRAM banks
	vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_MAIN_SPRITE, VRAM_C_SUB_BG, VRAM_D_SUB_SPRITE);

	// Init oam with 1D mapping 128 byte boundary and no external palette support
	oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);

	// Init the backgrounds
	bg3Main = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	bgSetPriority(bg3Main, 3);

	bg2Main = bgInit(2, BgType_Bmp8, BgSize_B8_256x256, 3, 0);
	bgSetPriority(bg2Main, 2);

	bg3Sub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	bgSetPriority(bg3Sub, 3);

	bg2Sub = bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 3, 0);
	bgSetPriority(bg2Sub, 2);

	// Set main background as target for sprite transparency
	REG_BLDCNT     = 1 << 11;
	REG_BLDCNT_SUB = 1 << 11;
}

void Gui::clear(bool top) {
	toncset(bgGetGfxPtr(top ? bg3Main : bg3Sub), 0, 256 * 192);
}

void Gui::ScreenDraw(bool top) {
	Gui::top = top;
}

void Gui::Draw_Rect(int x, int y, int w, int h, u8 color) {
	SCALE_3DS(x);
	SCALE_3DS(y);
	SCALE_3DS(w);
	SCALE_3DS(h);

	u8 *dst = (u8 *)bgGetGfxPtr(top ? bg3Main : bg3Sub);
	for(int i = 0; i < h; i++) {
		toncset(dst + ((y + i) * 256 + x), color, w);
	}
}