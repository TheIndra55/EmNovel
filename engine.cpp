#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_audio.h>

#include "engine.hpp"
#include "util.hpp"

#include <stdio.h>

void Engine::CreateRenderer(int width, int height)
{
    m_renderContext.window = SDL_CreateWindow("engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    SDL_GL_GetDrawableSize(m_renderContext.window, &m_renderContext.width, &m_renderContext.height);

    m_renderContext.renderer = SDL_CreateRenderer(m_renderContext.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    m_renderContext.main_font = TTF_OpenFont("data/font.ttf", 28);
    m_running = true;
}

RenderContext_t Engine::RenderContext()
{
    return m_renderContext;
}

Asset_t Engine::CreateTexture(const char* path)
{
    printf("Engine::CreateTexture: loading %s\n", path);

    uint32_t hash = jenkins_one_at_a_time_hash(path, strlen(path));
    Asset_t asset;

    SDL_Surface* image = IMG_Load(path);
    if(image == NULL)
    {
        printf("Failed to load texture %s: %s\n", path, SDL_GetError());
    }

    asset.texture = SDL_CreateTextureFromSurface(m_renderContext.renderer, image);
    SDL_FreeSurface(image);

    m_assets.insert(std::make_pair(hash, asset));
    return asset;
}

void Engine::DestroyTexture(const char* path)
{
    uint32_t hash = jenkins_one_at_a_time_hash(path, strlen(path));
    DestroyTexture(hash);
}

void Engine::DestroyTexture(uint32_t key)
{
    auto iter = m_assets.find(key);
    if (iter != m_assets.end() )
    {
        SDL_DestroyTexture(iter->second.texture);
        m_assets.erase(key);
    }
}

bool Engine::HasTexture(uint32_t key, Asset_t* texture)
{
    auto iter = m_assets.find(key);
    if (iter != m_assets.end() )
    {
        *texture = iter->second;
        return true;
    }

    return false;
}

void Engine::PlayAudio(char* file)
{
    SDL_AudioSpec wavSpec;
 
    printf("%s\n", file);

    // TODO look into SDL_Mixer or maybe sound compression since nobody likes 14mb sounds
    SDL_LoadWAV(file, &wavSpec, &m_audioContext.wavBuffer, &m_audioContext.wavLength);

    if(!m_currentAudioDevice)
    {
        m_currentAudioDevice = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
        if(m_currentAudioDevice == 0)
        {
            printf("Failed to open audio device: %s\n", SDL_GetError());
        }
    }

    m_audioContext.playing = true;
    m_audioContext.currentFile = file;

    SDL_QueueAudio(m_currentAudioDevice, m_audioContext.wavBuffer, m_audioContext.wavLength);
    SDL_PauseAudioDevice(m_currentAudioDevice, 0);
}

void Engine::StopAudio()
{
    SDL_ClearQueuedAudio(m_currentAudioDevice);

    m_audioContext.playing = false;
    SDL_FreeWAV(m_audioContext.wavBuffer);
}

void Engine::QuitGame() noexcept
{
    m_running = false;
}

bool Engine::IsRunning() const noexcept
{
    return m_running;
}

void Engine::Destroy() noexcept
{
    StopAudio();

    SDL_DestroyRenderer(m_renderContext.renderer);
    SDL_DestroyWindow(m_renderContext.window);
    TTF_CloseFont(m_renderContext.main_font);
}
