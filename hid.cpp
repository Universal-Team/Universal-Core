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

#include "hid.hpp"

HID::HID(size_t EntryPerPage, size_t EntryAmount) {
	maxEntries = EntryAmount;
	pageEntry = EntryPerPage;
}

size_t HID::getIndex() {
	return currentEntry;
}

size_t HID::getMaxEntries() {
	return maxEntries;
}

void HID::nextEntry() {
	if (currentEntry < maxEntries-1) {
		currentEntry++;
	}
}

void HID::lastEntry() {
	if (currentEntry > 0) {
		currentEntry--;
	}
}

void HID::nextPage() {
	// Only go to the next page, if the next Page doesn't reach the maxEntries.
	if (currentEntry + pageEntry < maxEntries - 1) {
		currentPage++;
		currentEntry += pageEntry;
		// If the first index of the page is smaller than maxEntries -> Go to the next page.
	} else if (currentPage * pageEntry < maxEntries - 1) {
		currentPage++;
		currentEntry = currentPage * pageEntry;
	}
}

size_t HID::getPage() {
	return currentPage;
}

void HID::prevPage() {
	if (currentPage > 0) {
		currentPage--;
		currentEntry -= pageEntry;
	}
}