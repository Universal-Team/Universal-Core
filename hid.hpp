/*
*   This file is part of Universal-Core
*   Copyright (C) 2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#ifndef HID_HPP
#define HID_HPP

#include <cstddef>

class HID
{
public:
	// The parameters to initialize this class.
	HID(size_t EntryPerPage, size_t EntryAmount);
	// get the current index.
	size_t getIndex();
	// get the max amount of the index.
	size_t getMaxEntries();
	// Go to the next Entry, if maxEntries is not reached.
	void nextEntry();
	// Go to the last Entry, if 0 is not reached.
	void lastEntry();
	// Go to the next Page.
	void nextPage();
	// Go to the previous Page.
	void prevPage();
	// Get the current page.
	size_t getPage();

private:
	size_t maxEntries;
	size_t currentEntry = 0;
	size_t pageEntry;
	size_t currentPage = 0;
};

#endif