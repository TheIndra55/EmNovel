#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#ifndef _WIN32
#include <emscripten.h>
#else
#include <Windows.h>
#endif

#include "engine.hpp"
#include "game.hpp"
#include "util.hpp"

void mainloop()
{
    Engine* engine = Engine::Instance();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            Game* game = Game::Instance();
            Room* room = game->GetCurrentRoom();

            if(room->m_currentLine++ >= (room->m_numLines - 1))
            {
                if((game->m_currentRoom + 1) >= (game->m_data.numRooms))
                {
                    room->m_currentLine--;

                    printf("no more rooms\n");

                    continue;
                }

                game->m_currentRoom++;
            }
        }

        if (event.type == SDL_QUIT)
        {
            engine->QuitGame();
        }
    }

    CheckRoomMusic();

    RenderContext_t context = engine->RenderContext();

    SDL_RenderClear(context.renderer);

    DisplayGameElements();

    SDL_RenderPresent(context.renderer);
}

#define GAME_DAT "data/game.dat"

#ifndef _WIN32
int main()
#else
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
#endif
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();

    Engine* engine = Engine::Instance();

    try
    {
        engine->OpenArchive();
    }
    catch (std::exception& e)
    {
        engine->FatalError("Failed to open game archive game.dat");
    }

    engine->CreateRenderer(960, 540);

    gTextBox = engine->CreateTexture("data/textbox.png");

    printf("Game is " GAME_DAT "\n");

    Game* game = Game::Instance();
    game->ReadGameData(GAME_DAT);

    printf("game has %d rooms\n", game->m_data.numRooms);

#ifndef _WIN32
    emscripten_set_main_loop(mainloop, -1, 1);
#else
    while (engine->IsRunning())
    {
        mainloop();

        SDL_Delay(10);
    }

    engine->Destroy();
#endif

    return 0;
}
