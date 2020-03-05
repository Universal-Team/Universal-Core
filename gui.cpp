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

C3D_RenderTarget* Top;
C3D_RenderTarget* Bottom;

C2D_TextBuf TextBuf;
C2D_Font Font;
std::stack<std::unique_ptr<Screen>> screens;
bool currentScreen = false;

// Clear Text.
void Gui::clearTextBufs(void)
{
	C2D_TextBufClear(TextBuf);
}

// Draw a sprite from the sheet.
void Gui::DrawSprite(C2D_SpriteSheet sheet, size_t imgindex, int x, int y, float ScaleX, float ScaleY, u32 tintColor)
{
	// Tint sprite.
	C2D_ImageTint tint;
	C2D_SetImageTint(&tint, C2D_TopLeft, tintColor, 1);
	C2D_SetImageTint(&tint, C2D_TopRight, tintColor, 1);
	C2D_SetImageTint(&tint, C2D_BotLeft, tintColor, 1);
	C2D_SetImageTint(&tint, C2D_BotRight, tintColor, 1);
	C2D_Image img = C2D_SpriteSheetGetImage(sheet, imgindex);
	C2D_DrawImageAt(img, x, y, 0.5f, &tint, ScaleX, ScaleY);
}

// Initialize GUI.
Result Gui::init(void)
{
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	// Create Screen Targets.
	Top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	Bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	// Load Textbuffer.
	TextBuf = C2D_TextBufNew(4096);
	return 0;
}

// Load a Font.
Result Gui::loadFont(bool sysFont, const char* Path) {
	if (sysFont)	Font = C2D_FontLoadSystem(CFG_REGION_USA);
	else	Font = C2D_FontLoad(Path);
	return 0;
}

// Unload a Font.
Result Gui::unloadFont() {
	C2D_FontFree(Font);
	return 0;
}

// Load a Sheet.
Result Gui::loadSheet(const char* Path, C2D_SpriteSheet &sheet) {
	sheet	= C2D_SpriteSheetLoad(Path);
	return 0;
}

// Unload a Sheet.
Result Gui::unloadSheet(C2D_SpriteSheet &sheet) {
	C2D_SpriteSheetFree(sheet);
	return 0;
}

// Exit the GUI.
void Gui::exit(void)
{
	C2D_TextBufDelete(TextBuf);
	C2D_Fini();
	C3D_Fini();
}

void Gui::DrawStringCentered(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight) {
	float lineHeight, widthScale;
	lineHeight = Gui::GetStringHeight(size, " ");
	int line = 0;
	while(Text.find('\n') != Text.npos) {
		if(maxWidth == 0) {
			widthScale = Gui::GetStringWidth(size, Text.substr(0, Text.find('\n')));
		} else {
			widthScale = std::min((float)maxWidth, Gui::GetStringWidth(size, Text.substr(0, Text.find('\n'))));
		}
		Gui::DrawString((currentScreen ? 200 : 160)+x-(widthScale/2), y+(lineHeight*line), size, color, Text.substr(0, Text.find('\n')), maxWidth, maxHeight);
		Text = Text.substr(Text.find('\n')+1);
		line++;
	}
	if(maxWidth == 0) {
		widthScale = Gui::GetStringWidth(size, Text.substr(0, Text.find('\n')));
	} else {
		widthScale = std::min((float)maxWidth, Gui::GetStringWidth(size, Text.substr(0, Text.find('\n'))));
	}
	Gui::DrawString((currentScreen ? 200 : 160)+x-(widthScale/2), y+(lineHeight*line), size, color, Text.substr(0, Text.find('\n')), maxWidth, maxHeight);
}

// Draw String or Text.
void Gui::DrawString(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, Font, TextBuf, Text.c_str());
	C2D_TextOptimize(&c2d_text);

	float heightScale;
	if(maxHeight == 0) {
		heightScale = size;
	} else {
		heightScale = std::min(size, size*(maxHeight/Gui::GetStringHeight(size, Text)));
	}

	if(maxWidth == 0) {
		C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, size, heightScale, color);
	} else {
		C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, std::min(size, size*(maxWidth/Gui::GetStringWidth(size, Text))), heightScale, color);
	}
}


// Get String or Text Width.
float Gui::GetStringWidth(float size, std::string Text) {
	float width = 0;
	GetStringSize(size, &width, NULL, Text);
	return width;
}

// Get String or Text Size.
void Gui::GetStringSize(float size, float *width, float *height, std::string Text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, Font, TextBuf, Text.c_str());
	C2D_TextGetDimensions(&c2d_text, size, size, width, height);
}


// Get String or Text Height.
float Gui::GetStringHeight(float size, std::string Text) {
	float height = 0;
	GetStringSize(size, NULL, &height, Text.c_str());
	return height;
}

// Draw a Rectangle.
bool Gui::Draw_Rect(float x, float y, float w, float h, u32 color) {
	return C2D_DrawRectSolid(x, y, 0.5f, w, h, color);
}

// Mainloop the GUI.
void Gui::mainLoop(u32 hDown, u32 hHeld, touchPosition touch) {
	screens.top()->Draw();
	screens.top()->Logic(hDown, hHeld, touch);
}

// Set the current Screen.
void Gui::setScreen(std::unique_ptr<Screen> screen)
{
	screens.push(std::move(screen));
}

// Go a Screen back.
void Gui::screenBack()
{
	screens.pop();
}

// Select, on which Screen should be drawn.
void Gui::ScreenDraw(C3D_RenderTarget * screen)
{
	C2D_SceneBegin(screen);
	currentScreen = screen == Top ? 1 : 0;
}

void Gui::drawGrid(float xPos, float yPos, float Width, float Height, u32 color)
{
	static constexpr int w	= 1;
	// BG Color for the Grid. (Transparent.)
	C2D_DrawRectSolid(xPos, yPos, 0.5, Width, Height, C2D_Color32(0, 0, 0, 0));

	// Grid part.
	C2D_DrawRectSolid(xPos, yPos, 0.5, Width, w, color);	// top
	C2D_DrawRectSolid(xPos, yPos + w, 0.5, w, Height - 2 * w, color);	// left
	C2D_DrawRectSolid(xPos + Width - w, yPos + w, 0.5, w, Height - 2 * w, color); // right
	C2D_DrawRectSolid(xPos, yPos + Height - w, 0.5, Width, w, color);	// bottom
}

void Gui::drawAnimatedSelector(float xPos, float yPos, float Width, float Height, float speed, u32 SelectorColor, u32 colour)
{
	static constexpr int w     = 2;
	static float timer         = 0.0f;
	float highlight_multiplier = fmax(0.0, fabs(fmod(timer, 1.0) - 0.5) / 0.5);
	u8 r                       = SelectorColor & 0xFF;
	u8 g                       = (SelectorColor >> 8) & 0xFF;
	u8 b                       = (SelectorColor >> 16) & 0xFF;
	u32 color = C2D_Color32(r + (255 - r) * highlight_multiplier, g + (255 - g) * highlight_multiplier, b + (255 - b) * highlight_multiplier, 255);

	// BG Color for the Selector.
	C2D_DrawRectSolid(xPos, yPos, 0.5, Width, Height, colour);

	// Animated Selector part.
	C2D_DrawRectSolid(xPos, yPos, 0.5, Width, w, color);                      // top
	C2D_DrawRectSolid(xPos, yPos + w, 0.5, w, Height - 2 * w, color);          // left
	C2D_DrawRectSolid(xPos + Width - w, yPos + w, 0.5, w, Height - 2 * w, color); // right
	C2D_DrawRectSolid(xPos, yPos + Height - w, 0.5, Width, w, color);             // bottom

	timer += speed; // Speed of the animation. Example : .030f / .030
}