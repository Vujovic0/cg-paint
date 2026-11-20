// controls.cpp
#include "controls.h"
#include <iostream>

void printControls() {
    std::cout <<
        "==================== MiniPaint: komande ====================\n"
        "\n"
        "MIS\n"
        "  Levi klik / prevlacenje     crtanje po kanvasu\n"
        "  Srednji klik + prevlacenje  pomeranje (pan) kanvasa\n"
        "  Tockic                      promena velicine cetkice\n"
        "  Ctrl + tockic               zumiranje kanvasa\n"
        "  Prevlacenje ivice kanvasa   promena dimenzija kanvasa\n"
        "\n"
        "TASTATURA\n"
        "  1 / 2 / 3                   oblik cetkice: krug / trougao / kvadrat\n"
        "  H                           horizontalno okretanje kanvasa\n"
        "  V                           vertikalno okretanje kanvasa\n"
        "  R                           rotacija kanvasa u smeru kazaljke na satu\n"
        "  Shift + R                   rotacija suprotno od smera kazaljke na satu\n"
        "  E                           cetkica / gumica\n"
        "  Esc                         izlaz iz aplikacije\n"
        "\n"
        "TOOLBAR\n"
        "  R, G, B, A polja            unos boje (0-255)\n"
        "  OK dugme ili Enter          primena boje na cetkicu\n"
        "\n"
        "============================================================\n"
        << std::endl;
}