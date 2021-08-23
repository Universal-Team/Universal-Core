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
#include "image.hpp"
#include "screen.hpp"
#include "sprite.hpp"

#include <nds/ndstypes.h>

#ifdef UNIVCORE_3DS_SIZE
#define SCALE_3DS(Pos) Pos = (Pos * 4 / 5)
#else
#define SCALE_3DS(Pos)
#endif

namespace Gui {
	extern int bg3Main, bg2Main, bg3Sub, bg2Sub;
	extern bool widescreen;
	extern bool top;

	/**
	 * @brief Initializes the screens for drawing
	 */
	void init(void);

	/**
	 * @brief Clears the given screen
	 * @param top The screen to clear
	 */
	void clear(bool top);

	/**
	 * @brief Changes which screen to draw to
	 * @param top The screen to draw to
	 */
	void ScreenDraw(bool top);

	/**
	* @brief Draws a rectangle of a given size at a given position
	* @param x The X position
	* @param y The Y position
	* @param w The Width
	* @param h The Height
	* @param color The index of the color to use
	*/
	void Draw_Rect(int x, int y, int w, int h, u8 color);
}

#endif