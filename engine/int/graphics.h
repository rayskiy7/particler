#pragma once

#include <random>
#include <cmath>

#include <settings.h>

#include "particles.h"

extern thread_local std::mt19937 gen;

struct Point{
    float x, y;
};

struct Color{
    float r, g, b;
};

enum class Destiny {DIE, BLAST, LIVE};

Color hsv2rgb(Color c);
Color random_hsv();
Point random_v2();
Destiny random_dst();
