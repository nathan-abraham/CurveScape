#pragma once

class GraphToggledEvent : public UserEvent {
public:
	bool* toggleField;

	GraphToggledEvent(bool*);
	~GraphToggledEvent();
	void undo() override;
	void redo() override;
};