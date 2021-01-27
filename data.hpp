#include <stdint.h>
#include <cstddef>
#include <vector>

struct GameData
{
    uint8_t padding[80];
    uint32_t numRooms;
};

enum GameLineType
{
    GAME_LINE_TYPE_TEXT, // show story text and click continues to next line
    GAME_LINE_TYPE_ANSWER // TODO
};

struct GameLine
{
    char text[72];
    GameLineType type;
};

struct GameRoom
{
    char background[30];
    char music[30];
    uint32_t numLines;
};

class Room
{
public:
    Room(GameRoom room);

    void AddLine(GameLine line);
    GameLine GetCurrentLine();

    char m_background[30];
    char m_music[30];

    int m_numLines;
    int m_currentLine = 0;

private:
    std::vector<GameLine> m_lines;
};
