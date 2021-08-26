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

#ifndef _UNIVERSAL_CORE_GUI_HPP
#define _UNIVERSAL_CORE_GUI_HPP

#include "font.hpp"
#include "screen.hpp"
#include "sprite.hpp"
#include "spritesheet.hpp"

#include <nds/ndstypes.h>

#include "UNIVCORE_CONFIG.h"

#ifdef UNIVCORE_3DS_SIZE
#define SCALE_3DS(Pos) Pos = (Pos * 4 / 5)
#else
#define SCALE_3DS(Pos)
#endif

// TODO: Maybe change these, they need to be C2D's flags atm to be compatible with 3DS Universal-Core
enum {
	C2D_AtBaseline       = BIT(0), ///< Matches the Y coordinate with the baseline of the font.
	C2D_WithColor        = BIT(1), ///< Draws text with color. Requires a u32 color value.
	C2D_AlignLeft        = 0 << 2, ///< Draws text aligned to the left. This is the default.
	C2D_AlignRight       = 1 << 2, ///< Draws text aligned to the right.
	C2D_AlignCenter      = 2 << 2, ///< Draws text centered.
	C2D_AlignJustified   = 3 << 2, ///< Draws text justified. When C2D_WordWrap is not specified, right edge is x + scaleX*text->width. Otherwise, right edge is x + the width specified for those values.
	C2D_AlignMask        = 3 << 2, ///< Bitmask for alignment values.
	C2D_WordWrap         = BIT(4), ///< Draws text with wrapping of full words before specified width. Requires a float value, passed after color if C2D_WithColor is specified.
};

namespace Gui {
	/*
		Clear the Text Buffer.
	*/
	void clearTextBufs(void);

	/*
		Clear the screen.
	*/
	void clearScreen(bool top);

	/*
		Load an a spritesheet

		Path: Path to the spritesheet file. (TDX)
		sheet: Reference to the Spritesheet declaration.
	*/
	bool loadSheet(const char *Path, Spritesheet &image);

	/*
		Draw an image.
		image: The spritesheet which should be used.
		imgIndex: Index within the spritesheet.
		x: The X Position where the sprite should be drawn.
		y: The Y Position where the sprite should be drawn.
		ScaleX: The X-Scale for the sprite. (Optional!)
		ScaleY: The Y-Scale for the sprite. (Optional!)
	*/
	void DrawSprite(Spritesheet &sheet, size_t imgindex, int x, int y, float ScaleX = 1.0f, float ScaleY = 1.0f);

	/*
		Initialize the GUI.
		Call this when initializing.

		FontPath: The font to use.
	*/
	bool init(const char *FontPath = nullptr);

	/*
		Load a Font. (NFTR)

		fnt: The Font variable which should be initialized.
		Path: Path to the NFTR file.
		if you're unsure, just call 'Gui::init();' and it will load the default font.
	*/
	bool loadFont(Font &fnt, const char *Path = nullptr);

	/*
		Reinit the GUI.

		FontPath: The font to use.
	*/
	bool reinit(const char *FontPath = nullptr);

	/*
		Draws a centered String.
		x: The X Offset from center. (Center: 200 px on top, 160 px on Bottom.)
		y: The Y Position of the Text.
		size: The size of the Text.
		palette: The palette of the Text.
		Text: The Text which should be displayed.
		maxWidth: The maxWidth for the Text. (Optional!)
		maxHeight: The maxHeight of the Text. (Optional!)
		fnt: The Font which should be used. Uses default font by default. (Optional!)
		int flags: C2D text flags to use. (Optional!)
	*/
	void DrawStringCentered(int x, int y, float size, Palette palette, const std::string &Text, int maxWidth = 0, int maxHeight = 0, Font *fnt = nullptr, int flags = 0);

	/*
		Draws a String.

		x: The X Position where the Text should be drawn.
		y: The Y Position where the Text should be drawn.
		size: The size of the Text.
		palette: The palette of the Text.
		Text: The Text which should be displayed.
		maxWidth: The maxWidth for the Text. (Optional!)
		maxHeight: The maxHeight of the Text. (Optional!)
		fnt: The Font which should be used. Uses default font by default. (Optional!)
		flags: C2D text flags to use.
	*/
	void DrawString(int x, int y, float size, Palette palette, const std::string &Text, int maxWidth = 0, int maxHeight = 0, Font *fnt = nullptr, int flags = 0);

	/*
		Get the width of a String.

		size: The size of the Text.
		Text: The Text where the width should be getted from.
		fnt: The Font which should be used. Uses default font by default. (Optional!)
	*/
	int GetStringWidth(float size, const std::string &Text, Font *fnt = nullptr);

	/*
		Get the height of a String.

		size: The size of the Text.
		Text: The Text where the height should be getted from.
		fnt: The Font which should be used. Uses default font by default. (Optional!)
	*/
	int GetStringHeight(float size, const std::string &Text, Font *fnt = nullptr);


	/*
		Get the size of a String.

		size: The size of the Text.
		width: The width of the Text.
		height: The height of the Text.
		Text: The Text where the size should be getted from.
		fnt: The Font which should be used. Uses default font by default. (Optional!)
	*/
	void GetStringSize(float size, int *width, int *height, const std::string &Text, Font *fnt = nullptr);

	/*
		Draw a Rectangle.

		x: X Position of the Rectangle.
		y: Y Position of the Rectangle.
		w: The width of the rectangle.
		h: The height of the rectangle.
		color: The color of the rectangle.
	*/
	void Draw_Rect(int x, int y, int w, int h, u8 color);

	/*
		Used for the current Screen's Draw. (Optional!)
		stack: Is it the stack variant?
	*/
	void DrawScreen(bool stack = false);

	/*
		Used for the current Screen's Logic. (Optional!)

		hDown: the hidKeysDown() variable.
		hHeld: the HidKeysHeld() variable.
		touch: The TouchPosition variable.
		waitFade: Wheter to wait until the fade ends.
		stack: Is it the stack variant?
	*/
	void ScreenLogic(u32 hDown, u32 hHeld, touchPosition touch, bool waitFade = true, bool stack = false);

	/*
		Transfer the Temp Screen to the used one. (Optional!)

		stack: Is it the stack variant?
		It will check, if the tempScreen variable is not nullptr, so don't worry.
	*/
	void transferScreen(bool stack = false);

	/*
		Set a specific Screen with switch function. (Optional!)

		screen: unique_ptr of the screen. (Optional by using the screen class.)
		screenSwitch: Wheter to switch to the current screen.
		stack: Is it the stack variant?
	*/
	void setScreen(std::unique_ptr<Screen> screen, bool fade = false, bool stack = false);

	/*
		Fades into screens and calls the constructor after it. (Optional!)
		fadeoutFrames: Amount of frames for fadeout.
		fadeinFrames: Amount of frames for fadein.
		stack: Is it the stack variant?
	*/
	void fadeEffects(int fadeoutFrames = 6, int fadeinFrames = 6, bool stack = false);

	void screenBack(bool fade = false); // Goes a screen back. (Set!) (Stack only!)
	void screenBack2(); // Goes a screen back.(Action!) (Stack only!)

	/*
		Set on which screen to draw.

		top: Whether to draw on the top. (3DS Universal-Core compatible targets are inside the screenCommon.hpp file.)
	*/
	void ScreenDraw(bool top);

	/*
		Draws a grid.
		xPos: X Position of the grid.
		yPos: Y Position of the grid.
		Width: Width of the grid.
		Height: Height of the grid.
		color: Color of the grid.
		bgColor: The BG Color from the grid. (Optional! It's transparent by default.)
	*/
	void drawGrid(int xPos, int yPos, int Width, int Height, u8 color, u8 bgColor = 0);

	/*
		Draws an animated selector.
		xPos: X Position of the selector.
		yPos: Y Position of the Selector.
		Width: Width of the Selector.
		Height: Height of the Selector.
		speed: The speed of the animation. (Use .030f or something by default.)
		SelectorColor: The Color of the Selector outline.
		bgColor: The BG Color from the selector. (Optional! It's transparent by default.)
	*/
	void drawAnimatedSelector(int xPos, int yPos, int Width, int Height, int speed, u16 SelectorColor, u8 bgColor = 0);
}

#endif