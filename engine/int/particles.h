#pragma once

#include <utility>
#include <tuple>
#include <atomic>

#include <settings.h>

#include "graphics.h"

enum class PState: unsigned char {DEAD, SPAWN, ALIVE};
// FSM gparh: DEAD -h-> SPAWN -w-> ALIVE -h-> DEAD

using PID = int; // Particle ID

struct Particles {
    std::atomic<PState> st[N_PARTICLES];
    TSt ts[N_PARTICLES];
    Point pos[3][N_PARTICLES];
    Point phy[N_PARTICLES]; // spawn position or regular velocity
    Color clr[N_PARTICLES];

    // LL for the oldest
    PID first = -1;
    PID next[N_PARTICLES];
    PID last = -1;
    PID prev[N_PARTICLES];

    Particles() {
        for (int i=0; i<N_PARTICLES; ++i)
            st[i].store(PState::DEAD, std::memory_order_relaxed);
    }
};
