#include "Toolbar.h"
#include <algorithm>
#include <string>
#include "globals.h"

Toolbar::Toolbar(int width, int height, int originX, int originY,
    std::function<void(Color)> onSubmit)
    : bounds{ originX, originY, width, height }, onSubmit(std::move(onSubmit)) {

    const int y = originY + (height - 30) / 2;  
    for (int i = 0; i < 4; i++)                  
        manager.addComponent(std::make_unique<InputField>(80, 30, originX + 10 + i * 90, y));

    manager.addComponent(std::make_unique<Button>(
        80, 30, originX + 10 + 4 * 90, y, "OK", [this] { submit(); }));
}

static unsigned char parseChannel(const std::string& s) {
    if (s.empty()) return 0;
    try {
        return std::clamp(std::stoi(s), 0, 255);
    }
    catch (...) {
        return 0;
    }
}

void Toolbar::submit() {
    if (!onSubmit) return;
    onSubmit(Color{ parseChannel(manager.getValue(0)),
        parseChannel(manager.getValue(1)),
        parseChannel(manager.getValue(2)),
        parseChannel(manager.getValue(3)) });
}

void Toolbar::draw(Renderer& r) {
    r.fillRect(bounds.x, bounds.y, bounds.width, bounds.height, Color{ 230, 230, 230, 255 });
    manager.drawAll(r);
}