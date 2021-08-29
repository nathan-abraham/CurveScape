#include "userEvent.h"
#include "graphToggledEvent.h"

GraphToggledEvent::GraphToggledEvent(bool* field) {
    toggleField = field;
}

GraphToggledEvent::~GraphToggledEvent() {}

void GraphToggledEvent::undo() {
    *toggleField = !(*toggleField);
}

void GraphToggledEvent::redo() {
    *toggleField = !(*toggleField);
}