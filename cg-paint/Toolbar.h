#pragma once
#include <functional>
#include <memory>
#include "ComponentManager.h"
#include "InputField.h"
#include "Button.h"
#include "Geometry.h"

class Toolbar {
private:
    gmt::Rect bounds;
    ComponentManager manager;
    std::function<void(Color)> onSubmit;
public:
    Toolbar(int width, int height, int originX, int originY,
        std::function<void(Color)> onSubmit);

    Toolbar(const Toolbar&) = delete;
    Toolbar& operator=(const Toolbar&) = delete;

    void draw(Renderer& r);
    void onClick(int mx, int my) { manager.onClick(mx, my); }
    void onChar(char c) { manager.onChar(c); }
    void onBackspace() { manager.onBackspace(); }
    bool hasFocus() const { return manager.hasFocus(); }
    void setColor(Color c);
    void submit();
};