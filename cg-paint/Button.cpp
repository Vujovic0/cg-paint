#include "Button.h"
#include "Renderer.h"


void Button::draw(Renderer& r, bool focused) {
    r.fillRect(bounds.x, bounds.y, bounds.width, bounds.height, Color{ 200, 200, 200, 255 });

    int tw = r.measureText(label);
    int tx = bounds.x + (bounds.width - tw) / 2;
    int baseline = bounds.y + (bounds.height + r.textHeight()) / 2;
    r.drawText(label, tx, baseline, Color{ 0, 0, 0, 255 });
}