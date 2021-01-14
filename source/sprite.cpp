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

#include "sprite.hpp"

#include "tonccpy.h"

bool Sprite::_assigned[2][127];

Sprite::Sprite(bool top, SpriteSize size, SpriteColorFormat format, int x, int y, int priority, int id,
			   int paletteAlpha, int rotationIndex, bool doubleSize, bool visible, bool vFlip, bool hFlip, bool mosaic)
	: _top(top), _oam(top ? &oamMain : &oamSub), _size(size), _format(format), _x(x), _y(y), _priority(priority),
	  _id(id), _rotationIndex(rotationIndex), _paletteAlpha(paletteAlpha) {
	// If the ID is -1, set it to the first free one
	if(_id == -1) {
		for(uint i = 0; i < sizeof(_assigned[top]) / sizeof(_assigned[top][0]); i++) {
			if(!_assigned[top][i]) {
				_assigned[top][i] = true;
				_id               = i;
				break;
			}
		}
	}

	// Get the sprite width and height from the SpriteSize
	if(((_size >> 12) & 3) == OBJSHAPE_SQUARE) {
		_width  = 8 << ((_size >> 14) & 3);
		_height = 8 << ((_size >> 14) & 3);
	} else if(((_size >> 12) & 3) == OBJSHAPE_WIDE) {
		switch(_size & 0xFF) {
			case 4:
				_width  = 16;
				_height = 8;
				break;
			case 8:
				_width  = 32;
				_height = 8;
				break;
			case 16:
				_width  = 32;
				_height = 16;
				break;
			case 64:
				_width = 64;
				_width = 32;
				break;
		}
	} else if(((_size >> 12) & 3) == OBJSHAPE_TALL) {
		switch(_size & 0xFF) {
			case 4:
				_width  = 8;
				_height = 16;
				break;
			case 8:
				_width  = 8;
				_height = 32;
				break;
			case 16:
				_width  = 16;
				_height = 32;
				break;
			case 64:
				_width  = 32;
				_height = 64;
				break;
		}
	}

	// Allocate memory for graphics
	_gfx = oamAllocateGfx(_oam, _size, _format);

	// Set sprite
	oamSet(_oam, _id, _x, _y, _priority, _paletteAlpha, _size, _format, _gfx, _rotationIndex, doubleSize, !visible,
		   vFlip, hFlip, mosaic);
}

Sprite::~Sprite(void) {
	oamFreeGfx(_oam, _gfx);
	_assigned[_top][_id] = false;
}

void Sprite::rotation(int rotation) {
	_rotation = rotation;
	if(_rotationIndex != -1)
		oamRotateScale(_oam, _rotationIndex, _rotation, (1 / _scaleX) * (1 << 8), (1 / _scaleY) * (1 << 8));
}

void Sprite::scale(float scaleX, float scaleY) {
	_scaleX = scaleX;
	_scaleY = scaleY;
	if(_rotationIndex != -1)
		oamRotateScale(_oam, _rotationIndex, _rotation, (1 / scaleX) * (1 << 8), (1 / scaleY) * (1 << 8));
}

void Sprite::visibility(bool show) {
	_visibility = show;
	if(_rotationIndex == -1) {
		oamSetHidden(_oam, _id, !_visibility);
	} else {
		// Can't hide sprites with rotation / scaling, so move them off screen
		if(_visibility)
			oamSetXY(_oam, _id, _x, _y);
		else
			oamSetXY(_oam, _id, -64, -64);
	}
}

void Sprite::clear(void) { toncset16(_gfx, 0, (_size & 0xFF) << 5); }

void Sprite::fillColor(u16 color) { toncset16(_gfx, color, (_size & 0xFF) << 5); }

void Sprite::drawRectangle(int x, int y, int w, int h, u16 color1, u16 color2) {
	for(int i = 0; i < h; i++) {
		toncset(_gfx + ((y + i) * _width + x), ((i % 2) ? color1 : color2), w);
	}
}

void Sprite::drawImage(int x, int y, const Image &image, float scaleX, float scaleY) {
	// If the scale is 1 use faster integer math
	if(scaleX == 1.0f && scaleY == 1.0f) {
		for(int i = 0; i < image.height(); i++) {
			for(int j = 0; j < image.width(); j++) {
				u16 px = image.palette()[image.bitmap()[i * image.width() + j] - image.palOfs()];
				if(px & 0x8000)
					toncset16(_gfx + (y + i) * _height + x + j, px, 1);
			}
		}
	} else {
		for(float i = 0.0f; i < image.height(); i += 1 / scaleY) {
			for(float j = 0.0f; j < image.width(); j += 1 / scaleX) {
				u16 px = image.palette()[image.bitmap()[int(i * image.width() + j)] - image.palOfs()];
				if(px & 0x8000)
					toncset16(_gfx + int((y + i) * _height + j) + x, px, 1);
			}
		}
	}
}

void Sprite::drawImageSegment(int x, int y, int imageX, int imageY, int w, int h, const Image &image, float scaleX,
							  float scaleY) {
	// If the scale is 1 use faster integer math
	if(scaleX == 1.0f && scaleY == 1.0f) {
		for(int i = 0; i < h; i++) {
			for(int j = 0; j < w; j++) {
				u16 px = image.palette()[image.bitmap()[i * image.width() + j] - image.palOfs()];
				if(px & 0x8000)
					toncset16(_gfx + ((y + i) * _height + x + j), px, 1);
			}
		}
	} else {
		for(float i = 0; i < h; i += 1 / scaleX) {
			for(float j = 0; j < image.width(); j += 1 / scaleY) {
				u16 px = image.palette()[image.bitmap()[i * image.width() + j] - image.palOfs()];
				if(px & 0x8000)
					toncset16(_gfx + int((y + i) * _height + x + j), px, 1);
			}
		}
	}
}