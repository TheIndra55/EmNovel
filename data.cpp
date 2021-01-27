#include "data.hpp"
#include <stdio.h>

Room::Room(GameRoom room)
{
    m_numLines = room.numLines;

    strcpy(m_background, room.background);
    strcpy(m_music, room.music);
}

void Room::AddLine(GameLine line)
{
    m_lines.push_back(line);
}

GameLine Room::GetCurrentLine()
{
    return m_lines.at(m_currentLine);
}
