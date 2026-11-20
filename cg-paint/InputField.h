#pragma once
#include <string>
#include <algorithm>
#include "Geometry.h"
#include "Component.h"
#include "Renderer.h"

class InputField : public Component {
private:
    gmt::Rect bounds;
    std::string text;
    size_t cursorPosition = 0;
    size_t selectionAnchor = 0;

    void deleteSelection();
    bool hasSelection() const;
public:
    InputField(int width, int height, int originX, int originY);

    void addCharacter(char c);
    void removeCharacter();
    const std::string& getText() const;

    bool isClickedOn(int mx, int my) const override;
    void onClick(int mx, int my) override;
    void onChar(char c) override;
    void onBackspace() override;
    void draw(Renderer& r, bool focused) override;
    std::string getValue() const override;
    void setValue(const std::string& v) override;
};