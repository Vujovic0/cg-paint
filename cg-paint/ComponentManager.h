#pragma once
#include <vector>
#include "Component.h"
#include <memory>
#include <string>
#include "Renderer.h"

class ComponentManager {
private:
	int focused = -1;
	std::vector<std::unique_ptr<Component>> components;
public:
	void drawAll(Renderer& r);
	void onClick(int mx, int my);
	void addComponent(std::unique_ptr<Component> component);
	void onChar(char c);
	void onBackspace();
	std::string getValue(size_t index) const {
		return components[index]->getValue();
	}
	bool hasFocus() const { return focused != -1; }
};