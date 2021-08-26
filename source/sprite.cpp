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

#include "gui.hpp"
#include "tonccpy.h"

u8 Sprite::_assigned[2][128] = {{false}, {false}};

Sprite::Sprite(bool top, SpriteSize size, SpriteColorFormat format, int x, int y, int priority, int id,
			   int paletteAlpha, int rotationIndex, bool doubleSize, bool visible, bool vFlip, bool hFlip, bool mosaic)
	: _top(top), _oam(top ? &oamMain : &oamSub), _size(size), _format(format), _x(x), _y(y), _priority(priority),
	  _id(id), _rotationIndex(rotationIndex), _paletteAlpha(paletteAlpha), _visibility(visible) {
	SCALE_3DS(_x);
	SCALE_3DS(_y);

	// If the ID is -1, set it to the first free one
	if(_id == -1) {
		for(uint i = 0; i < sizeof(_assigned[_top]) / sizeof(_assigned[_top][0]); i++) {
			if(!_assigned[_top][i]) {
				_id = i;
				break;
			}
		}
	}

	_assigned[top][_id]++;

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
	oamSet(_oam, _id, _x, _y, _priority, _paletteAlpha, _size, _format, _gfx, _rotationIndex, doubleSize, !_visibility,
		   vFlip, hFlip, mosaic);
}

Sprite::Sprite(const Sprite &sprite)
	: _top(sprite._top), _oam(sprite._oam), _size(sprite._size), _format(sprite._format), _x(sprite._x), _y(sprite._y),
	  _priority(sprite._priority), _id(sprite._id), _rotationIndex(sprite._rotationIndex),
	  _paletteAlpha(sprite._paletteAlpha), _width(sprite._width), _height(sprite._height), _rotation(sprite._rotation),
	  _scaleX(sprite._scaleX), _scaleY(sprite._scaleY), _visibility(sprite._visibility), _gfx(sprite._gfx) {
	_assigned[_top][_id]++;
}

Sprite &Sprite::operator=(const Sprite &sprite) {
	_top           = sprite._top;
	_oam           = sprite._oam;
	_size          = sprite._size;
	_format        = sprite._format;
	_x             = sprite._x;
	_y             = sprite._y;
	_priority      = sprite._priority;
	_id            = sprite._id;
	_rotationIndex = sprite._rotationIndex;
	_paletteAlpha  = sprite._paletteAlpha;
	_width         = sprite._width;
	_height        = sprite._height;
	_rotation      = sprite._rotation;
	_scaleX        = sprite._scaleX;
	_scaleY        = sprite._scaleY;
	_visibility    = sprite._visibility;
	_gfx           = sprite._gfx;

	_assigned[_top][_id]++;

	return *this;
}

Sprite::~Sprite(void) {
	_assigned[_top][_id]--;
	if(!_assigned[_top][_id]) {
		oamFreeGfx(_oam, _gfx);
		oamClearSprite(_oam, _id);
		oamUpdate(_oam);
	}
}

void Sprite::position(int x, int y) {
	SCALE_3DS(x);
	SCALE_3DS(y);

	_x = x;
	_y = y;
	
	if(_visibility)
		oamSetXY(_oam, _id, _x, _y);
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

void Sprite::drawOutline(int x, int y, int w, int h, u16 color) {
	h += y;
	if(y >= 0 && y <= _height)
		toncset16(_gfx + y * _width + std::max(x, 0), color, std::min(w, _width - x));
	for(y++; y < (h - 1); y++) {
		if(y >= 0 && y <= _height && x >= 0)
			_gfx[y * _width + x] = color;
		if(y >= 0 && y <= _height && x + w <= _width)
			_gfx[y * _width + x + w - 1] = color;
	}
	if(y >= 0 && y <= _height)
		toncset16(_gfx + y * _width + std::max(x, 0), color, std::min(w, _width - x));
}

void Sprite::drawRectangle(int x, int y, int w, int h, u16 color1, u16 color2) {
	for(int i = 0; i < h; i++) {
		toncset16(_gfx + ((y + i) * _width + x), ((i % 2) ? color1 : color2), w);
	}
}

void Sprite::drawImage(int x, int y, const Image &image, float scaleX, float scaleY) {
	// If the scale is 1 use faster integer math
	if(scaleX == 1.0f && scaleY == 1.0f) {
		for(int i = 0; i < image.height(); i++) {
			for(int j = 0; j < image.width(); j++) {
				u16 px = image.palette()[image.bitmap()[i * image.width() + j] - image.paletteStart()];
				if(px & 0x8000)
					_gfx[(y + i) * _height + x + j] = px;
			}
		}
	} else {
		for(int i = 0; i < image.height() * scaleY; i++) {
			for(int j = 0; j < image.width() * scaleX; j++) {
				u16 px =
					image.palette()[image.bitmap()[int(i / scaleY) * image.width() + int(j / scaleX)] - image.paletteStart()];
				if(px & 0x8000)
					_gfx[(y + i) * _height + j + x] = px;
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
				u16 px = image.palette()[image.bitmap()[(imageY + i) * image.width() + imageX + j] - image.paletteStart()];
				if(px & 0x8000)
					_gfx[(y + i) * _height + x + j] = px;
			}
		}
	} else {
		for(int i = 0; i < h * scaleY; i++) {
			for(int j = 0; j < w * scaleX; j++) {
				u16 px = image.palette()[image.bitmap()[(imageY + int(i / scaleY)) * image.width() + imageX +
														int(j / scaleX)] -
										 image.paletteStart()];
				if(px & 0x8000)
					_gfx[(y + i) * _height + x + j] = px;
			}
		}
	}
}
