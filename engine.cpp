#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_audio.h>

#include <stdio.h>
#include <sstream>

#if _WIN32
#include <Windows.h>
#else
#include <emscripten.h>
#endif

#include "engine.hpp"
#include "util.hpp"

void Engine::CreateRenderer(int width, int height)
{
    m_renderContext.window = SDL_CreateWindow("engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    SDL_GL_GetDrawableSize(m_renderContext.window, &m_renderContext.width, &m_renderContext.height);

    m_renderContext.renderer = SDL_CreateRenderer(m_renderContext.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    auto font = OpenFile("data/font.ttf");
    m_renderContext.main_font = TTF_OpenFontRW(font, 1, 28);
    m_running = true;
}

void Engine::OpenArchive()
{
    m_archive = new Archive("game.dat");
}

Archive* Engine::GetArchive() const noexcept
{
    return m_archive;
}

SDL_RWops* Engine::OpenFile(const char* path) const
{
    auto size = GetArchive()->GetFileSize(path);

    // check if file exists
    if (!size)
    {
        std::ostringstream err;
        err << "Failed to open " << path;

        FatalError(err.str());
    }

    // read file from archive
    auto data = new char[size];
    GetArchive()->ReadFile(path, data);

    return SDL_RWFromMem(data, size);
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

    // read file from archive
    auto file = OpenFile(path);

    // load image
    SDL_Surface* image = IMG_Load_RW(file, 1);

    if(image == nullptr)
    {
        std::ostringstream err;
        err << "Failed to load texture " << path << " " << SDL_GetError();

        FatalError(err.str());
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
    auto audio = OpenFile(file);
    SDL_LoadWAV_RW(audio, 1, &wavSpec, &m_audioContext.wavBuffer, &m_audioContext.wavLength);

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

    m_archive->~Archive();

    SDL_DestroyRenderer(m_renderContext.renderer);
    SDL_DestroyWindow(m_renderContext.window);
    TTF_CloseFont(m_renderContext.main_font);
}

void Engine::FatalError(std::string err) const
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error", err.c_str(), m_renderContext.window);

#if _WIN32
    ExitProcess(1);
#else
    emscripten_force_exit(1);
#endif
}
