#include <vector>
#include <utility>

#include <settings.h>

#include "queue.h"
#include "particles.h"
#include "handler.h"

Handler::Handler(std::shared_ptr<Particles> pts, std::shared_ptr<Queue<Point>> spawn_q, std::shared_ptr<Queue<PID>> kill_q)
    : pts{pts}, spawn_q{spawn_q}, kill_q{kill_q}, free_stack(N_PARTICLES)
{
    running.clear(std::memory_order_relaxed);

    for (int i = 0; i < N_PARTICLES; ++i)
        free_stack[i] = N_PARTICLES - 1 - i;
}

void Handler::run_async()
{
    running.test_and_set(std::memory_order_relaxed);
    thread = std::thread{&Handler::handle, this};
}

void Handler::stop_async()
{
    running.clear(std::memory_order_relaxed);
}

Handler::~Handler()
{
    stop_async();
    if (thread.joinable())
        thread.join();
}



inline void Handler::kill(PID pid)
{
    pts->st[pid].store(PState::DEAD, std::memory_order_relaxed);
    free_stack.push_back(pid);
}

void Handler::kill_all()
{
    if (!kill_q->ready())
        return;
    PID first = kill_q->pop(), last = first;
    kill(first);
    while (kill_q->ready())
    {
        last = kill_q->pop();
        kill(last);
    }
    pts->next[pts->prev[first]] = pts->next[last];
    pts->prev[pts->next[last]] = pts->prev[first];
}

void Handler::slay_oldest(int n)
{
    PID last = pts->first;
    while (n-->0 && last!=-1){
        kill_q->push(last);
        last = pts->next[last];
    }
}

void Handler::spawn_all()
{
    int need_particles = spawn_q->n_ready() * BLAST_SIZE;
    int can_spawn = free_stack.size();
    while (spawn_q->ready() && can_spawn >= BLAST_SIZE){
        spawn_blast(spawn_q->pop());
        need_particles -= BLAST_SIZE;
        can_spawn -= BLAST_SIZE;
    }
    slay_oldest(need_particles - can_spawn);
}

void Handler::spawn_blast(Point p){
    for (int i=0;i<BLAST_SIZE;++i)
    {
        auto pid = free_stack.back(); free_stack.pop_back();
        pts->next[pts->last] = pid;
        pts->prev[pid] = pts->last;
        pts->last = pid;
        pts->phy[pid] = p;
        pts->clr[pid] = random_hsv();
        pts->st[pid].store(PState::SPAWN, std::memory_order_release);
    }
    pts->next[pts->last] = -1;
}

void Handler::handle()
{
    while (running.test(std::memory_order_relaxed))
    {
        kill_all();
        spawn_all();
    }
}
