#pragma once
#include <chrono>
#include <thread>
#include <math.h>
#include <iostream>

using namespace std::chrono;
using Clock = std::chrono::steady_clock;


inline void regulate_main_loop_timing(Clock::time_point &loop_start, int target_us)
{
    static long elapsed_us ;
    
    
    elapsed_us = duration_cast<microseconds>(Clock::now() - loop_start).count();

    if (target_us - elapsed_us > 500)
        std::this_thread::sleep_for(microseconds( (target_us - elapsed_us) - 500 ));

    while (elapsed_us < target_us)
    {
        elapsed_us = duration_cast<microseconds>(Clock::now() - loop_start).count();
    }

    loop_start = Clock::now();

    if (elapsed_us > target_us + 150)
        std::cout << "  elapsed_us LONG !!!!!!!!!!!! " << elapsed_us << std::endl;
}


inline void regulate_delta_from_sync_timing(
    const Clock::time_point &sync_start,
    int target_us)
{
    static long elapsed_us;

    elapsed_us =
        duration_cast<microseconds>(
            Clock::now() - sync_start).count();

    // sleep most of remaining time
    if ((target_us - elapsed_us) > 500)
    {
        std::this_thread::sleep_for(
            microseconds(
                (target_us - elapsed_us) - 500));
    }

    // spin final few hundred us
    while (elapsed_us < target_us)
    {
        elapsed_us =
            duration_cast<microseconds>(
                Clock::now() - sync_start).count();
    }

    if (elapsed_us > target_us * 1.1)
    {
        std::cout
            << "sync timing LONG !!!!!  "
            << elapsed_us
            << std::endl;
    }
}



// Sleep most of the interval, spin the tail for better accuracy.
// NOTE: still not RT; for sub-millisecond accuracy consider OS timers or RT tuning.
inline void Delay_Msec(double msec)
{
    if (msec <= 0.0) return;

    const auto start = Clock::now();
    const auto target = start + duration_cast<Clock::duration>(duration<double, std::milli>(msec));

    // Sleep for most of it
    // Leave ~0.3 ms margin for spin (tweakable).
    static constexpr double spin_margin_ms = 0.3;
    const double sleep_ms = std::max(0.0, msec - spin_margin_ms);

    if (sleep_ms > 0.0) {
        std::this_thread::sleep_for(duration<double, std::milli>(sleep_ms));
    }

    // Spin until target
    while (Clock::now() < target) {
        // optional pause/yield on some platforms
        // std::this_thread::yield();
    }
}



// Sleep most of the interval, spin the tail for better accuracy.
// Delay in MICROSECONDS.
inline void Delay_Usec(double usec)
{
    if (usec <= 0.0)
        return;

    const auto start = Clock::now();

    const auto target =
        start + duration_cast<Clock::duration>(
                    duration<double, std::micro>(usec));

    // Leave ~100 us for spin (tweakable)
    static constexpr double spin_margin_us = 100.0;

    const double sleep_us =
        std::max(0.0, usec - spin_margin_us);

    if (sleep_us > 0.0)
    {
        std::this_thread::sleep_for(
            duration<double, std::micro>(sleep_us));
    }

    // spin until target
    while (Clock::now() < target)
    {
        // optional:
        // std::this_thread::yield();
    }
}


