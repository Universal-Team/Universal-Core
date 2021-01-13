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

#ifndef _UNIVERSAL_CORE_GRAPHICS_HPP
#define _UNIVERSAL_CORE_GRAPHICS_HPP

#include "font.hpp"
#include "image.hpp"
#include "sprite.hpp"

#include <nds/ndstypes.h>

namespace Graphics {
	extern int bg3Main, bg2Main, bg3Sub, bg2Sub;
	extern bool wideScreen;

	/**
	 * @brief Initializes the screens for drawing
	 */
	void init(void);

	/**
	 * @brief Clears the given layer
	 */
	void clear(bool top, int layer);

	/**
	* @brief Draws a rectangle outline of a given size at a given position
	* @param x The X position
	* @param y The Y position
	* @param w The Width
	* @param h The Height
	* @param color The index of the color to use
	* @param top Whether to draw to the top or bottom screen
	* @param layer The layer to draw to
	*/
	void drawOutline(int x, int y, int w, int h, u8 color, bool top, int layer);

	/**
	* @brief Draws a rectangle of a given size at a given position
	* @param x The X position
	* @param y The Y position
	* @param w The Width
	* @param h The Height
	* @param color The index of the color to use
	* @param top Whether to draw on the top or bottom screen
	* @param layer The layer to draw to
	*/
	void drawRectangle(int x, int y, int w, int h, u8 color, bool top, bool layer);

	/**
	* @brief Draws a rectangle of a given size at a given position
	* @param x The X position
	* @param y The Y position
	* @param w The Width
	* @param h The Height
	* @param color1 The index of the color to use for even rows
	* @param color2 The index of the color to use for odd rows
	* @param top Whether to draw on the top or bottom screen
	* @param layer The layer to draw to
	*/
	void drawRectangle(int x, int y, int w, int h, u8 color1, u8 color2, bool top, bool layer);
}

#endif