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

#ifndef GUI_HPP
#define GUI_HPP

#include "screen.hpp"

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <stack>

namespace Gui
{
	// Clear Text Buffer.
	void clearTextBufs(void);

	// Draw a sprite from a sheet.
	void DrawSprite(C2D_SpriteSheet sheet, size_t imgindex, int x, int y, float ScaleX = 1, float ScaleY = 1);

	// Initialize the GUI with Citro2D & Citro3D and initialize the Textbuffer.
	Result init(void);

	// Load a Font. (bcfnt)
	Result loadFont(bool sysFont = true, const char * Path = "");

	// Unload a Font. (bcfnt)
	Result unloadFont();

	// Load a spritesheet.
	Result loadSheet(const char* Path, C2D_SpriteSheet &sheet);

	// Unload a spritesheet.
	Result unloadSheet(C2D_SpriteSheet &sheet);

	// Exit the GUI.
	void exit(void);
	
	// Draw a centered String.
	void DrawStringCentered(float x, float y, float size, u32 color, std::string Text, int maxWidth = 0, int maxHeight = 0);

	// Draw a String.
	void DrawString(float x, float y, float size, u32 color, std::string Text, int maxWidth = 0, int maxHeight = 0);

	// Get the width of a String.
	float GetStringWidth(float size, std::string Text);

	// Get the size of a String.
	void GetStringSize(float size, float *width, float *height, std::string Text);

	// Get the height of a String.
	float GetStringHeight(float size, std::string Text);

	// Draw a Rectangle.
	bool Draw_Rect(float x, float y, float w, float h, u32 color);
	
	// Mainloop the GUI / Screen part.
	void mainLoop(u32 hDown, u32 hHeld, touchPosition touch);

	// Set a specific Screen.
	void setScreen(std::unique_ptr<Screen> screen);

	// Go a Screen back.
	void screenBack();

	// Set on which screen to draw.
	void ScreenDraw(C3D_RenderTarget * screen);

	// Draw a grid.
	void drawGrid(float xPos, float yPos, float Width, float Height, u32 color);

	// Draw an animated selector.
	void drawAnimatedSelector(float xPos, float yPos, float Width, float Height, float speed, u32 SelectorColor, u32 colour);
}

#endif