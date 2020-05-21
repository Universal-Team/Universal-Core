/*
*   This file is part of Universal-Core
*   Copyright (C) 2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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
#include "screenCommon.hpp"

#include <3ds.h>
#include <unistd.h>

C3D_RenderTarget* Top;
C3D_RenderTarget* TopRight;
C3D_RenderTarget* Bottom;

C2D_TextBuf TextBuf;
C2D_Font Font;
std::stack<std::unique_ptr<Screen>> screens;
bool currentScreen = false;

// Clear Text.
void Gui::clearTextBufs(void) { C2D_TextBufClear(TextBuf); }

// Draw a sprite from the sheet.
void Gui::DrawSprite(C2D_SpriteSheet sheet, size_t imgindex, int x, int y, float ScaleX, float ScaleY) {
	if (sheet != nullptr) {
		if (C2D_SpriteSheetCount(sheet) >= imgindex) {
			C2D_DrawImageAt(C2D_SpriteSheetGetImage(sheet, imgindex), x, y, 0.5f, nullptr, ScaleX, ScaleY);
		}
	}
}

// Initialize GUI.
Result Gui::init(void) {
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	// Create Screen Targets.
	Top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	TopRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
	Bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	// Load Textbuffer.
	TextBuf = C2D_TextBufNew(4096);
	Font = C2D_FontLoadSystem(CFG_REGION_USA); // Load System font.
	return 0;
}

// Load a Font.
Result Gui::loadFont(C2D_Font &fnt, const char* Path) {
	if (access(Path, F_OK) == 0)	fnt = C2D_FontLoad(Path); // Only load if found.
	return 0;
}

// Unload a Font.
Result Gui::unloadFont(C2D_Font &fnt) {
	if (fnt != nullptr) {
		C2D_FontFree(fnt); // Make sure to only unload if not nullptr.
	}
	return 0;
}

// Load a Sheet.
Result Gui::loadSheet(const char* Path, C2D_SpriteSheet &sheet) {
	if (access(Path, F_OK) == 0)	sheet = C2D_SpriteSheetLoad(Path); // Only load if found.
	return 0;
}

// Unload a Sheet.
Result Gui::unloadSheet(C2D_SpriteSheet &sheet) {
	if (sheet != nullptr)	C2D_SpriteSheetFree(sheet); // Make sure to only unload if not nullptr.
	return 0;
}

// Exit the GUI.
void Gui::exit(void) {
	C2D_TextBufDelete(TextBuf);
	C2D_Fini();
	C3D_Fini();
}

// Draw a Centered String.
void Gui::DrawStringCentered(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight, C2D_Font fnt) {
	float lineHeight, widthScale;

	// Check for the lineHeight.
	if (fnt != nullptr) {
		lineHeight = Gui::GetStringHeight(size, " ", fnt);
	} else {
		lineHeight = Gui::GetStringHeight(size, " ");
	}

	int line = 0;
	while(Text.find('\n') != Text.npos) {
		if (maxWidth == 0) {
			// Do the widthScale.
			if (fnt != nullptr) {
				widthScale = Gui::GetStringWidth(size, Text.substr(0, Text.find('\n')), fnt);
			} else {
				widthScale = Gui::GetStringWidth(size, Text.substr(0, Text.find('\n')));
			}

		} else {
			// Do the widthScale 2.
			if (fnt != nullptr) {
				widthScale = std::min((float)maxWidth, Gui::GetStringWidth(size, Text.substr(0, Text.find('\n')), fnt));
			} else {
				widthScale = std::min((float)maxWidth, Gui::GetStringWidth(size, Text.substr(0, Text.find('\n'))));
			}

		}

		if (fnt != nullptr) {
			Gui::DrawString((currentScreen ? 200 : 160)+x-(widthScale/2), y+(lineHeight*line), size, color, Text.substr(0, Text.find('\n')), maxWidth, maxHeight, fnt);
		} else {
			Gui::DrawString((currentScreen ? 200 : 160)+x-(widthScale/2), y+(lineHeight*line), size, color, Text.substr(0, Text.find('\n')), maxWidth, maxHeight);
		}

		Text = Text.substr(Text.find('\n')+1);
		line++;
	}

	if (maxWidth == 0) {
		// Do the next WidthScale.
		if (fnt != nullptr) {
			widthScale = Gui::GetStringWidth(size, Text.substr(0, Text.find('\n')), fnt);
		} else {
			widthScale = Gui::GetStringWidth(size, Text.substr(0, Text.find('\n')));
		}

	} else {
		// And again.
		if (fnt != nullptr) {
			widthScale = std::min((float)maxWidth, Gui::GetStringWidth(size, Text.substr(0, Text.find('\n')), fnt));
		} else {
			widthScale = std::min((float)maxWidth, Gui::GetStringWidth(size, Text.substr(0, Text.find('\n'))));
		}

	}
	if (fnt != nullptr) {
		Gui::DrawString((currentScreen ? 200 : 160)+x-(widthScale/2), y+(lineHeight*line), size, color, Text.substr(0, Text.find('\n')), maxWidth, maxHeight, fnt);
	} else {
		Gui::DrawString((currentScreen ? 200 : 160)+x-(widthScale/2), y+(lineHeight*line), size, color, Text.substr(0, Text.find('\n')), maxWidth, maxHeight);
	}
}

// Draw String or Text.
void Gui::DrawString(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight, C2D_Font fnt) {
	C2D_Text c2d_text;

	if (fnt != nullptr) {
		C2D_TextFontParse(&c2d_text, fnt, TextBuf, Text.c_str());
	} else {
		C2D_TextFontParse(&c2d_text, Font, TextBuf, Text.c_str());
	}

	C2D_TextOptimize(&c2d_text);

	float heightScale;
	if (maxHeight == 0) {
		heightScale = size;
	} else {
		
		if (fnt != nullptr) {
			heightScale = std::min(size, size*(maxHeight/Gui::GetStringHeight(size, Text, fnt)));
		} else {
			heightScale = std::min(size, size*(maxHeight/Gui::GetStringHeight(size, Text)));
		}
	}

	if (maxWidth == 0) {
		C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, size, heightScale, color);
	} else {
		if (fnt != nullptr) {
			C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, std::min(size, size*(maxWidth/Gui::GetStringWidth(size, Text, fnt))), heightScale, color);
		} else {
			C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, std::min(size, size*(maxWidth/Gui::GetStringWidth(size, Text))), heightScale, color);
		}
	}
}

// Get String or Text Width.
float Gui::GetStringWidth(float size, std::string Text, C2D_Font fnt) {
	float width = 0;
	if (fnt != nullptr) {
		GetStringSize(size, &width, NULL, Text, fnt);
	} else {
		GetStringSize(size, &width, NULL, Text);
	}
	return width;
}

// Get String or Text Size.
void Gui::GetStringSize(float size, float *width, float *height, std::string Text, C2D_Font fnt) {
	C2D_Text c2d_text;
	if (fnt != nullptr) {
		C2D_TextFontParse(&c2d_text, fnt, TextBuf, Text.c_str());
	} else {
		C2D_TextFontParse(&c2d_text, Font, TextBuf, Text.c_str());
	}
	C2D_TextGetDimensions(&c2d_text, size, size, width, height);
}


// Get String or Text Height.
float Gui::GetStringHeight(float size, std::string Text, C2D_Font fnt) {
	float height = 0;
	if (fnt != nullptr) {
		GetStringSize(size, NULL, &height, Text.c_str(), fnt);
	} else {
		GetStringSize(size, NULL, &height, Text.c_str());
	}
	return height;
}

// Draw a Rectangle.
bool Gui::Draw_Rect(float x, float y, float w, float h, u32 color) {
	return C2D_DrawRectSolid(x, y, 0.5f, w, h, color);
}

// Mainloop the GUI.
void Gui::mainLoop(u32 hDown, u32 hHeld, touchPosition touch) {
	if (!screens.empty()) {
		screens.top()->Draw();
		screens.top()->Logic(hDown, hHeld, touch);
	}
}

// Set the current Screen.
void Gui::setScreen(std::unique_ptr<Screen> screen) { screens.push(std::move(screen)); }

// Go a Screen back.
void Gui::screenBack() { if (screens.size() > 0)	screens.pop(); }

// Select, on which Screen should be drawn.
void Gui::ScreenDraw(C3D_RenderTarget * screen) {
	C2D_SceneBegin(screen);
	currentScreen = (screen==Top || screen==TopRight) ? 1 : 0;
}

void Gui::drawGrid(float xPos, float yPos, float Width, float Height, u32 color) {
	static constexpr int w	= 1;
	// BG Color for the Grid. (Transparent.)
	C2D_DrawRectSolid(xPos, yPos, 0.5, Width, Height, C2D_Color32(0, 0, 0, 0));

	// Grid part.
	C2D_DrawRectSolid(xPos, yPos, 0.5, Width, w, color);	// top
	C2D_DrawRectSolid(xPos, yPos + w, 0.5, w, Height - 2 * w, color);	// left
	C2D_DrawRectSolid(xPos + Width - w, yPos + w, 0.5, w, Height - 2 * w, color); // right
	C2D_DrawRectSolid(xPos, yPos + Height - w, 0.5, Width, w, color);	// bottom
}

void Gui::drawAnimatedSelector(float xPos, float yPos, float Width, float Height, float speed, u32 SelectorColor, u32 bgColor) {
	static constexpr int w		= 2;
	static float timer			= 0.0f;
	float highlight_multiplier  = fmax(0.0, fabs(fmod(timer, 1.0) - 0.5) / 0.5);
	u8 r						= SelectorColor & 0xFF;
	u8 g						= (SelectorColor >> 8) & 0xFF;
	u8 b						= (SelectorColor >> 16) & 0xFF;
	u32 color 					= C2D_Color32(r + (255 - r) * highlight_multiplier, g + (255 - g) * highlight_multiplier, b + (255 - b) * highlight_multiplier, 255);

	// BG Color for the Selector.
	C2D_DrawRectSolid(xPos, yPos, 0.5, Width, Height, bgColor);

	// Animated Selector part.
	C2D_DrawRectSolid(xPos, yPos, 0.5, Width, w, color); // top
	C2D_DrawRectSolid(xPos, yPos + w, 0.5, w, Height - 2 * w, color); // left
	C2D_DrawRectSolid(xPos + Width - w, yPos + w, 0.5, w, Height - 2 * w, color); // right
	C2D_DrawRectSolid(xPos, yPos + Height - w, 0.5, Width, w, color); // bottom

	timer += speed; // Speed of the animation. Example : .030f / .030
}