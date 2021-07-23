#include <stack>
#include "userEvent.h"
#include "eventManager.h"

EventManager::EventManager() {}

EventManager::~EventManager() {
	while (!undo.empty()) {
		UserEvent* ev = undo.top();
		delete ev;
		undo.pop();
	}
	while (!redo.empty()) {
		UserEvent* ev = redo.top();
		delete ev;
		redo.pop();
	}
}

void EventManager::undo_event() {
	if (!undo.empty()) {
		UserEvent* ev = undo.top();
		ev->undo();
		redo.push(ev);
		undo.pop();
	}
}

void EventManager::redo_event() {
	if (!redo.empty()) {
		UserEvent* ev = redo.top();
		ev->redo();
		if (!redo.empty())
			redo.pop();
		undo.push(ev);
	}
}

void EventManager::addEvent(UserEvent* event) {
	undo.push(event);
}