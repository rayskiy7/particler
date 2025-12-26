#pragma once

#include <random>
#include <thread>
#include <algorithm>
#include <chrono>

unsigned char calc_total_threads();
using TSt = std::chrono::steady_clock::time_point;
using Dur = std::chrono::duration<float, std::milli>;
using namespace std::chrono_literals;

enum class CMode { UNIFORM, NORMAL };

inline constexpr CMode COLOR_MODE = CMode::NORMAL;

inline const unsigned int SEED = std::random_device{}(); // can be set manually

inline constexpr float SCREEN_WIDTH = 1024, SCREEN_HEIGHT = 900;
static_assert(100 <= SCREEN_HEIGHT && SCREEN_HEIGHT <= SCREEN_WIDTH && SCREEN_WIDTH <= 4000);

inline constexpr unsigned char BLAST_SIZE = 64;
static_assert(10 <= BLAST_SIZE && BLAST_SIZE <= 100);

inline constexpr unsigned int MAX_BLASTS = 2048;
static_assert(10 <= MAX_BLASTS && MAX_BLASTS <= 5000);

inline constexpr unsigned int N_PARTICLES = MAX_BLASTS * BLAST_SIZE;

inline constexpr unsigned int SPAWN_QUEUE_SIZE = 2<<15;
static_assert(MAX_BLASTS <= SPAWN_QUEUE_SIZE && SPAWN_QUEUE_SIZE <= N_PARTICLES);

inline constexpr float MIN_SPEED = 0.2f, MAX_SPEED = 3.0f;
static_assert(0.01f <= MIN_SPEED && MIN_SPEED <= MAX_SPEED && MAX_SPEED <= 10.0f);

inline constexpr float TIME_FACTOR = 0.1f;
static_assert(0.01f <= TIME_FACTOR && TIME_FACTOR <= 10.0f);

inline constexpr float GRAVITY_STRENGTH = 0.08f;
static_assert(0.0f <= GRAVITY_STRENGTH && GRAVITY_STRENGTH <= 10.0f);

inline constexpr float WIND_STRENGTH = 0.001f;
static_assert(0.0f <= WIND_STRENGTH && WIND_STRENGTH <= 1.0f);

inline constexpr Dur BLAST_MIN_LIFETIME = 1s;
static_assert(100ms <= BLAST_MIN_LIFETIME && BLAST_MIN_LIFETIME <= 5s);

inline constexpr float FADE_PROBABILITY = 0.4f, REPLICATION_PROBABILITY = 0.05f;
static_assert(0.0f <= FADE_PROBABILITY && FADE_PROBABILITY <= 1.0f);
static_assert(0.0f <= REPLICATION_PROBABILITY && REPLICATION_PROBABILITY <= 1.0f);
static_assert(FADE_PROBABILITY + REPLICATION_PROBABILITY <= 1.0f);

inline constexpr Dur MIN_UPDATE_PERIOD = 0ms;
static_assert(0ms <= MIN_UPDATE_PERIOD && MIN_UPDATE_PERIOD <= 1s);

inline constexpr unsigned char TOTAL_LOGICAL_CORES_FALLBACK = 10;
static_assert(1 <= TOTAL_LOGICAL_CORES_FALLBACK && TOTAL_LOGICAL_CORES_FALLBACK <= 32);

inline const unsigned char TOTAL_THREADS = calc_total_threads();
inline const unsigned char WORKER_THREADS = TOTAL_THREADS - 2;

inline unsigned char get_hardware_logical_cores()
{
    unsigned int cores = std::thread::hardware_concurrency();
    return (cores == 0) ? TOTAL_LOGICAL_CORES_FALLBACK : cores;
}

inline unsigned char calc_total_threads()
{
    return std::max<unsigned char>(3, get_hardware_logical_cores());
}
