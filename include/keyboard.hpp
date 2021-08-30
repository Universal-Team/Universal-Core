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

#ifndef _UNIVERSAL_CORE_KEYBOARD_HPP
#define _UNIVERSAL_CORE_KEYBOARD_HPP

#include "structs.hpp"

#include <map>
#include <string>
#include <vector>

class UCKeyboard {
public:
	enum class Status {
		Active,
		DoneEnter,
		DoneExit
	};

private:
	struct Key {
		enum class Property : uint8_t { Invalid, Action, Mode, Value };

		Structs::ButtonPos Pos;
		std::string Label;
		std::map<Property, std::string> Properties = { };
		bool Active = false;
		uint32_t Button = 0;

		Key(Structs::ButtonPos Pos, const std::string &Label) : Pos(Pos), Label(Label) { };
	};

	struct Mode {
		std::vector<Key> Keys = { };
		bool Ret = false;
	};

	uint8_t BgColor, BarColor, OutlineColor, KeyColor, KeyColorPressed, KeyColorActive, TextColor, HintColor;

	bool Loaded = false;

	std::vector<std::string> CurrentMode = { "!main" };
	int KbdX = 0, KbdY = 0;
	std::map<std::string, Mode> Kbd;

	std::string CurrentString = "";
	int Cursor = 0;
	uint MaxSize = 0;
	Status CurrentStatus = Status::Active;

	uint8_t GetCharSize(void) const;
	uint8_t GetPrevCharSize(void) const;

	void HandleKeyPress(const Key &Key);
	void SwitchLayout() const;
public:
	/**
	 * @brief UCKeyboard user input class.
	 * @param KeyboardJSON The path to the layout JSON.
	 * @param BgColor The background color.
	 * @param BgColor The bar color.
	 * @param BgColor The outline color.
	 * @param KeyColor The key color.
	 * @param KeyColorPressed The pressed key color.
	 * @param KeyColorActive The active key color.
	 * @param TextColor The text color.
	 * @param HintColor The hint text color.
	 */
	UCKeyboard(const std::string &KeyboardJSON, uint8_t BgColor, uint8_t BarColor, uint8_t OutlineColor, uint8_t KeyColor, uint8_t KeyColorPressed, uint8_t KeyColorActive, uint8_t TextColor, uint8_t HintColor);

	~UCKeyboard(void) { };

	/**
	 * @brief Draws the keyboard, use with Handler() for live input.
	 * @param Held The value from keysHeld().
	 * @param Repeat The value from keysDownRepeat().
	 * @param T The value from touchRead().
	 */
	void Draw(uint32_t Held, uint32_t Repeat, touchPosition T) const;

	/**
	 * @brief Handles keyboard actions, use with Draw() for live input.
	 * @param Held The value from keysHeld().
	 * @param Repeat The value from keysDownRepeat().
	 * @param T The value from touchRead().
	 */
	void Handler(uint32_t Held, uint32_t Repeat, touchPosition T);

	/**
	 * @brief Gets the current string for use in live input mode.
	 */
	std::string String(void) const { return CurrentString; };

	/**
	 * @brief Gets the status of the keyboard, use to check if the user has finished.
	 */
	Status GetStatus(void) const { return CurrentStatus; };

	/**
	 * @brief Gets a string from user input.
	 * @param maxSize The maximum size *in bytes*, set to 0 for no limit.
	 * @param Hint The hint text.
	 */
	std::string GetString(uint MaxSize, const std::string &Hint);

	/**
	 * @brief Gets an int from user input.
	 * @param maxSize The maximum size of the number, set to 0 for no limit.
	 * @param Hint The hint text.
	 */
	uint GetInt(uint Max, const std::string &Hint);
};

#endif
