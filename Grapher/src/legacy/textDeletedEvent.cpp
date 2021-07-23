#include <string.h>
#include "textDeletedEvent.h"
#include "userEvent.h"

TextDeletedEvent::TextDeletedEvent(char* text, char deletedChar) {
	textField = text;
	lastChar = deletedChar;
}

TextDeletedEvent::~TextDeletedEvent() {}

void TextDeletedEvent::undo() {
	int len = strlen(textField);
	textField[len] = lastChar;
	textField[len+1] = '\0';
}

void TextDeletedEvent::redo() {
	lastChar = textField[strlen(textField) - 1];
	textField[strlen(textField) - 1] = '\0';
}