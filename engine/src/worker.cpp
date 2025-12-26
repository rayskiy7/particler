#include <algorithm>
#include <utility>
#include <chrono>

#include <settings.h>

#include "particles.h"
#include "worker.h"
#include "queue.h"
#include "graphics.h"

inline TSt now(){
    return std::chrono::steady_clock::now();
}

Worker::Worker(int id, std::shared_ptr<Particles> pts, std::shared_ptr<Queue<PID>> kill_q, std::shared_ptr<Queue<Point>> spawn_q)
    : id{id}, pts{pts}, kill_q{kill_q}, spawn_q{spawn_q}
{
    running.clear(std::memory_order_relaxed);
    st.store(states[3], std::memory_order_relaxed);
}

void Worker::run_async()
{
    last_calc = std::chrono::steady_clock::now();
    running.test_and_set(std::memory_order_relaxed);
    thread = std::thread{&Worker::work, this};
}

void Worker::stop_async()
{
    running.clear(std::memory_order_relaxed);
}

Worker::~Worker()
{
    stop_async();
    if (thread.joinable())
        thread.join();
}

void Worker::work()
{
    while (running.test(std::memory_order_relaxed))
    {
        auto start_st = st.load(std::memory_order_relaxed);
        refresh_context(start_st);

        if (dt < MIN_UPDATE_PERIOD)
            std::this_thread::sleep_for(MIN_UPDATE_PERIOD - dt);

        // cout << "Worker-" << id << " do loop. Context: " << dt << ", " << source << ", " << target << endl;

        for (PID pid = A; pid < B; ++pid)
        {
            switch (pts->st[pid].load(std::memory_order_acquire))
            {
            case PState::DEAD:
                continue;
            case PState::SPAWN:
                graph_spawn(pid);
                break;
            case PState::ALIVE:
                tick_simulation(pid);
                break;
            }
        }

        switch_buffers(start_st);
    }
}

void Worker::refresh_context(TState st)
{
    TSt llast = last_calc;
    last_calc = now();
    dt = last_calc - llast;
    std::tie(source, target) = get_buffer_indices(st);
}

std::pair<int, int> Worker::get_buffer_indices(TState st)
{
    int source, target;
    source = st.get_ind(BState::I);
    if (source == -1)
        source = st.get_ind(BState::R);
    target = st.get_ind(BState::O);
    return std::pair{source, target};
}

void Worker::graph_spawn(PID pid)
{
    pts->ts[pid] = now();
    pts->pos[target][pid].x = pts->phy[pid].x;
    pts->pos[target][pid].y = pts->phy[pid].y;
    pts->clr[pid] = hsv2rgb(pts->clr[pid]);
    Point rv = random_v2();
    pts->phy[pid].x = rv.x;
    pts->phy[pid].y = rv.y;
    pts->st[pid].store(PState::ALIVE, std::memory_order_release);
}

void Worker::tick_simulation(PID pid)
{
    TSt newts = now();
    pts->phy[pid].x += -WIND_STRENGTH * pts->phy[pid].x * dt.count() * TIME_FACTOR;
    pts->phy[pid].y -= (GRAVITY_STRENGTH - WIND_STRENGTH * pts->phy[pid].y) * dt.count() * TIME_FACTOR;

    float new_x = pts->pos[source][pid].x + pts->phy[pid].x * dt.count() * TIME_FACTOR;
    float new_y = pts->pos[source][pid].y + pts->phy[pid].y * dt.count() * TIME_FACTOR;
    if (new_x < 0 || new_y < 0 || new_x > SCREEN_WIDTH || new_y > SCREEN_HEIGHT)
        goto out_of_screen;

    pts->pos[target][pid].x = new_x;
    pts->pos[target][pid].y = new_y;

    if (newts - pts->ts[pid] > BLAST_MIN_LIFETIME)
    switch(random_dst()){
    case Destiny::DIE:
    out_of_screen:
        pts->st[pid].store(PState::DEAD, std::memory_order_relaxed);
        kill_q->push(pid);
        break;
    case Destiny::BLAST:
        pts->st[pid].store(PState::DEAD, std::memory_order_relaxed);
        kill_q->push(pid);
        spawn_q->push({pts->pos[target][pid].x, pts->pos[target][pid].y});
        break;
    case Destiny::LIVE:
        pts->ts[pid] = newts;
    }
}

void Worker::switch_buffers(TState start_st){
    TState next_w = worker_transfer[start_st.n_repr];
    TState next_r = render_transfer[start_st.n_repr];
    if(!st.compare_exchange_strong(start_st, next_w, std::memory_order_release))
        if(!st.compare_exchange_strong(next_r, worker_transfer[next_r.n_repr], std::memory_order_relaxed))
            throw std::runtime_error("Transfer (3buf) FSM fail");
}

void Worker::for_each(void (*f)(float, float, float, float, float, float)){
    auto last = st.load(std::memory_order_relaxed);
    while (!st.compare_exchange_weak(last, render_transfer[last.n_repr], std::memory_order_relaxed))
        ;
    auto readb = st.load(std::memory_order_acquire).get_ind(BState::R);
    for (int pid=A; pid<B; ++pid)
        if (pts->st[pid].load(std::memory_order_relaxed) == PState::ALIVE)
            f(
                pts->pos[readb][pid].x,
                pts->pos[readb][pid].y,
                pts->clr[pid].r,
                pts->clr[pid].g,
                pts->clr[pid].b,
                1.0
            );
}
