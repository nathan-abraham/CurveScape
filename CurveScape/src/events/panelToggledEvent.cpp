#include "userEvent.h"
#include "panelToggledEvent.h"

PanelToggledEvent::PanelToggledEvent(bool* field) {
    toggleField = field;
}

PanelToggledEvent::~PanelToggledEvent() {}

void PanelToggledEvent::undo() {
    *toggleField = !(*toggleField);
}

void PanelToggledEvent::redo() {
    *toggleField = !(*toggleField);
}