#pragma once

class PanelToggledEvent : public UserEvent {
public:
	bool* toggleField;

	PanelToggledEvent(bool*);
	~PanelToggledEvent();
	void undo() override;
	void redo() override;
};
