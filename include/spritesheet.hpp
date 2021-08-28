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

#ifndef _UNIVERSAL_CORE_SPRITESHEET_HPP
#define _UNIVERSAL_CORE_SPRITESHEET_HPP

#include "image.hpp"

class Spritesheet {
private:
	static Image _blank;

	std::vector<Image *> _images;

public:
	/**
	 * @brief Image group class
	 * @param paths The paths to try load the spritesheet from, from highest to lowest priority
	 * @param indexes The image indexes to load, if blank all images will be loaded
	 */
	Spritesheet(const std::vector<std::string> &paths, const std::vector<u32> indexes = {});
	/**
	 * @brief Image group class
	 * @param path The path to load the spritesheet from
	 * @param indexes The image indexes to load, if blank all images will be loaded
	 */
	Spritesheet(const std::string &path, const std::vector<u32> indexes = {}) : Spritesheet(std::vector<std::string>({path}), indexes) {};

	~Spritesheet(void);

	/**
	 * @brief Returns the number of images in the spritesheet
	 */
	size_t size(void) const { return _images.size(); }

	/**
	 * @brief Returns the Image at index
	 * @param index The Image to get
	 */
	Image &operator[](size_t index) const;
};

#endif
