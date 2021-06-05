#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "engine.hpp"
#include "game.hpp"
#include "util.hpp"

Asset_t gTextBox;

void DisplayGameElements()
{
    Game* game = Game::Instance();
    Engine* engine = Engine::Instance();

    Room* room = game->GetCurrentRoom();
    uint32_t joaat = jenkins_one_at_a_time_hash(room->m_background, strlen(room->m_background));

    Asset_t texture;
    if(!engine->HasTexture(joaat, &texture))
    {
        printf("background for room %d not loaded, loading %s as %d\n", game->m_currentRoom, room->m_background, joaat);
        texture = engine->CreateTexture(room->m_background); // TODO images are never unloaded after use
    }

    RenderContext_t context = engine->RenderContext();

    SDL_Rect r = {0, 0, context.width, context.height};
    SDL_RenderCopy(context.renderer, texture.texture, NULL, &r);

    RenderLineText(context, room->GetCurrentLine());
}

void CheckRoomMusic()
{
    Game* game = Game::Instance();
    Engine* engine = Engine::Instance();
    Room* room = game->GetCurrentRoom();

    if(strlen(room->m_music) > 0)
    {
        if(room->m_music != game->m_currentAudio)
        {
            printf("play now %s\n", room->m_music);

            engine->StopAudio();

            game->m_currentAudio = room->m_music;
            engine->PlayAudio(room->m_music);
        }
    }
}

void RenderLineText(RenderContext_t context, GameLine line)
{
    SDL_Color textColor = {255, 255, 255, 0};
    SDL_Surface* text = TTF_RenderText_Blended(context.main_font, line.text, textColor);

    SDL_Rect text_rect = {(context.width / 2) - (text->w / 2), (500 - text->h), text->w, text->h};
    auto textTexture = SDL_CreateTextureFromSurface(context.renderer, text);

    SDL_Rect r = {0, (500 - text->h) - 10, context.width, text->h + 20};
    SDL_FreeSurface(text);

    SDL_RenderCopy(context.renderer, gTextBox.texture, NULL, &r);

    SDL_RenderCopy(context.renderer, textTexture, NULL, &text_rect);
    SDL_DestroyTexture(textTexture);
}

Room* Game::GetCurrentRoom()
{
    return &m_rooms.at(m_currentRoom);
}

void Game::ReadGameData(const char* path)
{
    auto engine = Engine::Instance();
    auto file = engine->OpenFile(path);

    file->read(file, &m_data, sizeof(GameData), 1);

    for(int i = 0; i < m_data.numRooms; i++)
    {
        GameRoom dataroom;
        file->read(file, &dataroom, sizeof(GameRoom), 1);

        printf("Room %d, background = %s, numLines = %d\n", i, dataroom.background, dataroom.numLines);
        
        Room room(dataroom);
        for(int j = 0; j < dataroom.numLines; j++)
        {
            GameLine line;
            file->read(file, &line, sizeof(GameLine), 1);

            room.AddLine(line);
        }

        m_rooms.push_back(room);
    }
}
