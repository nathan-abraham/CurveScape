#pragma once
#include "userEvent.h"
#include <stack>

class EventManager {
public:
    std::stack<UserEvent*> undo;
    std::stack<UserEvent*> redo;

    EventManager();
    ~EventManager();
    void undo_event();
    void redo_event();
    void addEvent(UserEvent*);
};
