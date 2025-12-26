#pragma once

#include <atomic>

// ! can be optimized with &mask over %N for power of 2.

class Handler;

enum class CState : unsigned char
{
    FREE,
    RESERVED,
    WRITTEN
};

// MPSC (limited, ring)
template <class T>
class Queue
{
    size_t N;
    T *data;
    void (*on_overflow)();
    unsigned long long from;
    std::atomic<unsigned long long> to;
    std::atomic<CState> *st;

    bool ready();
    int n_ready();
    T pop();

    friend class Handler;

public:
    Queue(size_t, void (*on_of)());
    ~Queue();
    bool push(T t);
};

template <class T>
Queue<T>::Queue(size_t limit, void (*on_of)())
    : N{limit}, data{new T[N]}, on_overflow{on_of}, from{0}, to{0}, st{new std::atomic<CState>[N]}
{
    for (int i=0; i<N; ++i)
        st[i].store(CState::FREE, std::memory_order_relaxed);
}

template <class T>
Queue<T>::~Queue()
{
    delete[] st;
    delete[] data;
}

template <class T>
bool Queue<T>::ready()
{
    return st[from % N].load(std::memory_order_acquire) == CState::WRITTEN;
}

template <class T>
int Queue<T>::n_ready()
// does not prepare data for reading
{
    int k = 0, f = from;
    while (st[f++ % N].load(std::memory_order_relaxed) == CState::WRITTEN)
        k++;
    return k;
}

template <class T>
T Queue<T>::pop()
{
    T res = std::move(data[from % N]);
    st[from++ % N].store(CState::FREE, std::memory_order_release);
    return res;
}

template <class T>
bool Queue<T>::push(T t)
{
    auto c = to.fetch_add(1, std::memory_order_relaxed);
    CState expected = CState::FREE;
    if (st[c % N].compare_exchange_strong(expected, CState::RESERVED, std::memory_order_acq_rel))
    {
        data[c % N] = t;
        st[c % N].store(CState::WRITTEN, std::memory_order_release);
        return true;
    }
    to.fetch_sub(1, std::memory_order_relaxed);
    on_overflow();
    return false;
}