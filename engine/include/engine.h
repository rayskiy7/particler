#pragma once

#include <vector>

#include <settings.h>

#include "../int/handler.h"
#include "../int/queue.h"
#include "../int/worker.h"
#include "../int/particles.h"

class PhysicsEngine
{
    std::shared_ptr<Particles> pts;
    std::shared_ptr<Queue<Point>> spawn_q;
    std::shared_ptr<Queue<PID>> kill_q;

    Handler handler;
    std::vector<std::unique_ptr<Worker>> workers;

public:
    PhysicsEngine();
    void run_async();
    void for_each_particle(void (*f)(float, float, float, float, float, float));
    void interact(float x, float y);
    void stop_async();
    ~PhysicsEngine();
};
