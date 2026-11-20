#pragma once
#include <string>

class Renderer;

class Component {
public:
    virtual ~Component() = default;
    virtual bool isClickedOn(int mx, int my) const = 0;
    virtual void onClick(int mx, int my) {}
    virtual void onChar(char c) {}
    virtual void onBackspace() {}
    virtual void draw(Renderer& r, bool focused) = 0;
    virtual std::string getValue() const { return ""; }
};