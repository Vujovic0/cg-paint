#include "Geometry.h"

namespace gmt {
    bool isClickedOn(const Rect& r, int mx, int my) {
        return mx >= r.x && mx <= r.x + r.width &&
            my >= r.y && my <= r.y + r.height;
    }
}
