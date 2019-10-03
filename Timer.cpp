#include "Timer.h"
#include <SDL.h>

Timer::Timer()
{
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    started = true;
    paused = false;
    startTicks = SDL_GetTicks();
}

void Timer::stop()
{
    started = false;
    paused = false;
}

void Timer::pause()
{
    if (started && !paused) {
        paused = true;
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unPause()
{
    if (paused) {
        paused = false;
        startTicks = SDL_GetTicks() - pausedTicks;
        pausedTicks = 0;
    }
}

Uint32 Timer::getTicks() const
{
    if (started) {
        if (paused) {
            return pausedTicks;
        }
        return SDL_GetTicks() - startTicks;
    }
    return 0;
}

bool Timer::isStarted() const
{
    return started;
}

bool Timer::isPaused() const
{
    return paused;
}
