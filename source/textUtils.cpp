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

#include "TextUtils.hpp"

#include <string.h>

/* Get first codepoint from a UTF-8 string. */
char32_t TextUtils::GetCodepoint(const char *Str) {
	/* Return 0 if nullptr or empty string. */
	if (!Str || !*Str) return 0;

	size_t Len = strlen(Str);
	char32_t Codepoint = 0xFFFD;

	if (!(*Str & 0x80)) {
		Codepoint = *Str;

	} else if ((*Str & 0xE0) == 0xC0 && Len >= 2) {
		Codepoint  = (*(Str++) & 0x1F) << 6;
		Codepoint |=  *(Str++) & 0x3F;

	} else if ((*Str & 0xF0) == 0xE0 && Len >= 3) {
		Codepoint  = (*(Str++) & 0x0F) << 12;
		Codepoint |= (*(Str++) & 0x3F) << 6;
		Codepoint |=  *(Str++) & 0x3F;

	} else if ((*Str & 0xF8) == 0xF0 && Len >= 4) {
		Codepoint  = (*(Str++) & 0x07) << 18;
		Codepoint |= (*(Str++) & 0x3F) << 12;
		Codepoint |= (*(Str++) & 0x3F) << 6;
		Codepoint |=  *(Str++) & 0x3F;
	};

	return Codepoint;
};

/* Try to make the first given character have a dakuten. */
std::string TextUtils::Dakutenify(std::string Str, bool Handakuten) {
	char32_t Char = GetCodepoint(Str.c_str());
	if (Char >= u'ァ' && Char <= u'ヶ') Char -= 0x60; // Katakana, convert to Hiragana

	int Change = 0;

	if (Handakuten) {
		for (const char16_t Item : Handakutenable) {
			if (Char == Item) {
				Change = 2;
				break;
			};
		};

	} else {
		if (Char == u'う') {
			Change = 0x4E;

		} else {
			for (const char16_t Item : Dakutenable) {
				if (Char == Item) {
					Change = 1;
					break;
				};
			};
		};
	};

	if (Change) {
		if ((Str[2] & 0x3F) + Change < 0x3F) {
			Str[2] += Change;

		} else {
			Str[2] = 0x80 | ((Str[2] + Change) & 0x3F);
			Str[1]++;
		};

	} else {
		int I = 1;
		while((Str[I] & 0xC0) == 0x80) I++;
		Str.insert(I, Handakuten ? "゜" : "゛");
	};

	return Str;
};

uint32_t TextUtils::StrToKey(const std::string &Str) {
	for (const auto &Key : KeyNames) {
		if (strcmp(Str.c_str(), Key.first) == 0) return Key.second;
	};

	return 0;
};
