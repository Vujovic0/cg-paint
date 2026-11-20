#pragma once

namespace gmt {
    struct Rect {
        int x, y;
        int width, height;
    };

    bool isClickedOn(const Rect& r, int mx, int my);
}