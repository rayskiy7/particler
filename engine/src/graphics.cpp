#include <settings.h>

#include "particles.h"
#include "graphics.h"

thread_local std::mt19937 gen(SEED);

Color hsv2rgb(Color color) {
    auto [h,s,v] = color;
    float c = v * s;
    float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r1=0, g1=0, b1=0;
    if      (h < 60)  { r1=c; g1=x; b1=0; }
    else if (h < 120) { r1=x; g1=c; b1=0; }
    else if (h < 180) { r1=0; g1=c; b1=x; }
    else if (h < 240) { r1=0; g1=x; b1=c; }
    else if (h < 300) { r1=x; g1=0; b1=c; }
    else              { r1=c; g1=0; b1=x; }

    return { r1 + m, g1 + m, b1 + m };
}

// not thread-safe
Color random_hsv() {
    static std::uniform_real_distribution<float> uni_hue(0.0f, 360.0f);
    static std::uniform_real_distribution<float> uni_bright(0.5f, 1.0f);
    static std::normal_distribution<float> normal_hue(0.0f, 20.0f); // σ = 20°
    static int call_number = 0;
    static float targetHue = uni_hue(gen);

    float h;
    if constexpr(COLOR_MODE == CMode::UNIFORM) {
        h = uni_hue(gen);
    } else {
        if (call_number % BLAST_SIZE == 0) {
            targetHue = uni_hue(gen);
        }
        float dh = normal_hue(gen);
        h = std::fmod(targetHue + dh + 360.0f, 360.0f);
        ++call_number;
    }

    float s = uni_bright(gen);
    float v = uni_bright(gen);

    return Color{h,s,v};
}

// thread-safe
Point random_v2(){
    thread_local static std::uniform_real_distribution<float> angle_dist(0.f, 2.f * 3.14159265f);
    thread_local static std::uniform_real_distribution<float> speed_dist(MIN_SPEED, MAX_SPEED);

    float theta = angle_dist(gen);
    float speed = speed_dist(gen);
    return { std::cos(theta) * speed, std::sin(theta) * speed };
}

Destiny random_dst(){
    thread_local static std::uniform_real_distribution<float> dice(0.f, 1.f);
    float result = dice(gen);
    if (result<FADE_PROBABILITY)
        return Destiny::DIE;
    else if (result<FADE_PROBABILITY+REPLICATION_PROBABILITY)
        return Destiny::BLAST;
    else
        return Destiny::LIVE;
}