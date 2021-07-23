#include <string.h>
#include "userEvent.h"
#include "textEnteredEvent.h"

TextEnteredEvent::TextEnteredEvent(char* text) {
	textField = text;
	lastChar = '\0';
}

TextEnteredEvent::~TextEnteredEvent() {}

void TextEnteredEvent::undo() {
	lastChar = textField[strlen(textField) - 1];
	textField[strlen(textField) - 1] = '\0';
}

void TextEnteredEvent::redo() {
	int len = strlen(textField);
	textField[len] = lastChar;
	textField[len+1] = '\0';
}