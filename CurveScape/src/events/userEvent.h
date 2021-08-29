#pragma once

enum EventType {
    E_PANEL_TOGGLE = 0,
    E_TEXT_ENTER,
    E_DROPDOWN_SELECT,
    E_GRAPH_TOGGLE,
};

class UserEvent {
public:
    UserEvent();
    virtual ~UserEvent();
    virtual void undo() = 0;
    virtual void redo() = 0;
};

