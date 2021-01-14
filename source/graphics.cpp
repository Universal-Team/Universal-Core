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

#include "graphics.hpp"

#include "tonccpy.h"

#include <nds.h>

int Graphics::bg3Main, Graphics::bg2Main, Graphics::bg3Sub, Graphics::bg2Sub;
bool Graphics::wideScreen = false;

void Graphics::init(void) {
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

void Graphics::clear(bool top, int layer) { toncset(bgGetGfxPtr(top ? layer : layer + 4), 0, 256 * 192); }

void Graphics::drawOutline(int x, int y, int w, int h, u8 color, bool top, int layer) {
	u8 *dst = (u8 *)bgGetGfxPtr(top ? layer : layer + 4);
	h += y;
	if(y >= 0 && y < 192)
		toncset(dst + ((y * 256) + (x < 0 ? 0 : x)), color, (x + w > 256 ? w + (256 - x - w) : w));
	for(y++; y < (h - 1); y++) {
		if(y >= 0 && y < 192 && x > 0)
			toncset(dst + ((y)*256 + x), color, 1);
		if(y >= 0 && y < 192 && x + w < 256)
			toncset(dst + ((y)*256 + x + w - 1), color, 1);
	}
	if(y >= 0 && y < 192)
		toncset(dst + ((y * 256) + (x < 0 ? 0 : x)), color, (x + w > 256 ? w + (256 - x - w) : w));
}

void Graphics::drawRectangle(int x, int y, int w, int h, u8 color, bool top, bool layer) {
	Graphics::drawRectangle(x, y, w, h, color, color, top, layer);
}
void Graphics::drawRectangle(int x, int y, int w, int h, u8 color1, u8 color2, bool top, bool layer) {
	u8 *dst = (u8 *)bgGetGfxPtr(top ? layer : layer + 4);
	for(int i = 0; i < h; i++) {
		toncset(dst + ((y + i) * 256 + x), ((i % 2) ? color1 : color2), w);
	}
}
