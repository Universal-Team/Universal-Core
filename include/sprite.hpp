#ifndef _UNIVERSAL_CORE_SPRITE_HPP
#define _UNIVERSAL_CORE_SPRITE_HPP

#include "image.hpp"

#include <nds.h>

class Sprite {
private:
	bool _top;
	OamState *_oam;
	SpriteSize _size;
	SpriteColorFormat _format;
	int _x, _y, _priority, _id, _rotationIndex, _paletteAlpha, _width, _height, _rotation;
	float _scaleX, _scaleY;
	bool _visibility;
	u16 *_gfx;

	static u8 _assigned[2][128];

public:
	/**
	 * @brief A class for easier management of OAM sprites
	 */
	Sprite() {}

	/**
	 * @brief A class for easier management of OAM sprites
	 * @param top Whether to make it on the top or bottom screen
	 * @param size The size of the sprite
	 * @param format The color format of the sprite
	 * @param (Optional) x The starting X position of the sprite
	 * @param (Optional) y The starting Y position of the sprite
	 * @param (Optional) priority The starting priority of the sprite, 0 - 3 with 0 on top
	 * @param (Optional) id The ID to use for the sprite, set to -1 to automatically use the first free one
	 * @param (Optional) paletteAlpha For paletted sprites, the pallete to use. For bitmap sprites, the alpha
	 * @param (Optional) rotationIndex The rotation index to use, set to -1 for no rotation
	 * @param (Optional) doubleSize Whether to double the size for rotation
	 * @param (Optional) visible Whether the sprite should start visible
	 * @param (Optional) vFlip Whether to start the sprite flipped vertically
	 * @param (Optional) hFlip Whether to start the sprite flipped horizontally
	 * @param (Optional) mosaic Whether to mosaic the sprite // TODO: What is this?
	 */
	Sprite(bool top, SpriteSize size, SpriteColorFormat format, int x = 0, int y = 0, int priority = 0, int id = -1, int paletteAlpha = 15, int rotationIndex = -1, bool doubleSize = false, bool visible = true, bool vFlip = false, bool hFlip = false, bool mosaic = false);

	Sprite(const Sprite &sprite);
	~Sprite(void);

	Sprite &operator=(const Sprite &sprite);

	u16* gfx(void) const { return _gfx; }
	SpriteSize size(void) const { return _size; }
	SpriteColorFormat format(void) const { return _format; }
	bool visibility(void) const { return _visibility; }
	int id(void) const { return _id; }
	int rotationIndex(void) const { return _rotationIndex; }
	int paletteAlpha(void) const { return _paletteAlpha; }
	int x(void) const { return _x; }
	int y(void) const { return _y; }
	int width(void) const { return _width; }
	int height(void) const { return _height; }
	int rotation(void) const { return _rotation; }
	float scaleX(void) const { return _scaleX; }
	float scaleY(void) const { return _scaleY; }

	/**
	 * @brief Sets the alpha of the sprite
	 * @param alpha The alpha, 0 - 15
	 */
	void alpha(int alpha) { oamSetAlpha(_oam, _id, alpha); }

	/**
	 * @brief Sets the flip of the sprite
	 * @param hFlip The horizontal flip
	 * @param vFlip The vertical flip
	 */
	void flip(bool hFlip, bool vFlip) { oamSetFlip(_oam, _id, hFlip, vFlip); }

	/**
	 * @brief Sets the position of the sprite
	 * @param x The X position
	 * @param y The Y position
	 */
	void position(int x, int y) { _x = x, _y = y; if(_visibility) oamSetXY(_oam, _id, _x, _y); }

	/**
	 * @brief Sets the priority of the sprite
	 * @param priority The priority, 0 - 3 with 0 being on top
	 */
	void priority(int priority) { oamSetPriority(_oam, _id, priority); }

	/**
	 * @brief Sets the scale of the sprite
	 * @param scaleX The scale on the X axis
	 * @param scaleY The scale on the Y axis
	 */
	void rotation(int rotation);

	/**
	 * @brief Sets the scale of the sprite
	 * @param scaleX The scale on the X axis
	 * @param scaleY The scale on the Y axis
	 */
	void scale(float scaleX, float scaleY);

	/**
	 * @brief Sets the visibility of the sprite
	 * @param show Whether to show the sprite or not, true to show
	 */
	void visibility(bool show);

	/**
	 * @brief Clears the sprite
	 */
	void clear(void);

	/**
	 * @brief Fills the sprite with a solid color
	 * @param color The color to fill with
	 */
	void fillColor(u16 color);

	/**
	 * @brief Draws a solid color rectangle to the sprite
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param w The width of the rectangle
	 * @param h The height of the rectangle
	 * @param color The color of the rectangle
	 */
	void drawRectangle(int x, int y, int w, int h, u16 color) { drawRectangle(x, y, w, h, color, color); }
	/**
	 * @brief Draws a solid color rectangle to the sprite
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param w The width of the rectangle
	 * @param h The height of the rectangle
	 * @param color The color of even rows
	 * @param color The color of odd rows
	 */
	void drawRectangle(int x, int y, int w, int h, u16 color1, u16 color2);

	/**
	 * @brief Draws the image to a sprite, can skip alpha and scale the image
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param image The image to draw to the sprite
	 * @param scaleX (Optional) The scale for the X axis
	 * @param scaleY (Optional) The scale for the Y axis
	 */
	void drawImage(int x, int y, const Image &image, float scaleX = 1.0f, float scaleY = 1.0f);

	/**
	 * @brief Draws a segment of an image to a sprite, faster but overwrites alpha and no scaling
	 * @param x The X position to draw at
	 * @param y The Y position to draw at
	 * @param imageX The X position in the image to draw from
	 * @param imageY The Y position in the image to draw from
	 * @param w The width to draw
	 * @param h The height to draw
	 * @param image The image to draw to the sprite
	 * @param scaleX (Optional) The scale for the X axis
	 * @param scaleY (Optional) The scale for the Y axis
	 */
	void drawImageSegment(int x, int y, int imageX, int imageY, int w, int h, const Image &image, float scaleX = 1.0f, float scaleY = 1.0f);

	/**
	 * @brief Updates the OAM that this sprite is on, also updates all other sprites on that screen. Avoid calling this excessively, if changing a lot of sprites call it once when they're all done.
	 */
	void update(void) { oamUpdate(_oam); }
	/**
	 * @brief Updates all sprites on the given screen. Avoid calling this excessively, if changing a lot of sprites call it once when they're all done.
	 * @param top Whether to update the top or bottom screen
	 */
	static void update(bool top) { oamUpdate(top ? &oamMain : &oamSub); }
};

#endif
