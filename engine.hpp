#pragma once
#include <map>

struct RenderContext_t
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* main_font;

    int width;
    int height;
};

struct AudioContext
{
    bool playing;
    char* currentFile;

    uint32_t wavLength;
    uint8_t* wavBuffer;
};

struct Asset_t
{
    SDL_Texture* texture;
};

class Engine
{
public:
    void CreateRenderer(int width, int height);

    Asset_t CreateTexture(const char* path);
    void DestroyTexture(const char* path);
    void DestroyTexture(uint32_t key);
    bool HasTexture(uint32_t key, Asset_t* texture);

    void PlayAudio(char* file);
    void StopAudio();

    bool IsRunning() const noexcept;
    void QuitGame() noexcept;

    void Destroy() noexcept;

    RenderContext_t RenderContext();

private:
    RenderContext_t m_renderContext;
    AudioContext m_audioContext;

    std::map<uint32_t, Asset_t> m_assets;

    SDL_AudioDeviceID m_currentAudioDevice;

    bool m_running = false;
public:
    static Engine* Instance()
    {
        static Engine engine;
        return &engine;
    }
private:
    Engine() { }
};
