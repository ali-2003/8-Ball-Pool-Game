#ifndef MUSIC_H
#define MUSIC_H

#include <SDL_mixer.h>
#include <string>


// SDL_mixer Mix_Chunk wrapper
class Sound {
    Mix_Chunk* gChunk;
public:
    Sound();
    Sound(const std::string& path);
    ~Sound() noexcept;

    Sound(const Sound&) = delete;
    Sound& operator=(const Sound&) = delete;
    Sound(Sound&&) noexcept = default;
    Sound& operator=(Sound&&) noexcept = default;

    void play();
};


#endif // MUSIC_H
