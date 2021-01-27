#pragma once
#include <vector>
#include "data.hpp"

void DisplayGameElements();
void RenderLineText(RenderContext_t context, GameLine line);
void CheckRoomMusic();

extern Asset_t gTextBox;

class Game
{
public:
    void ReadGameData(const char* path);
    Room* GetCurrentRoom();

    GameData m_data;
    std::vector<Room> m_rooms;

    int m_currentRoom = 0;
    int m_currentLine = 0;

    char* m_currentAudio;
private:

public:
    static Game* Instance()
    {
        static Game game;
        return &game;
    }
private:
    Game() { }
};
