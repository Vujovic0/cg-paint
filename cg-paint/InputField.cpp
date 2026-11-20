#include "InputField.h"
#include "Renderer.h"
#include <iostream>

InputField::InputField(int width, int height, int originX, int originY)
    : bounds{ originX, originY, width, height } {
}

void InputField::deleteSelection() {
    size_t start = std::min(cursorPosition, selectionAnchor);
    size_t end = std::max(cursorPosition, selectionAnchor);
    text.erase(start, end - start);
    cursorPosition = selectionAnchor = start;
}

bool InputField::hasSelection() const {
    return cursorPosition != selectionAnchor;
}

void InputField::addCharacter(char c) {
    if (hasSelection())
        deleteSelection();

    text.insert(cursorPosition, 1, c);
    cursorPosition++;
    selectionAnchor = cursorPosition;
}

void InputField::removeCharacter() {
    if (hasSelection()) {
        deleteSelection();
    }
    else if (cursorPosition > 0) {
        text.erase(cursorPosition - 1, 1);
        cursorPosition--;
        selectionAnchor = cursorPosition;
    }
}

const std::string& InputField::getText() const {
    return text;
}

bool InputField::isClickedOn(int mx, int my) const {
    return gmt::isClickedOn(bounds, mx, my);
}

void InputField::onClick(int mx, int my) {
    selectionAnchor = cursorPosition;
}

void InputField::onChar(char c) {
    if (c >= '0' && c <= '9' && text.size() < 3)
        addCharacter(c);
}

void InputField::onBackspace() {
    removeCharacter();
}

std::string InputField::getValue() const {
    return text;
}

void InputField::draw(Renderer& r, bool focused) {
    const int pad = 6;
    const int top = bounds.y;

    Color border = focused ? Color{ 51, 128, 255, 255 } : Color{ 153, 153, 153, 255 };
    r.fillRect(bounds.x, top, bounds.width, bounds.height, border);
    r.fillRect(bounds.x + 1, top + 1, bounds.width - 2, bounds.height - 2, { 255, 255, 255, 255 });

    const int textX = bounds.x + pad;
    const int baseline = top + (bounds.height + r.textHeight()) / 2;

    if (hasSelection()) {
        size_t s = std::min(cursorPosition, selectionAnchor);
        size_t e = std::max(cursorPosition, selectionAnchor);
        int x1 = textX + r.measureText(text.substr(0, s));
        int x2 = textX + r.measureText(text.substr(0, e));
        r.fillRect(x1, top + 4, x2 - x1, bounds.height - 8, { 179, 204, 255, 255 });
    }

    r.drawText(text, textX, baseline, { 0, 0, 0, 255 });

    if (focused) {
        int cx = textX + r.measureText(text.substr(0, cursorPosition));
        r.fillRect(cx, top + 4, 2, bounds.height - 8, { 0, 0, 0, 255 });
    }
}

void InputField::setValue(const std::string& v) {
    text = v;
    cursorPosition = selectionAnchor = text.size();
}