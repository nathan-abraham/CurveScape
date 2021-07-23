#pragma once
#include "userEvent.h"

class TextDeletedEvent : public UserEvent {
public:
	char* textField;
	char lastChar;
	TextDeletedEvent(char* text, char deletedChar);
	~TextDeletedEvent();
	void undo() override;
	void redo() override;
};
