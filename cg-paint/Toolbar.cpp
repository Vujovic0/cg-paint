#include "Toolbar.h"
#include "Renderer.h"
#include <algorithm>
#include <string>
#include "globals.h"

namespace {
    constexpr int FIELD_W = 80, FIELD_H = 30;
    constexpr int LABEL_W = 20;                    // prostor za slovo ispred polja
    constexpr int SLOT_W = LABEL_W + FIELD_W + 10; // labela + polje + razmak
    constexpr int PAD = 10;
    const char* LABELS[4] = { "R", "G", "B", "A" };
    const char* AUTHOR = "Vukasin Vujovic SV11/2023";
}

Toolbar::Toolbar(int width, int height, int originX, int originY,
    std::function<void(Color)> onSubmit)
    : bounds{ originX, originY, width, height }, onSubmit(std::move(onSubmit)) {

    const int y = originY + (height - FIELD_H) / 2;
    for (int i = 0; i < 4; i++)
        manager.addComponent(std::make_unique<InputField>(
            FIELD_W, FIELD_H, originX + PAD + i * SLOT_W + LABEL_W, y));

    // iza polja: kvadrat sa trenutnom bojom (40 px), pa dugme
    const int afterFields = originX + PAD + 4 * SLOT_W;
    manager.addComponent(std::make_unique<Button>(
        80, FIELD_H, afterFields + 40 + PAD, y, "OK", [this] { submit(); }));
}

static unsigned char parseChannel(const std::string& s) {
    if (s.empty()) return 0;
    try {
        return (unsigned char)std::clamp(std::stoi(s), 0, 255);
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

void Toolbar::setColor(Color c) {
    manager.setValue(0, std::to_string((int)c.r));
    manager.setValue(1, std::to_string((int)c.g));
    manager.setValue(2, std::to_string((int)c.b));
    manager.setValue(3, std::to_string((int)c.a));
}

void Toolbar::draw(Renderer& r) {
    r.fillRect(bounds.x, bounds.y, bounds.width, bounds.height, Color{ 230, 230, 230, 255 });

    const int fieldY = bounds.y + (bounds.height - FIELD_H) / 2;
    const int baseline = fieldY + (FIELD_H + r.textHeight()) / 2;

    for (int i = 0; i < 4; i++)
        r.drawText(LABELS[i], bounds.x + PAD + i * SLOT_W, baseline, Color{ 0, 0, 0, 255 });

    const int swatchX = bounds.x + PAD + 4 * SLOT_W;
    const Color cur = gbs::cursorPtr->color;
    r.fillRect(swatchX, fieldY, 40, FIELD_H, Color{ 80, 80, 80, 255 });    
    r.fillRect(swatchX + 1, fieldY + 1, 38, FIELD_H - 2, Color{ 255, 255, 255, 255 });
    r.fillRect(swatchX + 1, fieldY + 1, 38, FIELD_H - 2, cur);

    manager.drawAll(r);

    const int tw = r.measureText(AUTHOR);
    r.drawText(AUTHOR, bounds.x + bounds.width - tw - PAD, baseline, Color{ 0, 0, 0, 255 });
}