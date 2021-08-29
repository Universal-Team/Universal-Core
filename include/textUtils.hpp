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

#ifndef _UNIVERSAL_CORE_TEXTUTILS_HPP
#define _UNIVERSAL_CORE_TEXTUTILS_HPP

#include <array>
#include <string>
#include <nds/ndstypes.h>
#include <nds/input.h>

class TextUtils {
private:
	static constexpr std::array<char16_t, 20> Dakutenable = {
		u'か', u'き', u'く', u'け', u'こ',
		u'さ', u'し', u'す', u'せ', u'そ',
		u'た', u'ち', u'つ', u'て', u'と',
		u'は', u'ひ', u'ふ', u'へ', u'ほ'
	};

	static constexpr std::array<char16_t, 5> Handakutenable = {
		u'は', u'ひ', u'ふ', u'へ', u'ほ'
	};

	static constexpr std::array<std::pair<const char *, uint32_t>, 13> KeyNames = {{
		{ "A", KEY_A },
		{ "B", KEY_B },
		{ "SELECT", KEY_SELECT },
		{ "START", KEY_START },
		{ "R", KEY_R },
		{ "L", KEY_L },
		{ "X", KEY_X },
		{ "Y", KEY_Y },
		{ "TOUCH", KEY_TOUCH },
		{ "UP", KEY_UP },
		{ "DOWN", KEY_DOWN },
		{ "LEFT", KEY_LEFT },
		{ "RIGHT", KEY_RIGHT }
	}};

public:
	static char32_t GetCodepoint(const char *Str);
	static std::string Dakutenify(std::string Str, bool Handakuten);
	static uint32_t StrToKey(const std::string &Str);
};

#endif
