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

#ifndef _UNIVERSAL_CORE_FONT_HPP
#define _UNIVERSAL_CORE_FONT_HPP

#include "sprite.hpp"

#include <nds/ndstypes.h>
#include <string>
#include <string_view>
#include <vector>

#include "UNIVCORE_CONFIG.h"

enum class Alignment {
	left,
	center,
	right,
};

enum class Palette : u8 {
#ifdef UNIVCORE_TEXT_PALETTES
	UNIVCORE_TEXT_PALETTES
#else
	white
#endif
};

#ifndef UNIVCORE_TEXT_DEFAULT_PALETTE
#define UNIVCORE_TEXT_DEFAULT_PALETTE white
#endif

class Font {
private:
#ifdef UNIVCORE_TEXT_BUFFERED
	static u8 textBuf[2][256 * 192];
#endif

	u8 tileWidth = 0, tileHeight = 0;
	u16 tileSize = 0;
	u16 questionMark = 0;
	std::vector<u8> fontTiles;
	std::vector<u8> fontWidths;
	std::vector<u16> fontMap;

	u16 charIndex(char16_t c);

	void print(std::u16string_view text, int x, int y, Alignment align, Palette palette, int maxWidth, float scaleX, float scaleY, bool rtl, Sprite *sprite);

public:
	/**
	 * @brief Converts a UTF-8 string to UCS-2
	 * @param text The string to convert
	 * @return The converted string
	 */
	static std::u16string utf8to16(std::string_view text);

	/**
	 * @brief Text printing class
	 * @param paths The path to load the font from
	 */
	Font(const std::string &path) : Font(std::vector<std::string>({path})) {}
	/**
	 * @brief Text printing class
	 * @param paths The paths to try load the font from, from highest to lowest priority
	 */
	Font(const std::vector<std::string> &paths);

	Font() {}
	~Font() {}

	/**
	 * @brief The height of the font
	 * @return The font height
	 */
	u8 height(void) { return tileHeight; }

	/**
	 * @brief Calculates the width of a given string of text
	 * @param text The text to calculate the width of
	 * @return The width of the given string
	 */
	int calcWidth(std::string_view text) { return calcWidth(utf8to16(text)); }
	int calcWidth(std::u16string_view text);

	/**
	 * @brief Prints an integer value to a background layer
	 * @param x The X position to print at
	 * @param y The Y position to print at
	 * @param value The value to print
	 * @param align (Optional) The alignment to use
	 * @param palette (Optional) The palette to use
	 * @param maxWidth (Optional) The maximum width of the string, set to 0 for no max width
	 * @param scaleX (Optional) The scale on the X axis
	 * @param scaleY (Optional) The scale on the Y axis
	 */
	void print(int x, int y, int value, Alignment align = Alignment::left, Palette palette = Palette::UNIVCORE_TEXT_DEFAULT_PALETTE, int maxWidth = 0, float scaleX = 1.0f, float scaleY = 1.0f) { print(utf8to16(std::to_string(value)), x, y, align, palette, maxWidth, scaleX, scaleY, false, nullptr); }

	/**
	 * @brief Prints a string to a background layer
	 * @param x The X position to print at
	 * @param y The Y position to print at
	 * @param text The string to print
	 * @param align (Optional) The alignment to use
	 * @param palette (Optional) The palette to use
	 * @param maxWidth (Optional) The maximum width of the string, set to 0 for no max width
	 * @param scaleX (Optional) The scale on the X axis
	 * @param scaleY (Optional) The scale on the Y axis
	 */
	void print(int x, int y, std::string_view text, Alignment align = Alignment::left, Palette palette = Palette::UNIVCORE_TEXT_DEFAULT_PALETTE, int maxWidth = 0, float scaleX = 1.0f, float scaleY = 1.0f) { print(utf8to16(text), x, y, align, palette, maxWidth, scaleX, scaleY, false, nullptr); }
	void print(int x, int y, std::u16string_view text, Alignment align = Alignment::left, Palette palette = Palette::UNIVCORE_TEXT_DEFAULT_PALETTE, int maxWidth = 0, float scaleX = 1.0f, float scaleY = 1.0f) { print(text, x, y, align, palette, maxWidth, scaleX, scaleY, false, nullptr); }

	/**
	 * @brief Prints an integer value to a sprite
	 * @param x The X position to print at
	 * @param y The Y position to print at
	 * @param value The value to print
	 * @param sprite The sprite to print to
	 * @param align (Optional) The alignment to use
	 * @param palette (Optional) The palette to use
	 * @param maxWidth (Optional) The maximum width of the string, set to 0 for no max width
	 * @param scaleX (Optional) The scale on the X axis
	 * @param scaleY (Optional) The scale on the Y axis
	 */
	void print(int x, int y, int value, Sprite &sprite, Alignment align = Alignment::left, Palette palette = Palette::UNIVCORE_TEXT_DEFAULT_PALETTE, int maxWidth = 0, float scaleX = 1.0f, float scaleY = 1.0f) { print(utf8to16(std::to_string(value)), x, y, align, palette, maxWidth, scaleX, scaleY, false, nullptr); }

	/**
	 * @brief Prints a string to a sprite
	 * @param x The X position to print at
	 * @param y The Y position to print at
	 * @param text The string to print
	 * @param sprite The sprite to print to
	 * @param align (Optional) The alignment to use
	 * @param palette (Optional) The palette to use
	 * @param maxWidth (Optional) The maximum width of the string, set to 0 for no max width
	 * @param scaleX (Optional) The scale on the X axis
	 * @param scaleY (Optional) The scale on the Y axis
	 */
	void print(int x, int y, std::string_view text, Sprite &sprite, Alignment align = Alignment::left, Palette palette = Palette::UNIVCORE_TEXT_DEFAULT_PALETTE, int maxWidth = 0, float scaleX = 1.0f, float scaleY = 1.0f) { print(utf8to16(text), x, y, align, palette, maxWidth, scaleX, scaleY, false, &sprite); }
	void print(int x, int y, std::u16string_view text, Sprite &sprite, Alignment align = Alignment::left, Palette palette = Palette::UNIVCORE_TEXT_DEFAULT_PALETTE, int maxWidth = 0, float scaleX = 1.0f, float scaleY = 1.0f) { print(text, x, y, align, palette, maxWidth, scaleX, scaleY, false, &sprite); }

#ifdef UNIVCORE_TEXT_BUFFERED
	/**
	 * @brief Clears all text from both screens
	 */
	static void clear(void) { clear(true); clear(false); }
	/**
	 * @brief Clears all text from the given screen
	 * @param Whether to clear the top or bottom screen
	 */
	static void clear(bool top);

	/**
	 * @brief Copies all text on both screens from the buffer to actual screen
	 */
	static void update(void) { update(true); update(false); }
	/**
	 * @brief Copies all text on the given screen from the buffer to actual screen
	 * @param top Whether to update the top or bottom screen
	 */
	static void update(bool top);
#endif

};

#endif
