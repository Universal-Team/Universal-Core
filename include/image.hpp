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

#ifndef _UNIVERSAL_CORE_IMAGE_HPP
#define _UNIVERSAL_CORE_IMAGE_HPP

#include <nds/ndstypes.h>
#include <string>
#include <vector>

class Image {
private:
	u16 _width;
	u16 _height;
	std::vector<u8> _bitmap;
	std::vector<u16> _palette;
	u16 _palOfs;

public:
	/**
	 * Image drawing class
	 * @param paths The paths to try load the image from, from highest to lowest priority
	 */
	Image(const std::vector<std::string> &paths);
	/**
	 * @brief Text printing class
	 * @param path The path to load the image from
	 */
	Image(const std::string &path) : Image(std::vector<std::string>({path})) {};

	/**
	 * @brief Image drawing class
	 * @param file The file to load the image from, seeked to the '.GFX' magic
	 */
	Image(FILE *file);

	Image() {};
	~Image(void) {};

	u16 width(void) const { return _width; }
	u16 height(void) const { return _height; }
	std::vector<u8> bitmap(void) const { return _bitmap; }
	std::vector<u16> palette(void) const { return _palette; }
	u16 palOfs(void) const { return _palOfs; }

	/**
	 * @brief Draws the image to a background layer, faster but without alpha, scaling, or palette offsetting
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param copyPal (Optional) Whether to copy the image's palette into palette VRAM
	 */
	void draw(int x, int y, bool copyPal = true);

	/**
	 * @brief Draws the image to a background layer, slower but can skip alpha, scale, and offset the palette
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param scaleX (Optional) The scale for the X axis
	 * @param scaleY (Optional) The scale for the Y axis
	 * @param paletteOffset (Optional) How much to offset the palette by
	 * @param copyPal (Optional) Whether to copy the image's palette into palette VRAM
	 */
	void drawSpecial(int x, int y, float scaleX = 1.0f, float scaleY = 1.0f, int paletteOffset = 0, bool copyPal = true);

	/**
	 * @brief Draws a segment of an image to a background layer, faster but overwrites alpha and no scaling or palette offsetting
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param imageX The X position in the image to draw from
	 * @param imageY The Y position in the image to draw from
	 * @param w The width to draw
	 * @param h The height to draw
	 * @param copyPal (Optional) Whether to copy the image's palette into palette VRAM
	 */
	void drawSegment(int x, int y, int imageX, int imageY, int w, int h, bool copyPal = true);

	/**
	 * @brief Draws a segment of an image to a background layer, slower but can skip alpha, scale, and offset the palette
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param imageX The X position in the image to draw from
	 * @param imageY The Y position in the image to draw from
	 * @param w The width to draw
	 * @param h The height to draw
	 * @param scaleX (Optional) The scale for the X axis
	 * @param scaleY (Optional) The scale for the Y axis
	 * @param paletteOffset (Optional) How much to offset the palette by
	 * @param copyPal (Optional) Whether to copy the image's palette into palette VRAM
	 */
	void drawSegmentSpecial(int x, int y, int imageX, int imageY, int w, int h, float scaleX = 1.0f, float scaleY = 1.0f, int paletteOffset = 0, bool copyPal = true);

};

#endif
