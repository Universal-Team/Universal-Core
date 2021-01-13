#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

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