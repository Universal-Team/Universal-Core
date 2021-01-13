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

void Image::draw(int x, int y, bool top, int layer, int channel, bool copyPal) {
	if(copyPal)
		tonccpy((top ? BG_PALETTE : BG_PALETTE_SUB) + _palOfs, _palette.data(), _palette.size() * 2);

	u8 *dst   = (u8 *)bgGetGfxPtr(top ? layer : layer + 4);
	int width = 256;

	// If full width and X is 0, copy it all in one go
	if(_width == width && x == 0) {
		dmaCopyHalfWords(channel, _bitmap.data(), dst, _width * _height);
	} else {
		for(int i = 0; i < _height; i++) {
			dmaCopyHalfWords(channel, _bitmap.data() + (i * _width), dst + (y + i) * 256 + x, _width);
		}
	}
}

void Image::drawSpecial(int x, int y, bool top, int layer, float scaleX, float scaleY, int paletteOffset,
						bool copyPal) {
	if(copyPal)
		tonccpy((top ? BG_PALETTE : BG_PALETTE_SUB) + _palOfs + paletteOffset, _palette.data(), _palette.size() * 2);

	u8 *dst = (u8 *)bgGetGfxPtr(top ? layer : layer + 4);

	// If the scale is 1 use faster integer math
	if(scaleX == 1.0f && scaleY == 1.0f) {
		for(int i = 0; i < _height; i++) {
			for(float j = 0; j < _width; j++) {
				u8 px = _bitmap[i * _width + j];
				if(_palette[px - _palOfs] & 0x8000)
					toncset(dst + int((y + i) * 256 + x + j), px + paletteOffset, 1);
			}
		}
	} else {
		for(float i = 0.0f; i < _height; i += 1 / scaleY) {
			for(float j = 0.0f; j < _width; j += 1 / scaleX) {
				u8 px = _bitmap[int((i * _width) + j)];
				if(_palette[px - _palOfs] & 0x8000)
					toncset(dst + int((y + i) * 256 + x + j), px + paletteOffset, 1);
			}
		}
	}
}

void Image::drawSegment(int x, int y, int imageX, int imageY, int w, int h, bool top, int layer, int channel,
						bool copyPal) {
	if(copyPal)
		tonccpy((top ? BG_PALETTE : BG_PALETTE_SUB) + _palOfs, _palette.data(), _palette.size() * 2);

	for(int i = 0; i < h; i++) {
		dmaCopyHalfWords(channel, _bitmap.data() + ((imageY + i) * _width) + imageX,
						 (u8 *)bgGetGfxPtr(top ? layer : layer + 4) + ((y + i) * 256) + x, w);
	}
}

void Image::drawSegmentSpecial(int x, int y, int imageX, int imageY, int w, int h, bool top, int layer, float scaleX,
							   float scaleY, int paletteOffset, bool copyPal) {
	if(copyPal)
		tonccpy((top ? BG_PALETTE : BG_PALETTE_SUB) + _palOfs + paletteOffset, _palette.data(), _palette.size() * 2);

	u8 *dst = (u8 *)bgGetGfxPtr(top ? layer : layer + 4);

	// If the scale is 1 use faster integer math
	if(scaleX == 1.0f && scaleY == 1.0f) {
		for(int i = 0; i < h; i++) {
			for(int j = 0; j < w; j++) {
				u8 px = _bitmap[i * _width + j];
				if(_palette[px - _palOfs] & 0x8000)
					toncset(dst + ((y + i) * 256 + x + j), px + paletteOffset, 1);
			}
		}
	} else {
		for(float i = 0; i < h; i += 1 / scaleX) {
			for(float j = 0; j < _width; j += 1 / scaleY) {
				u8 px = _bitmap[i * _width + j];
				if(_palette[px - _palOfs] & 0x8000)
					toncset(dst + int((y + i) * 256 + x + j), px + paletteOffset, 1);
			}
		}
	}
}
