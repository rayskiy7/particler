#include <iostream>

#include <settings.h>
#include <engine.h>

PhysicsEngine::PhysicsEngine()
    : pts{std::make_shared<Particles>()},
    spawn_q{std::make_shared<Queue<Point>>(SPAWN_QUEUE_SIZE, []{std::cerr<<"Skip spawn of a new blast";})},
    kill_q{std::make_shared<Queue<PID>>(N_PARTICLES, []{std::cerr<<"Skip kill queue push";})},
    handler{pts, spawn_q, kill_q},
    workers{}
{
    for (int i = 0; i < WORKER_THREADS; ++i)
        workers.push_back(std::make_unique<Worker>(i, pts, kill_q, spawn_q));
};

PhysicsEngine::~PhysicsEngine(){
    stop_async();
}

void PhysicsEngine::run_async() {
    handler.run_async();
    for (int i=0; i<WORKER_THREADS; ++i)
        workers[i]->run_async();
};

void PhysicsEngine::for_each_particle(void (*f)(float, float, float, float, float, float)) {
    for (auto &w: workers){
        w->for_each(f);
    }
};

void PhysicsEngine::interact(float x, float y) {
    spawn_q->push({x, y});
};

void PhysicsEngine::stop_async() {
    for (int i=WORKER_THREADS; i>0; --i)
        workers[i-1]->stop_async();
    handler.stop_async();
};
