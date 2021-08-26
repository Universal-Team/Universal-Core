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

#include "spritesheet.hpp"

#include <nds.h>

Image Spritesheet::_blank = Image(nullptr, 0);

Spritesheet::Spritesheet(const std::vector<std::string> &paths, const std::vector<u32> indexes) {
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

	u32 magic;
	fread(&magic, 4, 1, file);
	if(magic != 0x5844542E) // '.TDX'
		return;

	u32 imageCount;
	fread(&imageCount, 4, 1, file);

	_images.resize(imageCount);
	
	for(u32 i = 0; i < imageCount; i++) {
		if(indexes.size() == 0 || std::find(indexes.begin(), indexes.end(), i) != indexes.end()) {
			fseek(file, 0x10 + i * 8, SEEK_SET);

			u32 offset, size;
			fread(&offset, 4, 1, file);
			fread(&size, 4, 1, file);

			u8 *grf = new u8[size];
			fseek(file, offset, SEEK_SET);
			fread(grf, 1, size, file);

			_images[i] = new Image(grf, 0);

			delete[] grf;
		} else {
			_images[i] = nullptr;
		}
	}

	fclose(file);
}

Spritesheet::~Spritesheet(void) {
	for(Image *image : _images) {
		if(image)
			delete image;
	}
}

Image &Spritesheet::operator[](size_t index) {
	if(_images[index] && index < _images.size())
		return *_images[index];
	else
		return _blank;
}