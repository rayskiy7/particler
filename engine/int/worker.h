#pragma once

#include <chrono>

#include <settings.h>

#include "queue.h"
#include "graphics.h"

enum BState : unsigned char
{
    R, // this buffer is being read by render thread
    I, // this buffer contains fresh snapshot
    O, // this buffer is being filled by worker threads
    X, // this buffer was last read
};

struct TState
{
    BState b0 : 2;
    BState b1 : 2;
    BState b2 : 2;
    unsigned char n_repr;

    int get_ind(BState st){
        if (b0 == st) return 0;
        if (b1 == st) return 1;
        if (b2 == st) return 2;
        return -1;
    }
};

inline TState states[12] = {
    [0]  = {R, O, I, 0},  //  R  0  1
    [1]  = {X, O, R, 1},  // -1  0  R
    [2]  = {O, I, R, 2},  //  0  1  R
    [3]  = {O, R, X, 3},  //  0  R -1
    [4]  = {I, R, O, 4},  //  1  R  0
    [5]  = {R, X, O, 5},  //  R -1  0
    [6]  = {R, I, O, 6},  //  R  1  0
    [7]  = {X, R, O, 7},  // -1  R  0
    [8]  = {O, R, I, 8},  //  0  R  1
    [9]  = {O, X, R, 9},  //  0 -1  R
    [10] = {I, O, R, 10}, //  1  0  R
    [11] = {R, O, X, 11}, //  R  0 -1
};

inline TState worker_transfer[12] = {
    [0] = states[6],
    [1] = states[2],
    [2] = states[10],
    [3] = states[4],
    [4] = states[8],
    [5] = states[0],
    [6] = states[0],
    [7] = states[8],
    [8] = states[4],
    [9] = states[10],
    [10] = states[2],
    [11] = states[6],
};

inline TState render_transfer[12] = {
    [0] = states[1],
    [1] = states[1],
    [2] = states[3],
    [3] = states[3],
    [4] = states[5],
    [5] = states[5],
    [6] = states[7],
    [7] = states[7],
    [8] = states[9],
    [9] = states[9],
    [10] = states[11],
    [11] = states[11],
};


class Worker
{
    inline static const int base = N_PARTICLES / WORKER_THREADS;
    inline static const int rem = N_PARTICLES % WORKER_THREADS;

    const int id;
    const std::shared_ptr<Particles> pts;
    const int A = id * base + std::min(id, rem); // from A
    const int B = A + base + (id < rem);         //   to B
    const std::shared_ptr<Queue<PID>> kill_q;
    const std::shared_ptr<Queue<Point>> spawn_q;
    std::atomic<TState> st;

    // thread data
    std::thread thread;
    std::atomic_flag running;

    // tick context
    TSt last_calc;
    Dur dt;
    int source, target;

    void refresh_context(TState st);
    std::pair<int, int> get_buffer_indices(TState st);

    void work();
    void graph_spawn(PID pid);
    void tick_simulation(PID pid);
    void switch_buffers(TState);

public:
    Worker(int id, std::shared_ptr<Particles> pts, std::shared_ptr<Queue<PID>> kill_q, std::shared_ptr<Queue<Point>> spawn_q);
    void run_async();
    void stop_async();
    void for_each(void (*f)(float, float, float, float, float, float));
    ~Worker();
};