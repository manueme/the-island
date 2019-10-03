#pragma once
#include <SDL_hints.h>

// Initial code from: http://lazyfoo.net/SDL_tutorials/lesson14/index.php

class Timer {
public:
    Timer();

    void start();
    void stop();
    void pause();
    void unPause();

    Uint32 getTicks() const;

    bool isStarted() const;
    bool isPaused() const;

private:
    Uint32 startTicks;
    Uint32 pausedTicks;
    bool paused;
    bool started;
};
