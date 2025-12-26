#pragma once

#include <vector>

#include "queue.h"
#include "particles.h"


class Handler
{
    std::shared_ptr<Particles> pts;
    std::shared_ptr<Queue<Point>> spawn_q;
    std::shared_ptr<Queue<PID>> kill_q;

    std::vector<PID> free_stack;

    std::atomic_flag running;

    void handle();
    void kill(PID pid);
    void kill_all();
    void slay_oldest(int n);
    void spawn_blast(Point p);
    void spawn_all();
    
    std::thread thread;

public:
    Handler(std::shared_ptr<Particles> pp, std::shared_ptr<Queue<Point>> sq, std::shared_ptr<Queue<PID>> kq);
    void run_async();
    void stop_async();
    ~Handler();
};
