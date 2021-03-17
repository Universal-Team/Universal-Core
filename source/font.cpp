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

#include "font.hpp"

#include "tonccpy.h"

#ifdef TEXT_BUFFERED
u8 Font::textBuf[2][256 * 192];
#endif

Font::Font(const std::vector<std::string> &paths) {
	FILE *file = nullptr;
	for(const auto &path : paths) {
		file = fopen(path.c_str(), "rb");
		if(file)
			break;
	}

	if(file) {
		// Get file size
		fseek(file, 0, SEEK_END);
		u32 fileSize = ftell(file);

		// Skip font info
		fseek(file, 0x14, SEEK_SET);
		fseek(file, fgetc(file) - 1, SEEK_CUR);

		// Load glyph info
		u32 chunkSize;
		fread(&chunkSize, 4, 1, file);
		tileWidth  = fgetc(file);
		tileHeight = fgetc(file);
		fread(&tileSize, 2, 1, file);

		// Load character glyphs
		int tileAmount = ((chunkSize - 0x10) / tileSize);
		fontTiles      = std::vector<u8>(tileSize * tileAmount);
		fseek(file, 4, SEEK_CUR);
		fread(fontTiles.data(), tileSize, tileAmount, file);

		// Fix top row
		for(int i = 0; i < tileAmount; i++) {
			fontTiles[i * tileSize]     = 0;
			fontTiles[i * tileSize + 1] = 0;
			fontTiles[i * tileSize + 2] = 0;
		}

		// Load character widths
		fseek(file, 0x24, SEEK_SET);
		u32 locHDWC;
		fread(&locHDWC, 4, 1, file);
		fseek(file, locHDWC - 4, SEEK_SET);
		fread(&chunkSize, 4, 1, file);
		fseek(file, 8, SEEK_CUR);
		fontWidths = std::vector<u8>(3 * tileAmount);
		fread(fontWidths.data(), 3, tileAmount, file);

		// Load character maps
		fontMap = std::vector<u16>(tileAmount);
		fseek(file, 0x28, SEEK_SET);
		u32 locPAMC, mapType;
		fread(&locPAMC, 4, 1, file);

		while(locPAMC < fileSize) {
			u16 firstChar, lastChar;
			fseek(file, locPAMC, SEEK_SET);
			fread(&firstChar, 2, 1, file);
			fread(&lastChar, 2, 1, file);
			fread(&mapType, 4, 1, file);
			fread(&locPAMC, 4, 1, file);

			switch(mapType) {
				case 0: {
					u16 firstTile;
					fread(&firstTile, 2, 1, file);
					for(unsigned i = firstChar; i <= lastChar; i++) {
						fontMap[firstTile + (i - firstChar)] = i;
					}
					break;
				}
				case 1: {
					for(int i = firstChar; i <= lastChar; i++) {
						u16 tile;
						fread(&tile, 2, 1, file);
						fontMap[tile] = i;
					}
					break;
				}
				case 2: {
					u16 groupAmount;
					fread(&groupAmount, 2, 1, file);
					for(int i = 0; i < groupAmount; i++) {
						u16 charNo, tileNo;
						fread(&charNo, 2, 1, file);
						fread(&tileNo, 2, 1, file);
						fontMap[tileNo] = charNo;
					}
					break;
				}
			}
		}
		fclose(file);
		questionMark = charIndex('?');
	}
}

u16 Font::charIndex(char16_t c) {
	// Try a binary search
	int left  = 0;
	int right = fontMap.size();

	while(left <= right) {
		int mid = left + ((right - left) / 2);
		if(fontMap[mid] == c) {
			return mid;
		}

		if(fontMap[mid] < c) {
			left = mid + 1;
		} else {
			right = mid - 1;
		}
	}

	// If not found, return a question mark
	return questionMark;
}

std::u16string Font::utf8to16(std::string_view text) {
	std::u16string out;
	for(uint i = 0; i < text.size();) {
		char16_t c;
		if(!(text[i] & 0x80)) {
			c = text[i++];
		} else if((text[i] & 0xE0) == 0xC0) {
			c = (text[i++] & 0x1F) << 6;
			c |= text[i++] & 0x3F;
		} else if((text[i] & 0xF0) == 0xE0) {
			c = (text[i++] & 0x0F) << 12;
			c |= (text[i++] & 0x3F) << 6;
			c |= text[i++] & 0x3F;
		} else {
			i++; // out of range or something (This only does up to 0xFFFF since it goes to a U16 anyways)
		}
		out += c;
	}
	return out;
}

int Font::calcWidth(std::u16string_view text) {
	uint x = 0;

	for(auto c : text) {
		u16 index = charIndex(c);
		x += fontWidths[(index * 3) + 2];
	}

	return x;
}

ITCM_CODE void Font::print(std::u16string_view text, int x, int y, bool top, int layer, Alignment align, int maxWidth,
						   int color, float scaleX, float scaleY, bool rtl, Sprite *sprite) {
	// If RTL isn't forced, check for RTL text
	for(const auto c : text) {
		if(c >= 0x0590 && c <= 0x05FF) {
			rtl = true;
			break;
		}
	}
	auto ltrBegin = text.end(), ltrEnd = text.end();

	// Adjust x for alignment
	switch(align) {
		case Alignment::left: {
			break;
		}
		case Alignment::center: {
			size_t newline = text.find('\n');
			while(newline != text.npos) {
				print(text.substr(0, newline), x, y, top, layer, align, maxWidth, color, scaleX, scaleY, rtl, sprite);
				text    = text.substr(newline + 1);
				newline = text.find('\n');
				y += tileHeight;
			}

			x += ((sprite ? sprite->width() : 256) - (calcWidth(text) * scaleX)) / 2;
			break;
		}
		case Alignment::right: {
			size_t newline = text.find('\n');
			while(newline != text.npos) {
				print(text.substr(0, newline), x - (calcWidth(text.substr(0, newline)) * scaleX), y, top, layer,
					  Alignment::left, maxWidth, color, scaleX, scaleY, rtl, sprite);
				text    = text.substr(newline + 1);
				newline = text.find('\n');
				y += tileHeight;
			}
			x -= calcWidth(text) * scaleX;
			break;
		}
	}
	const int xStart = x;

	if(maxWidth != 0)
		scaleX = std::min(scaleX, (float)maxWidth / (calcWidth(text) * scaleX));

	// Loop through string and print it
	for(auto it = (rtl ? text.end() - 1 : text.begin()); true; it += (rtl ? -1 : 1)) {
		// If we hit the end of the string in an LTR section of an RTL
		// string, it may not be done, if so jump back to printing RTL
		if(it == (rtl ? text.begin() - 1 : text.end())) {
			if(ltrBegin == text.end() || (ltrBegin == text.begin() && ltrEnd == text.end())) {
				break;
			} else {
				it       = ltrBegin;
				ltrBegin = text.end();
				rtl      = true;
			}
		}

		// If at the end of an LTR section within RTL, jump back to the RTL
		if(it == ltrEnd && ltrBegin != text.end()) {
			if(ltrBegin == text.begin())
				break;

			it       = ltrBegin;
			ltrBegin = text.end();
			rtl      = true;
			// If in RTL and hit a non-RTL character that's not punctuation, switch to LTR
		} else if(rtl &&
				  ((*it < 0x0590 || *it > 0x05FF) &&
				   ((*it >= '0' && *it <= '9') || (*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z') ||
					*it >= 127))) {
			// Save where we are as the end of the LTR section
			ltrEnd = it + 1;

			// Go back until an RTL character or the start of the string
			while((*it < 0x0590 || *it > 0x05FF) && it != text.begin())
				it--;

			// Save where we are to return to after printing the LTR section
			ltrBegin = it;

			// If on an RTL char right now, add one
			if(*it >= 0x0590 && *it <= 0x05FF)
				it++;

			// Skip all punctuation at the end
			while(*it < '0' || (*it > '9' && *it < 'A') || (*it > 'Z' && *it < 'a') || (*it > 'z' && *it < 127)) {
				if(it != text.begin())
					ltrBegin++;
				it++;
			}
			rtl = false;
		}

		if(*it == '\n') {
			x = xStart;
			y += tileHeight * scaleY;
			continue;
		}

		u16 index = charIndex(*it);

		// Brackets are flipped in RTL
		if(rtl) {
			switch(*it) {
				case '(':
					index = charIndex(')');
					break;
				case ')':
					index = charIndex('(');
					break;
				case '[':
					index = charIndex(']');
					break;
				case ']':
					index = charIndex('[');
					break;
				case '<':
					index = charIndex('>');
					break;
				case '>':
					index = charIndex('<');
					break;
				default:
					break;
			}
		}

		if(sprite) {
			// Don't draw off sprite chars
			if(x >= 0 && x < sprite->width() && y >= 0 && y + tileHeight < sprite->height()) {
				u16 *dst = sprite->gfx() + x + fontWidths[(index * 3)];
				// Use faster integer math if scale is 1
				if(scaleX == 1.0f && scaleY == 1.0f) {
					for(int i = 0; i < tileHeight; i++) {
						for(int j = 0; j < tileWidth; j++) {
							u8 px = fontTiles[(index * tileSize) + (i * tileWidth + j) / 4] >>
									((3 - ((i * tileWidth + j) % 4)) * 2) &
								3;
							if(px)
								dst[(y + i) * sprite->width() + j] = px + (color * 4);
						}
					}
				} else {
					for(float i = 0.0f; i < tileHeight; i += 1 / scaleY) {
						for(float j = 0.0f; j < tileWidth; j += 1 / scaleY) {
							u8 px = fontTiles[(index * tileSize) + (i * tileWidth + j) / 4] >>
									((3 - (int(i * tileWidth + j) % 4)) * 2) &
								3;
							if(px)
								dst[int((y + i) * sprite->width() + j)] = px + (color * 4);
						}
					}
				}
			}
		} else {
			// Don't draw off screen chars
			if(x >= 0 && x + fontWidths[(index * 3) + 2] < 256 && y >= 0 && y + tileHeight < 192) {
#ifdef TEXT_BUFFERED
				u8 *dst = textBuf[top] + y * 256 + x + fontWidths[(index * 3)];
#else
				u8 *dst = (u8 *)bgGetGfxPtr(top ? layer : layer + 4) + y * 256 + x + fontWidths[(index * 3)];
#endif
				// Use faster integer math if scale is 1
				if(scaleX == 1.0f && scaleY == 1.0f) {
					for(int i = 0; i < tileHeight; i++) {
						for(int j = 0; j < tileWidth; j++) {
							u8 px = fontTiles[(index * tileSize) + (i * tileWidth + j) / 4] >>
									((3 - ((i * tileWidth + j) % 4)) * 2) &
								3;
							if(px) {
#ifdef TEXT_BUFFERED
								dst[i * 256 + j] = px + (color * 4);
#else
								toncset(dst + i * 256 + j, px + (color * 4), 1);
#endif
							}
						}
					}
				} else {
					for(int i = 0; i < tileHeight * scaleY; i++) {
						for(int j = 0; j < tileWidth * scaleX; j++) {
							u8 loc = int(i / scaleY) * tileWidth + int(j / scaleX);
							u8 px  = fontTiles[index * tileSize + loc / 4] >> ((3 - (loc % 4)) * 2) & 3;
							if(px) {
#ifdef TEXT_BUFFERED
								dst[i * 256 + j] = px + (color * 4);
#else
								toncset(dst + i * 256 + j, px + (color * 4), 1);
#endif
							}
						}
					}
				}
			}
		}

		x += fontWidths[(index * 3) + 2] * scaleX;
	}
}

#ifdef TEXT_BUFFERED
void Font::clear(bool top) { dmaFillWords(0, Font::textBuf[top], 256 * 192); }

void Font::update(bool top) {
	tonccpy(bgGetGfxPtr(top ? TEXT_TOP_LAYER : TEXT_BOTTOM_LAYER + 4), Font::textBuf[top], 256 * 192);
}
#endif
