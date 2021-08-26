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

#include "screenCommon.hpp"
#include "tonccpy.h"

#include <nds/bios.h>

void Image::decompressGrf(void *dst, const void *src) {
	if(!src || !dst)
		return;

	u32 header = *(u32*)src;
	u32 size = header >> 8;
	
	switch(header & 0xF0) {
		case 0x00: // No compression
			tonccpy(dst, (u8*)src + 4, size);
			break;
		case 0x10: // LZ77
			decompress(src, dst, LZ77);
			break;
		case 0x20: // Huffman
			decompress(src, dst, HUFF);
			break;
		case 0x30: // RLE
			decompress(src, dst, RLE);
			break;
		default:
			break;
	}
}


Image::Image(const std::vector<std::string> &paths, u8 paletteStart) {
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

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	u8 *grf = new u8[size];
	if(grf) {
		fread(grf, 1, size, file);

		load(grf, paletteStart);

		delete[] grf;
	}
}

Image::Image(const u8 *grf, u8 paletteStart) {
	load(grf, paletteStart);
}

void Image::load(const u8 *grf, u8 paletteStart) {
	const u32 *ptr = (u32 *)grf;
	if(!ptr || ptr[0] != 0x46464952 || ptr[2] != 0x20465247) {
		return;
	}
	
	u32 size = ptr[1]; // RIFF size
	ptr += 3; // Skip to first section

	while((u8 *)ptr < grf + size) {
		switch(*ptr) {
			case 0x20524448: { // 'HDR '
				memcpy(&_width, ptr + 4, 4);
				memcpy(&_height, ptr + 5, 4);
				break;
			} case 0x20584647: { // 'GFX '
				_bitmap = std::vector<u8>(ptr[2] >> 8);
				decompressGrf(_bitmap.data(), ptr + 2);
				changePaletteStart(paletteStart);
				break;
			} case 0x204C4150: { // 'PAL '
				_palette = std::vector<u16>((ptr[2] >> 8) / 2);
				decompressGrf(_palette.data(), ptr + 2);
				break;
			} default: {
				// Not supported yet
				break;
			}
		}

		ptr += 2 + ptr[1] / 4;
	}
}

void Image::changePaletteStart(u8 paletteStart) {
	int moveBy = paletteStart - _paletteStart;

	if(moveBy != 0) {
		for(u8 &px : _bitmap) {
			px += moveBy;
		}

		_paletteStart = paletteStart;
	}
}

void Image::copyPalette(int paletteStart) {
	tonccpy((currentScreen ? BG_PALETTE : BG_PALETTE_SUB) + paletteStart, _palette.data(), _palette.size() * 2);
}

void Image::draw(int x, int y, float scaleX, float scaleY, bool skipAlpha) {
	SCALE_3DS(x);
	SCALE_3DS(y);
	char s[64];
	// _bitmap[0] = 39;
	snprintf(s, sizeof(s), "%lu, %lu, %u, %u, 0x%X", _width, _height, _bitmap.size(), _bitmap[0], 0);

	// If the scale is 1 use faster integer math
	if(scaleX == 1.0f && scaleY == 1.0f) {
		if(skipAlpha) {
			for(u32 i = 0; i < _height; i++) {
				u8 *src = _bitmap.data() + i * _width;
				u8 *dst = (u8 *)bgGetGfxPtr(currentScreen ? 3 : 7) + (y + i) * 256 + x;
				for(u32 j = 0; j < _width; j++) {
					if(_palette[src[j] - _paletteStart] != 0x7C1F)
						toncset(dst + j, src[j], 1);
				}
			}
		} else {
			u8 *dst = (u8 *)bgGetGfxPtr(currentScreen ? 3 : 7) + y * 256 + x;
			for(u32 i = 0; i < _height; i++) {
				tonccpy(dst + i * 256, _bitmap.data() + i * _width, _width);
			}
		}
	} else {
		for(u32 i = 0; i < _height * scaleY; i++) {
			u8 *dst = (u8 *)bgGetGfxPtr(currentScreen ? 3 : 7) + (y + i) * 256 + x;
			for(u32 j = 0; j < _width * scaleX; j++) {
				u8 px = _bitmap[int(i / scaleY) * _width + int(j / scaleX)];
				if(_palette[px - _paletteStart] != 0x7C1F || !skipAlpha)
					toncset(dst + j, px, 1);
			}
		}
	}
}

void Image::drawSegment(int x, int y, int imageX, int imageY, int w, int h, float scaleX, float scaleY, bool skipAlpha) {
	SCALE_3DS(x);
	SCALE_3DS(y);

	// If the scale is 1 use faster integer math
	if(scaleX == 1.0f && scaleY == 1.0f) {
		if(skipAlpha) {
			for(int i = 0; i < h; i++) {
				u8 *src = _bitmap.data() + i * _width;
				u8 *dst = (u8 *)bgGetGfxPtr(currentScreen ? 3 : 7) + (y + i) * 256 + x;
				for(int j = 0; j < w; j++) {
					if(_palette[src[j] - _paletteStart] != 0x7C1F)
						toncset(dst + j, src[j], 1);
				}
			}
		} else {
			for(int i = 0; i < h; i++) {
				tonccpy((u8 *)bgGetGfxPtr(currentScreen ? 3 : 7) + (y + i) * 256 + x,
						_bitmap.data() + (imageY + i) * _width + imageX, w);
			}
		}
	} else {
		for(u32 i = 0; i < h * scaleY; i++) {
			u8 *dst = (u8 *)bgGetGfxPtr(currentScreen ? 3 : 7) + (y + i) * 256 + x;
			for(u32 j = 0; j < w * scaleX; j++) {
				u8 px = _bitmap[(imageY + int(i / scaleY)) * _width + imageX + int(j / scaleX)];
				if(_palette[px - _paletteStart] != 0x7C1F || !skipAlpha)
					toncset(dst + j, px, 1);
			}
		}
	}
}
