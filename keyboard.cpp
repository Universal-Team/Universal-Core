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

#include "keyboard.hpp"
#include "screenCommon.hpp"

std::string UCKeyboard::GetString(uint MaxSize, const std::string &Hint) const {
	C3D_FrameEnd(0); // Needed, so the system will not freeze.

	SwkbdState state;
	swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, MaxSize);
	char temp[MaxSize + 1] = { 0 };
	swkbdSetHintText(&state, Hint.c_str());
	swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, SWKBD_FILTER_PROFANITY, 0);

	SwkbdButton ret = swkbdInputText(&state, temp, sizeof(temp));
	temp[MaxSize] = '\0';

	return (ret == SWKBD_BUTTON_CONFIRM ? temp : "");
}


uint UCKeyboard::GetInt(uint Max, const std::string &Text) const {
	C3D_FrameEnd(0); // Needed, so the system will not freeze.

	uint Digits = 0;
	while(Max / (int)pow(10, Digits)) Digits++;


	SwkbdState state;
	swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, Digits);
	char temp[Digits + 1] = { 0 };
	swkbdSetHintText(&state, Text.c_str());
	swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, SWKBD_FILTER_PROFANITY, 0);

	SwkbdButton ret = swkbdInputText(&state, temp, sizeof(temp));
	temp[Digits] = '\0';

	return (ret == SWKBD_BUTTON_CONFIRM ? (int)std::min(std::stoi(temp), (int)Max) : 0);
}
