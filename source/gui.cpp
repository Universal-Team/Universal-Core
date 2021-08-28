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

#include "font.hpp"
#include "tonccpy.h"

#include <algorithm>
#include <nds.h>

#define ANIMATED_SELECTOR_COLOR 0xFF

std::unique_ptr<Font> DefaultFont;
std::unique_ptr<Screen> usedScreen, tempScreen; // tempScreen used for "fade" effects.
std::stack<std::unique_ptr<Screen>> screens;
bool currentScreen = false;
bool fadeout = false, fadein = false, fadeout2 = false, fadein2 = false;
int fadealpha = 0;
int fadecolor = 0;
bool widescreen = false;
int selectorTimer = 0;

bool Gui::init(const std::vector<std::string> &FontPaths) {
	// Initialize video mode
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);

	// initialize all the VRAM banks
	vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_MAIN_SPRITE, VRAM_C_SUB_BG, VRAM_D_SUB_SPRITE);

	// Init oam with 1D mapping 128 byte boundary and no external palette support
	oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);

	// Init the backgrounds
	bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	bgSetPriority(3, 3);

	bgInit(2, BgType_Bmp8, BgSize_B8_256x256, 3, 0);
	bgSetPriority(2, 2);

	bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	bgSetPriority(7, 3);

	bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 3, 0);
	bgSetPriority(6, 2);

	// Set main background as target for sprite transparency
	REG_BLDCNT     = 1 << 11;
	REG_BLDCNT_SUB = 1 << 11;

	// Load the default font
	DefaultFont = std::make_unique<Font>(FontPaths);

	return true;
}

void Gui::clearScreen(bool top) {
	toncset(bgGetGfxPtr(currentScreen ? 3 : 7), 0, 256 * 192);
}

void Gui::clearTextBufs(void) {
	DefaultFont->clear();
}

void Gui::DrawSprite(Spritesheet &sheet, size_t imgindex, int x, int y, float ScaleX, float ScaleY) {
	sheet[imgindex].draw(x, y, 0x20, ScaleX, ScaleY);
}

bool Gui::loadFont(Font &fnt, const std::vector<std::string> &Paths) {
	fnt = Font(Paths);

	return true;
}

bool Gui::loadSheet(const char *Path, Spritesheet &sheet) {
	if(Path && access(Path, F_OK) == 0) // Only load if found.
		sheet = Spritesheet(Path);

	return true;
}

bool Gui::reinit(const std::vector<std::string> &FontPaths) {
	return Gui::init(FontPaths);
}

void Gui::DrawStringCentered(int x, int y, float size, u8 color, const std::string &Text, int maxWidth, int maxHeight, Font *fnt, int flags) {
#ifdef UNIVCORE_3DS_SIZE
	Gui::DrawString(x, y, size, color, Text, maxWidth, maxHeight, fnt, flags | C2D_AlignCenter);
#else
	Gui::DrawString(x, y, size, color, Text, maxWidth, maxHeight, fnt, flags | C2D_AlignCenter);
#endif
}

void Gui::DrawString(int x, int y, float size, u8 color, const std::string &Text, int maxWidth, int maxHeight, Font *fnt, int flags) {
	float heightScale = maxHeight == 0 ? size : std::min(size, size * (maxHeight / Gui::GetStringHeight(size, Text, fnt)));
	float widthScale = maxWidth == 0 ? size : std::min(size, size * (maxWidth / Gui::GetStringWidth(size, Text, fnt)));

	// TODO: Wrapping and such
	if(fnt)
		fnt->print(x, y, Text, flags & C2D_AlignCenter ? Alignment::center : (flags & C2D_AlignRight ? Alignment::right : Alignment::left), color, maxWidth, widthScale, heightScale);
	else
		DefaultFont->print(x, y, Text, flags & C2D_AlignCenter ? Alignment::center : (flags & C2D_AlignRight ? Alignment::right : Alignment::left), color, maxWidth, widthScale, heightScale);
}

int Gui::GetStringWidth(float size, const std::string &Text, Font *fnt) {
	if(fnt)
		return fnt->calcWidth(Text) * size;
	else
		return DefaultFont->calcWidth(Text) * size;
}

int Gui::GetStringHeight(float size, const std::string &Text, Font *fnt) {
	const int lines = 1 + std::count(Text.begin(), Text.end(), '\n');

	if(fnt)
		return fnt->height() * lines * size;
	else
		return DefaultFont->height() * lines * size;
}

void Gui::GetStringSize(float size, int *width, int *height, const std::string &Text, Font *fnt) {
	if(width)
		*width = GetStringWidth(size, Text, fnt);

	if(height)
		*height = GetStringHeight(size, Text, fnt);
}

void Gui::Draw_Rect(int x, int y, int w, int h, u8 color) {
	SCALE_3DS(x);
	SCALE_3DS(y);
	SCALE_3DS(w);
	SCALE_3DS(h);

	u8 *dst = (u8 *)bgGetGfxPtr(currentScreen ? 3 : 7);
	for(int i = 0; i < h; i++) {
		toncset(dst + ((y + i) * 256 + x), color, w);
	}
}

void Gui::DrawScreen(bool stack) {
	if(!stack) {
		if(usedScreen)
			usedScreen->Draw();

	} else {
		if(!screens.empty())
			screens.top()->Draw();
	}
}

void Gui::ScreenLogic(u32 hDown, u32 hHeld, touchPosition touch, bool waitFade, bool stack) {
	if(waitFade) {
		if(!fadein && !fadeout && !fadein2 && !fadeout2) {
			if(!stack) {
				if(usedScreen)
					usedScreen->Logic(hDown, hHeld, touch);

			} else {
				if(!screens.empty())
					screens.top()->Logic(hDown, hHeld, touch);
			}
		}

	} else {
		if(!stack) {
			if(usedScreen)
				usedScreen->Logic(hDown, hHeld, touch);

		} else {
			if(!screens.empty())
				screens.top()->Logic(hDown, hHeld, touch);
		}
	}
}

void Gui::transferScreen(bool stack) {
	if(!stack) {
		if(tempScreen)
			usedScreen = std::move(tempScreen);

	} else {
		if(tempScreen)
			screens.push(std::move(tempScreen));
	}
}

void Gui::setScreen(std::unique_ptr<Screen> screen, bool fade, bool stack) {
	tempScreen = std::move(screen);

	/* Switch screen without fade. */
	if(!fade) {
		Gui::transferScreen(stack);

	} else {
		/* Fade, then switch. */
		fadeout = true;
	}
}

void Gui::fadeEffects(int fadeoutFrames, int fadeinFrames, bool stack) {
	// TODO!
	if(fadein) {
		fadealpha -= fadeinFrames;

		if(fadealpha < 0) {
			fadealpha = 0;
			fadecolor = 255;
			fadein = false;
		}
	}

	if(stack) {
		if(fadein2) {
			fadealpha -= fadeinFrames;

			if(fadealpha < 0) {
				fadealpha = 0;
				fadecolor = 255;
				fadein2 = false;
			}
		}
	}

	if(fadeout) {
		fadealpha += fadeoutFrames;

		if(fadealpha > 255) {
			fadealpha = 255;
			Gui::transferScreen(stack); // Transfer Temp screen to the stack / used one.
			fadein = true;
			fadeout = false;
		}
	}

	if(stack) {
		if(fadeout2) {
			fadealpha += fadeoutFrames;

			if(fadealpha > 255) {
				fadealpha = 255;
				Gui::screenBack2(); // Go screen back.
				fadein2 = true;
				fadeout2 = false;
			}
		}
	}
}

void Gui::screenBack(bool fade) {
	if(!fade) {
		if(screens.size() > 0)
			screens.pop();

	} else {
		if(screens.size() > 0)
			fadeout2 = true;
	}
}

void Gui::screenBack2() {
	if(screens.size() > 0)
		screens.pop();
}

void Gui::ScreenDraw(bool top) {
	currentScreen = top;
}

void Gui::drawGrid(int xPos, int yPos, int Width, int Height, u8 color, u8 bgColor) {
	constexpr int w = 1;

	/* BG Color for the Grid. (Transparent by default.) */
	Gui::Draw_Rect(xPos, yPos, Width, Height, bgColor);

	/* Grid part, Top. */
	Gui::Draw_Rect(xPos, yPos, Width, w, color);
	/* Left. */
	Gui::Draw_Rect(xPos, yPos + w, w, Height - 2 * w, color);
	/* Right. */
	Gui::Draw_Rect(xPos + Width - w, yPos + w, w, Height - 2 * w, color);
	/* Bottom. */
	Gui::Draw_Rect(xPos, yPos + Height - w, Width, w, color);
}

void Gui::drawAnimatedSelector(int xPos, int yPos, int Width, int Height, int speed, u16 SelectorColor, u8 bgColor) {
	constexpr int w = 2;
	int highlight_multiplier = (selectorTimer % 0x3F > 0x1F) ? selectorTimer % 0x1F : 0x1F - (selectorTimer % 0x1F);
	u8 r                     = SelectorColor & 0x1F;
	u8 g                     = (SelectorColor >> 5) & 0x1F;
	u8 b                     = (SelectorColor >> 10) & 0x1F;
	u16 color                = (r / highlight_multiplier) | (g / highlight_multiplier) << 5 | (b / highlight_multiplier) << 10 | BIT(15);

	BG_PALETTE[ANIMATED_SELECTOR_COLOR] = color;
	BG_PALETTE_SUB[ANIMATED_SELECTOR_COLOR] = color;

	/* BG Color for the Selector. */
	Gui::Draw_Rect(xPos, yPos, Width, Height, bgColor);

	/* Selector part, Top. */
	Gui::Draw_Rect(xPos, yPos, Width, w, ANIMATED_SELECTOR_COLOR);
	/* Left. */
	Gui::Draw_Rect(xPos, yPos + w, w, Height - 2 * w, ANIMATED_SELECTOR_COLOR);
	/* Right. */
	Gui::Draw_Rect(xPos + Width - w, yPos + w, w, Height - 2 * w, ANIMATED_SELECTOR_COLOR);
	/* Bottom. */
	Gui::Draw_Rect(xPos, yPos + Height - w, Width, w, ANIMATED_SELECTOR_COLOR);

	selectorTimer += speed;
}