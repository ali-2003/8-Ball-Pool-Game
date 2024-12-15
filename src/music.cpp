#include "music.h"
#include "exception.h"


Sound::Sound() : gChunk{nullptr} {}

Sound::Sound(const std::string& path)
    : Sound()
{
    gChunk = Mix_LoadWAV(path.c_str());

    if (gChunk == nullptr)
        throw SDLException(std::string("Could not load sound! SDL_mixer Error: ") + Mix_GetError());
}

Sound::~Sound() noexcept
{
    if (gChunk != nullptr)
        Mix_FreeChunk(gChunk);
}

void Sound::play()
{
    if (gChunk != nullptr)
        Mix_PlayChannel( -1, gChunk, 0 );
}



