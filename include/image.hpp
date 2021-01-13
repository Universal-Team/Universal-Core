#ifndef IMAGE_HPP
#define IMAGE_HPP

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
	 * @param path The path to load the image from
	 */
	Image(const std::vector<std::string> &paths);
	/**
	 * @brief Text printing class
	 * @param paths The paths to try load the image from, from highest to lowest priority
	 */
	Image(const std::string &path) : Image(std::vector<std::string>({path})) {};

	/**
	 * @brief Text printing class
	 * @param file The file to load the image from, seeked to the image magic
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
	 * @param top Whether to draw on teh top or bottom screen, not used for sprites
	 * @param layer (Optional) The layer to draw on, not used for sprites
	 * @param channel (Optional) The DMA channel to use
	 * @param copyPal (Optional) Whether to copy the image's palette into palette VRAM
	 */
	void draw(int x, int y, bool top, int layer = 3, int channel = 0, bool copyPal = true);

	/**
	 * @brief Draws the image to a background layer, slower but can skip alpha, scale, and offset the palette
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param top Whether to draw on the top or bottom screen
	 * @param layer (Optional) Which background layer to draw on
	 * @param scaleX (Optional) The scale for the X axis
	 * @param scaleY (Optional) The scale for the Y axis
	 * @param paletteOffset (Optional) How much to offset the palette by
	 * @param copyPal (Optional) Whether to copy the image's palette into palette VRAM
	 */
	void drawSpecial(int x, int y, bool top, int layer = 3, float scaleX = 1.0f, float scaleY = 1.0f, int paletteOffset = 0, bool copyPal = true);

	/**
	 * @brief Draws a segment of an image to a background layer, faster but overwrites alpha and no scaling or palette offsetting
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param imageX The X position in the image to draw from
	 * @param imageY The Y position in the image to draw from
	 * @param w The width to draw
	 * @param h The height to draw
	 * @param top Whether to draw on the top or bottom screen
	 * @param layer (Optional) Which background layer to draw on
	 * @param channel (Optional) The DMA channel to use
	 * @param copyPal (Optional) Whether to copy the image's palette into palette VRAM
	 */
	void drawSegment(int x, int y, int imageX, int imageY, int w, int h, bool top, int layer = 3, int channel = 0, bool copyPal = true);

	/**
	 * @brief Draws a segment of an image to a background layer, slower but can skip alpha, scale, and offset the palette
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param imageX The X position in the image to draw from
	 * @param imageY The Y position in the image to draw from
	 * @param w The width to draw
	 * @param h The height to draw
	 * @param top Whether to draw on the top or bottom screen
	 * @param layer (Optional) Which background layer to draw on
	 * @param scaleX (Optional) The scale for the X axis
	 * @param scaleY (Optional) The scale for the Y axis
	 * @param paletteOffset (Optional) How much to offset the palette by
	 * @param copyPal (Optional) Whether to copy the image's palette into palette VRAM
	 */
	void drawSegmentSpecial(int x, int y, int imageX, int imageY, int w, int h, bool top, int layer = 3, float scaleX = 1.0f, float scaleY = 1.0f, int paletteOffset = 0, bool copyPal = true);

};

#endif
