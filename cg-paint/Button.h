#pragma once
#include <functional>
#include <string>
#include "Component.h"
#include "Geometry.h"

class Button : public Component {
    gmt::Rect bounds;
    std::string label;
    std::function<void()> callback;
public:
    Button(int width, int height, int originX, int originY,
        std::string label, std::function<void()> callback)
        : bounds{ originX, originY, width, height },
        label(std::move(label)), callback(std::move(callback)) {
    }

    bool isClickedOn(int mx, int my) const override {
        return gmt::isClickedOn(bounds, mx, my);
    }
    void onClick(int, int) override { if (callback) callback(); }
    void draw(Renderer& r, bool focused);
};