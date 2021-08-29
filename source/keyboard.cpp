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

#include "gui.hpp"
#include "JSON.hpp"
#include "textUtils.hpp"

#include <unistd.h>

UCKeyboard::UCKeyboard(const std::string &KeyboardJSON, uint8_t BgColor, uint8_t BarColor, uint8_t OutlineColor, uint8_t KeyColor, uint8_t KeyColorPressed, uint8_t KeyColorActive, uint8_t TextColor, uint8_t HintColor) : BgColor(BgColor), BarColor(BarColor), OutlineColor(OutlineColor), KeyColor(KeyColor), KeyColorPressed(KeyColorPressed), KeyColorActive(KeyColorActive), TextColor(TextColor), HintColor(HintColor) {
	FILE *File = fopen(KeyboardJSON.c_str(), "rt");

	if (File) {
		nlohmann::json Json = nlohmann::json::parse(File, nullptr, false);
		fclose(File);

		/* Clear. */
		this->Kbd.clear();
		this->CurrentMode.clear();
		this->CurrentMode.push_back("!main");

		if (Json.contains("info") && Json["info"].is_object()) {
			/* UCKeyboard global X/Y offset */
			if (Json["info"].contains("x") && Json["info"]["x"].is_number()) this->KbdX = Json["info"]["x"];
			if (Json["info"].contains("y") && Json["info"]["y"].is_number()) this->KbdY = Json["info"]["y"];
		};

		if (Json.contains("layout") && Json["layout"].is_object()) {
			/* Loop through each mode and parse a struct out of the JSON. */
			for (const auto &Mode : Json["layout"].items()) {
				if (Mode.value().is_object() && Mode.value().contains("keys") && Mode.value()["keys"].is_object()) {
					this->Kbd[Mode.key()] = { };

					/* Add all the keys*/
					for (const auto &Key : Mode.value()["keys"].items()) {
						/* Check that the positions are good. */
						if (Key.value().is_array() && Key.value().size() >= 4) {
							bool Good = true;

							for (uint8_t Idx = 0; Idx < 4; Idx++) {
								if (!Key.value()[Idx].is_number()) {
									Good = false;
									break;
								};
							};

							if (Good) {
								this->Kbd[Mode.key()].Keys.emplace_back(Structs::ButtonPos({ this->KbdX + Key.value()[0].get<int>(), this->KbdY + Key.value()[1].get<int>(), Key.value()[2], Key.value()[3] }), Key.key());

								/* Check for any special properties. */
								if (Key.value().size() >= 5 && Key.value()[4].is_object()) {
									for (const auto &Property : Key.value()[4].items()) {
										if (Property.key() == "key") {
											if (Property.value().is_string()) {
												this->Kbd[Mode.key()].Keys.back().Button = TextUtils::StrToKey(Property.value());

											} else if (Property.value().is_array()) {
												for (const auto &Button : Property.value()) {
													if (Button.is_string()) {
														this->Kbd[Mode.key()].Keys.back().Button |= TextUtils::StrToKey(Button);
													};
												};
											};

										} else if (Property.value().is_string()) {
											if (Property.key() == "label") {
												this->Kbd[Mode.key()].Keys.back().Label = Property.value();

											} else {
												Key::Property Prop = Key::Property::Invalid;
												if (Property.key() == "action") Prop = Key::Property::Action;
												else if (Property.key() == "mode") Prop = Key::Property::Mode;
												else if (Property.key() == "value") Prop = Key::Property::Value;

												this->Kbd[Mode.key()].Keys.back().Properties[Prop] = Property.value();
											};

										} else if (Property.value().is_boolean()) {
											if (Property.key() == "active") this->Kbd[Mode.key()].Keys.back().Active = Property.value();
										};
									};
								};
							};
						};
					};

					/* Check if this should return on key press. */
					if (Mode.value().contains("return") && Mode.value()["return"].is_boolean()) {
						this->Kbd[Mode.key()].Ret = Mode.value()["return"];
					};
				};
			};
		};

		this->Loaded = true;

	} else {
		this->Loaded = false;
	};
};

uint8_t UCKeyboard::GetCharSize(void) const {
	const char *Str = this->CurrentString.c_str() + Cursor;
	do {
		Str++;
	} while ((*Str & 0xC0) == 0x80);

	return Str - this->CurrentString.c_str();
}

uint8_t UCKeyboard::GetPrevCharSize(void) const {
	const char *Str = this->CurrentString.c_str() + Cursor;
	do {
		Str--;
	} while ((*Str & 0xC0) == 0x80);

	return this->CurrentString.c_str() - Str;
}

void UCKeyboard::Draw(uint32_t Held, uint32_t Repeat, touchPosition T) const {
	Gui::clearTextBufs();

	/* A sub menu or so? */
	if (!this->Loaded) {
		Gui::Draw_Rect(48, 0, 320, 20, this->BarColor);
		Gui::Draw_Rect(48, 20, 320, 1, this->OutlineColor);
		Gui::DrawStringCentered(24, 2, 1.0f, this->TextColor, "Invalid keyboard layout", 310);

	} else {
		Gui::Draw_Rect(0, 0, 320, 240, this->BgColor);
		Gui::DrawStringCentered(24, 2, 1.0f, this->TextColor, this->CurrentString, 310);

		if (this->Kbd.contains(this->CurrentMode.back())) {
			for (const auto &Key : this->Kbd.at(this->CurrentMode.back()).Keys) {
				Gui::Draw_Rect(Key.Pos.x, Key.Pos.y, Key.Pos.w, Key.Pos.h, (Key.Active || Key.Pos.Touched(T) || Held & Key.Button) ? this->KeyColorPressed : this->KeyColor);
				Gui::DrawStringCentered(Key.Pos.x + (Key.Pos.w / 2) - 160, Key.Pos.y + (Key.Pos.h / 10), 1.0f, this->TextColor, Key.Label);
			};

		} else {
			Gui::Draw_Rect(48, 0, 320, 20, this->BarColor);
			Gui::Draw_Rect(48, 20, 320, 1, this->OutlineColor);
			Gui::DrawStringCentered(24, 2, 1.0f, this->TextColor, "Invalid keyboard layout", 310);
		};
	};

	Gui::updateTextBufs(false);
};


void UCKeyboard::SwitchLayout() const {
	// TODO?
};


void UCKeyboard::Handler(uint32_t Held, uint32_t Repeat, touchPosition T) {
	/* Handle Load. */
	if (!this->Loaded)
		return;

	if (Repeat & KEY_TOUCH) {
		/* Check if any key is being touched. */
		for (const auto &Key : this->Kbd[this->CurrentMode.back()].Keys) {
			if (Key.Pos.Touched(T)) {
				this->HandleKeyPress(Key);
				break;
			};
		};

	} else if(Repeat) {
		/* If not touching, then check all keys for button values. */
		for (const auto &Key : this->Kbd[this->CurrentMode.back()].Keys) {
			if (Repeat & Key.Button) {
				this->HandleKeyPress(Key);
			};
		};
	};
};

void UCKeyboard::HandleKeyPress(const Key &Key) {
	/* Return to last non-returning layout. */
	while (this->Kbd.at(this->CurrentMode.back()).Ret) this->CurrentMode.pop_back();

	/* If the key has any special properties, then apply them. */
	if (Key.Properties.size() > 0) {
		for (const auto &[Prop, Value] : Key.Properties) {
			switch (Prop) {
				/* Special action, such as modifying other characters. */
				case Key::Property::Action:
					if (Value == "backspace") {
						if (this->Cursor > 0) {
							this->Cursor -= GetPrevCharSize();
							this->CurrentString = this->CurrentString.substr(0, this->Cursor) + this->CurrentString.substr(this->Cursor + GetCharSize(), this->CurrentString.size());
						};
					} else if (Value == "delete") {
						// TextEditor::Remove();
						
					} else if (Value == "up") {
						// TextEditor::CursorUp();

					} else if (Value == "down") {
						// TextEditor::CursorDown();

					} else if (Value == "left") {
						// TextEditor::CursorLeft();

					} else if (Value == "right") {
						// TextEditor::CursorRight();

					} else if (Value == "dakuten" || Value == "handakuten") {
						// bool Handakuten = Value == "handakuten";

						// if (TextEditor::CursorPos > 0) {
						// 	TextEditor::CursorLeft();
						// 	const std::string Char = UniversalEdit::UE->CurrentFile->GetCharacter(TextEditor::CurrentLine, TextEditor::CursorPos);
						// 	const std::string Out = TextUtils::Dakutenify(Char, Handakuten);

						// 	UniversalEdit::UE->CurrentFile->EraseContent(TextEditor::CurrentLine, TextEditor::CursorPos, Char.size());
						// 	if (UniversalEdit::UE->CurrentFile->InsertContent(TextEditor::CurrentLine, TextEditor::CursorPos, Out)) {
						// 		TextEditor::CursorRight();

						// 		if (Out.size() > Char.size()) TextEditor::CursorRight();
						// 	};

						// } else {
						// 	if (UniversalEdit::UE->CurrentFile->InsertContent(TextEditor::CurrentLine, TextEditor::CursorPos, Handakuten ? "゜" : "゛")) {
						// 		TextEditor::CursorPos += 3;
						// 	};
						// };

					} else if (Value == "newline") {
						// TextEditor::InsertLine();

					} else if (Value == "exit") {
						IsDone = true;

					} else if (Value == "layout") {
						this->SwitchLayout();
						return; // Return to not mess up.
						
					} else if (Value == "phrases") {
						// UniversalEdit::UE->ActiveTab = UniversalEdit::Tabs::Phrases;
						return;
					};
					break;

				/* Changes mode, such as to Shift mode. */
				case Key::Property::Mode:
					if (this->Kbd.contains(Value)) this->CurrentMode.push_back(Value);
					else if (Value == "!return" && this->CurrentMode.size() > 1) this->CurrentMode.pop_back();
					break;

				/* Output a value that's not the label. */
				case Key::Property::Value:
					if(this->CurrentString.size() + Value.size() < this->MaxSize) {
						this->CurrentString += Value;
						this->Cursor += Value.size();
					};
					break;

				case Key::Property::Invalid:
					break;
			};
		};

	} else {
		nocashMessage(Key.Label.c_str());
		/* Otherwise, just output the label */
		if(this->CurrentString.size() + Key.Label.size() < this->MaxSize) {
			nocashMessage("adding");
			this->CurrentString += Key.Label;
			this->Cursor += Key.Label.size();
		};
	};
};

std::string UCKeyboard::GetString(uint MaxSize, const std::string &Hint) {
	this->MaxSize = MaxSize == 0 ? 0xFFFFFFFF : 0;

	u32 Held = 0, Repeat = 0;
	touchPosition T;
	while(1) {
		this->Draw(Held, Repeat, T);
		this->Handler(Held, Repeat, T);

		do {
			swiWaitForVBlank();
			scanKeys();
			Held = keysHeld();
			Repeat = keysDownRepeat();
			touchRead(&T);
			SCALE_3DS(T.px);
			SCALE_3DS(T.py);
		} while (!Held);

		if (Held & KEY_START) {
			break;
		};
	};

	return this->CurrentString;
};
