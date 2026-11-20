#pragma once
#include "ComponentManager.h"
#include <string>
#include <iostream>

void ComponentManager::drawAll(Renderer& r) {
	for (size_t i = 0; i < components.size(); i++)
		components[i]->draw(r, (int)i == focused);
}

void ComponentManager::addComponent(std::unique_ptr<Component> component) {
    components.push_back(std::move(component));
}

void ComponentManager::onClick(int mx, int my) {
    focused = -1;
    for (size_t i = 0; i < components.size(); i++) {
        if (components[i]->isClickedOn(mx, my)) {
            components[i]->onClick(mx, my);
            focused = (int)i;
            break;
        }
    }
}

void ComponentManager::onChar(char c) {
    if (focused != -1) components[focused]->onChar(c);
}

void ComponentManager::onBackspace() {
    if (focused != -1) components[focused]->onBackspace();
}
