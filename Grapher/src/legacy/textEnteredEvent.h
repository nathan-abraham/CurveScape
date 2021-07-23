#pragma once

class TextEnteredEvent : public UserEvent {
public:
	char* textField;
	char lastChar;
	TextEnteredEvent(char* text);
	~TextEnteredEvent();
	void undo() override;
	void redo() override;
};
