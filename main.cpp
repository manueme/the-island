#include <iostream>
#include "MainWindow.h"
#include "Timer.h"

int main(int argc, char* args[])
{
    auto mainWindow = make_unique<MainWindow>();

    if (!mainWindow->show()) {
        printf("Failed to initialize!\n");
    } else {
        auto quit = false;

        SDL_Event e;

        Timer fps;

        while (!quit) {
            fps.start();

            while (SDL_PollEvent(&e) != 0) {
                switch (e.type) {
                case SDL_QUIT: {
                    quit = true;
                }
                break;
                case SDL_KEYDOWN: {
                    mainWindow->propagateKeyPressed(e.key.keysym.sym);
                }
                break;
                case SDL_KEYUP: {
                    mainWindow->propagateKeyUp(e.key.keysym.sym);
                }
                break;
                case SDL_MOUSEMOTION: {
                    mainWindow->propagateMouseMoved(e.motion.x, e.motion.y);
                }
                break;
                case SDL_MOUSEBUTTONDOWN: {
                    mainWindow->propagateMouse(static_cast<MouseButtonCode>(e.button.button), e.motion.x, e.motion.y);
                }
                break;
                default: ;
                }
            }
            mainWindow->propagateUpdate();
            mainWindow->propagateRender();

            if (fps.getTicks() < 1000 / constants::SCREEN_FPS) {
                SDL_Delay(1000 / constants::SCREEN_FPS - fps.getTicks());
            }
        }
    }
    return 0;
}
