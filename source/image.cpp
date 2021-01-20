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

#include "image.hpp"

#include "tonccpy.h"

Image::Image(const std::vector<std::string> &paths) {
	// Attempt to load from the given paths
	FILE *file = nullptr;
	for(const auto &path : paths) {
		file = fopen(path.c_str(), "rb");
		if(file)
			break;
	}

	// Return if no file found
	if(!file)
		return;

	// Return if not in the correct format
	char magic[5] = {0};
	fread(magic, 1, 4, file);
	if(strcmp(magic, ".GFX"))
		return;

	// Read image
	fread(&_width, 1, 2, file);
	fread(&_height, 1, 2, file);
	_bitmap = std::vector<u8>(_width * _height);
	fread(_bitmap.data(), 1, _bitmap.size(), file);
	u16 palCount;
	fread(&palCount, 1, 2, file);
	fread(&_palOfs, 1, 2, file);
	_palette = std::vector<u16>(palCount);
	fread(_palette.data(), 2, palCount, file);
	fclose(file);
}

Image::Image(FILE *file) {
	// Return if no file
	if(!file)
		return;

	// Return if not in the correct format
	char magic[5];
	fread(magic, 1, 4, file);
	if(strcmp(magic, ".GFX"))
		return;

	// Read image
	fread(&_width, 1, 2, file);
	fread(&_height, 1, 2, file);
	_bitmap = std::vector<u8>(_width * _height);
	fread(_bitmap.data(), 1, _bitmap.size(), file);
	u16 palCount;
	fread(&palCount, 1, 2, file);
	fread(&_palOfs, 1, 2, file);
	_palette = std::vector<u16>(palCount);
	fread(_palette.data(), 2, palCount, file);
	fclose(file);
}

void Image::draw(int x, int y, bool top, int layer, bool copyPal) {
	if(copyPal)
		tonccpy((top ? BG_PALETTE : BG_PALETTE_SUB) + _palOfs, _palette.data(), _palette.size() * 2);

	u8 *dst = (u8 *)bgGetGfxPtr(top ? layer : layer + 4) + y * 256 + x;

	// If full width and X is 0, copy it all in one go
	if(_width == 256 && x == 0) {
		tonccpy(dst, _bitmap.data(), _width * _height);
	} else {
		for(int i = 0; i < _height; i++) {
			tonccpy(dst + i * 256, _bitmap.data() + i * _width, _width);
		}
	}
}

void Image::drawSpecial(int x, int y, bool top, int layer, float scaleX, float scaleY, int paletteOffset,
						bool copyPal) {
	if(copyPal)
		tonccpy((top ? BG_PALETTE : BG_PALETTE_SUB) + _palOfs + paletteOffset, _palette.data(), _palette.size() * 2);

	u8 *dst = (u8 *)bgGetGfxPtr(top ? layer : layer + 4) + y * 256 + x;

	// If the scale is 1 use faster integer math
	if(scaleX == 1.0f && scaleY == 1.0f) {
		for(int i = 0; i < _height; i++) {
			for(float j = 0; j < _width; j++) {
				u8 px = _bitmap[i * _width + j];
				if(_palette[px - _palOfs] & 0x8000)
					toncset(dst + i * 256 + j, px + paletteOffset, 1);
			}
		}
	} else {
		for(int i = 0; i < _height * scaleY; i++) {
			for(int j = 0; j < _width * scaleX; j++) {
				u8 px = _bitmap[int(i / scaleY) * _width + int(j / scaleX)];
				if(_palette[px - _palOfs] & 0x8000)
					toncset(dst + i * 256 + j, px + paletteOffset, 1);
			}
		}
	}
}

void Image::drawSegment(int x, int y, int imageX, int imageY, int w, int h, bool top, int layer, bool copyPal) {
	if(copyPal)
		tonccpy((top ? BG_PALETTE : BG_PALETTE_SUB) + _palOfs, _palette.data(), _palette.size() * 2);

	for(int i = 0; i < h; i++) {
		tonccpy((u8 *)bgGetGfxPtr(top ? layer : layer + 4) + (y + i) * 256 + x,
				_bitmap.data() + (imageY + i) * _width + imageX, w);
	}
}

void Image::drawSegmentSpecial(int x, int y, int imageX, int imageY, int w, int h, bool top, int layer, float scaleX,
							   float scaleY, int paletteOffset, bool copyPal) {
	if(copyPal)
		tonccpy((top ? BG_PALETTE : BG_PALETTE_SUB) + _palOfs + paletteOffset, _palette.data(), _palette.size() * 2);

	u8 *dst = (u8 *)bgGetGfxPtr(top ? layer : layer + 4) + y * 256 + x;

	// If the scale is 1 use faster integer math
	if(scaleX == 1.0f && scaleY == 1.0f) {
		for(int i = 0; i < h; i++) {
			for(int j = 0; j < w; j++) {
				u8 px = _bitmap[i * _width + j];
				if(_palette[px - _palOfs] & 0x8000)
					toncset(dst + i * 256 + j, px + paletteOffset, 1);
			}
		}
	} else {
		for(int i = 0; i < h * scaleY; i++) {
			for(int j = 0; j < w * scaleX; j++) {
				u8 px = _bitmap[(imageY + int(i / scaleY)) * _width + imageX + int(j / scaleX)];
				if(_palette[px - _palOfs] & 0x8000)
					toncset(dst + i * 256 + j, px + paletteOffset, 1);
			}
		}
	}
}
